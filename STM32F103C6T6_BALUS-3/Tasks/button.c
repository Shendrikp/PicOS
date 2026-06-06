#include "button.h"

// ========================= Автомат обработки кнопки ==========================
switch(button_state) {
	case BUTTON_START:
		if (!button_timer) { // Если таймер сработал
			button_timer = 10; // Запуск таймера
			button_state = BUTTON_WORK; // Изменение состояния
		} break;
	case BUTTON_WORK:
		button_state = BUTTON_START; // Заранее возврат назад
		button_queue <<= 1; // Сдвиг очереди бит кнопки
		// Выставление крайнего бита текущего состояния
		if (GPIOA->IDR & GPIO_IDR_IDR0) button_queue |= 1;
		if (button_queue == 0xFF) { // Если кнопка нажата
			button_mode_switch++; // Необходимо изменить режим работы
			button_state = BUTTON_RESTART; // Изменение состояния
		} break;
	case BUTTON_RESTART: 
		// Если кнопка отжата, то перезапуск кнопки
		if (!(GPIOA->IDR & GPIO_IDR_IDR0)) {
			button_state = BUTTON_START;
			button_queue = 0;
		} break;
}

// ============== Автомат изменения режима работы ПИД-регулятора ===============
if (button_mode_switch) { // Если кнопка нажата
	button_mode_state++; // Изменение текущего режима работы
	if (button_mode_state > 3) button_mode_state = 0;
	// Выключение всех светодиодов режима
	GPIOA->BSRR = GPIO_BSRR_BR4|GPIO_BSRR_BR5|GPIO_BSRR_BR6;
	switch(button_mode_state) {
		case BUTTON_MODE_DISTANCE_65:
			target_distance = 65; // Новое желаемое расстояние до вагонетки
			GPIOA->BSRR = GPIO_BSRR_BS6;
			break;
		case BUTTON_MODE_DISTANCE_190:
			target_distance = 190; // Новое желаемое расстояние до вагонетки
			GPIOA->BSRR = GPIO_BSRR_BS5;
			break;
		case BUTTON_MODE_DISTANCE_335:
			target_distance = 335; // Новое желаемое расстояние до вагонетки
			GPIOA->BSRR = GPIO_BSRR_BS4;
			break;
		case BUTTON_MODE_DISTANCE_190_1:
			target_distance = 190; // Новое желаемое расстояние до вагонетки
			GPIOA->BSRR = GPIO_BSRR_BS5;
		break;
	}	
	button_mode_switch = 0;
}