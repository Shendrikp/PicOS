#include "servo_drive_switch.h"

// ===================== Автомат управления сервоприводом =====================
switch(servo_drive_switch_status) {
	case SERVO_DRIVE_SWITCH_WORK:
		// Управляющее воздействие на сервопривод, в градусах
		if (servo_drive_switch > 180) servo_drive_switch = 180;
		TIM2->CCR4 = servo_drive_switch * 10.31 + 544;
		break;
}