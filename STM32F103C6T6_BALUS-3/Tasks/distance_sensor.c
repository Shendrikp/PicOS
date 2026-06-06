#include "distance_sensor.h"

// ================= Автомат измерения расстояния до объекта ===================
switch (distance_sensor_state) {
	case DISTANCE_SENSOR_START:
		GPIOB->BSRR = GPIO_BSRR_BS10; // Запуск дачтика HC-SR04
		delay_us(10 - 1); // Задержка около 10 мкс
		GPIOB->BSRR = GPIO_BSRR_BR10; // Выключение дачтика HC-SR04
		TIM1->CNT = 0; // Сброс таймера-счетчика 1
		TIM1->CR1 |= TIM_CR1_CEN; // Включение таймера-счетчика 1
		distance_sensor_state = DISTANCE_SENSOR_WORK; // Изменение состояния
		break;
	case DISTANCE_SENSOR_WORK:
		if (TIM1->SR & TIM_SR_UIF) { // Если произошло событие таймера
			TIM1->SR &= ~TIM_SR_UIF; // Сброс флага события таймера
			if (TIM1->SR & TIM_SR_CC2IF) { // Если получен задний фронт сигнала
				// Длительность сигнала, мкс (0.5 - шаг (квант) счёта)
				distance_sensor_value_mcs = (TIM1->CCR2 - TIM1->CCR1) * 0.5;
				// Измеренное расстояние, мм (длительность / 2 * скорость света)
				distance_sensor_value_mm = \
				(uint32_t)distance_sensor_value_mcs / 2 * 0.340;
				TIM1->CR1 &= ~TIM_CR1_CEN; // Выключение таймера-счетчика 1
				GPIOA->BSRR = GPIO_BSRR_BR7; // Сброс светодиода ошибки
				distance_sensor_state = DISTANCE_SENSOR_END; // Конец работы
			} else if (TIM1->SR & TIM_SR_CC1IF) break;
			else { // Если произошло переполнение таймера, повторить попытку
				distance_sensor_state = DISTANCE_SENSOR_START; // Перезапуск
				GPIOA->BSRR = GPIO_BSRR_BS7; // Включение светодиода ошибки
			}
		} break;
}
	
