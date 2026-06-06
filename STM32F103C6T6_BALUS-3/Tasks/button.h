#ifndef BUTTON
#define BUTTON
// Дата последней редакции: 28 апреля 2025 года

/* ======================== Краткое описание автомата =========================
	Обрабатывает нажатие кнопки. */
	
// ====================== Описание переменных автомата ========================
enum button_state { // Перечисление состояний автомата
	BUTTON_START,
	BUTTON_WORK,
	BUTTON_RESTART
};

enum button_mode_state { // Перечисление состояний автомата 
	BUTTON_MODE_DISTANCE_65 = 0, // 0, 1, 2, 3 - не менять
	BUTTON_MODE_DISTANCE_190 = 1, 
	BUTTON_MODE_DISTANCE_335 = 2,
	BUTTON_MODE_DISTANCE_190_1 = 3
	
};

uint8_t button_state = BUTTON_START; // Начальное состояние
uint8_t button_queue = 0; // Состояние кнопки: 11111111 - кнопка нажата
uint8_t button_mode_switch = 0; // Начальное состояние 
uint8_t button_mode_state = BUTTON_MODE_DISTANCE_190; // Режим работы
volatile uint16_t button_timer = 0; // Переменная-таймер для отсчета времени

#endif