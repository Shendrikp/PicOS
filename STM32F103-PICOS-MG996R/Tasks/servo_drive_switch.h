#ifndef SERVO_DRIVE_SWITCH
#define SERVO_DRIVE_SWITCH
//Дата последней редакции: 9 октября 2025 года

/* ======================== Краткое описание автомата =========================
	Создаёт управляющее воздействие на сервопривод. */
	
// ====================== Описание переменных автомата ========================
enum servo_drive_switch_status { // Перечисление состояний автомата
	SERVO_DRIVE_SWITCH_WORK
};

uint8_t servo_drive_switch_status = SERVO_DRIVE_SWITCH_WORK;
uint16_t servo_drive_switch = 0; // Угол поворота сервопривода

#endif