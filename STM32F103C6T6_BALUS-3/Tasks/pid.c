#include "pid.h"

// ====================== Автомат работы ПИД-регулятора =======================
switch(pid_state) {
	case PID_WAIT:
		TIM2->CCR4 = servo_initial; // Сброс положения сервопривода
		pid_state = PID_START; // Изменение состояния
		break;
	case PID_RESET:
		pid_p = pid_i = pid_d = 0.0; // Сброс накопившихся значений
		pid_queue = 0; // Сброс накопившихся значений
		pid_state = PID_START; // Изменение состояния
		break;
	case PID_START:
		if (!pid_timer) { // Если время периода прошло
			if (pid_period < 10) pid_period = 10; // Ограничение дискретизации
			pid_timer = pid_period; // Запуск таймера
			distance_sensor_state = DISTANCE_SENSOR_START; // Запуск HC-SR04
			pid_state = PID_CHECK; // Изменение состояния
		} break;
	case PID_CHECK:
		// Если нет нового измерения
		if (distance_sensor_state != DISTANCE_SENSOR_END) break;
		if (distance_sensor_result_mm() > 450) { // Если расстояние некорректно
			pid_state = PID_RESET; // Изменение состояния
			break;
		}
		// Ограничение желаемого расстояния до вагонетки
		if (target_distance > 450) target_distance = 450;
		pid_state = PID_WORK; // Изменение состояния
		break;
	case PID_WORK:
		// Ошибка регулирования: желаемое значение v - действительное y
		pid_error = target_distance - distance_sensor_result_mm();
		pid_dt =  pid_period / 1000.0; // Период дескритезации регулятора, с
		// Дифференцирующий фильтр 2-го порядка
		filter_d2y = pid_error / (filter_T * filter_T) - 2 * filter_d * \
					filter_dy / filter_T - filter_y / (filter_T * filter_T);
		filter_dy += filter_d2y * pid_dt;
		filter_y += filter_dy * pid_dt;
		pid_p = pid_error; // Пропорциональная часть
		pid_i += pid_error * pid_dt; // Интегральная часть
		pid_d = filter_dy; // Дифференциальная часть
		pid_u = k_p * pid_p + k_i * pid_i + k_d * pid_d; // Управление u
		// Управляющее воздействие на сервопривод
		TIM2->CCR4 = servo_initial - pid_u;
		// Исправление некорректных значений
		if (TIM2->CCR4 > 4000) TIM2->CCR4 = 4000;
		else if (TIM2->CCR4 < 1500) TIM2->CCR4 = 1500;
		servo_drive_switch = TIM2->CCR4; // Запись управляющего воздействия
		// Обработка очереди окончания переходного процесса
		pid_queue <<= 1;
		if (pid_p > -20 && pid_p < 20) pid_queue |= 1;
		if (pid_queue == 0xFFFFFFFF) // Переходный процесс завершился?
			GPIOA->BSRR = GPIO_BSRR_BS7; // Да: включение светодиода
		else GPIOA->BSRR = GPIO_BSRR_BR7; // Нет: выключение светодиода
		pid_state = PID_START; // Изменение состояния
		break;
}