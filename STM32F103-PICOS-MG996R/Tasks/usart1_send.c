#include "usart1_send.h"

// ===================== Автомат передачи данных по USART =====================
switch(usart1_send_state) {
	case USART1_SEND_START: // Проверка на готовность передачи
		// Если есть, что передавать и передатчик готов к передаче
		if (!usart1_send_ready() && (USART1->SR & USART_SR_TXE)) 
			usart1_send_state = USART1_SEND_ANALYSIS; // Изменение состояния
		break;
	case USART1_SEND_ANALYSIS: // Проверка на конец строки 
		// Если это конец строки
		if (*((char*)usart1_send_rbuff[usart1_send_read_index]) == '\0') {
			RBUFF_SETNEXT(USART1_SEND_RBUFF_SIZE, usart1_send_read_index);
			usart1_send_state = USART1_SEND_START; // Изменение состояния
			break;
		}
		usart1_send_state = USART1_SEND_TRANSMISSION; // Изменение состояния
		break;
	case USART1_SEND_TRANSMISSION: // Передача данных
		// Передаём байт данных и сдвигаем указатель на следующий байт
		USART1->DR =*((char*)usart1_send_rbuff[usart1_send_read_index]);
		usart1_send_rbuff[usart1_send_read_index]++;
		usart1_send_state = USART1_SEND_START; // Изменение состояния
		break;
}