#include "led_switch.h"

// ============== Автомата включения/выключения светодиода PC13 ===============
switch(led_state) {
	case LED_SWITCH_WORK:
		if (led_status) GPIOC->BSRR = GPIO_BSRR_BR13; // Включение светодиода
		else GPIOC->BSRR = GPIO_BSRR_BS13; // Выключение светодиода
		break;
}