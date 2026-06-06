#include "usart1_recv.h"

// ==================== Автомат получения строки по USART1 ====================
switch(usart1_recv_state) {
	case USART1_RECV_START:
		if (USART1->SR & USART_SR_RXNE) { // Если пришёл байт в линию
			// Запись байта в буфер
			usart1_recv_buff[usart1_recv_index] = USART1->DR;
			// Если пришёл символ стирания, но удалять нечего
			if (usart1_recv_buff[0] == '\b') break;
			usart1_recv_state = USART1_RECV_OUTPUT; // Изменение состояния
		}
		break;
	case USART1_RECV_OUTPUT: // Вывод полученного символа на экран
		// Если возможна передача байта 
		if (USART1->SR & USART_SR_TXE) {
			// Вывод полученного символа на экран
			USART1->DR = usart1_recv_buff[usart1_recv_index];
			usart1_recv_state = USART1_RECV_ANALYSIS; // Изменение состояния
		}
		break;
	case USART1_RECV_ANALYSIS: // Обработка символа
		usart1_recv_state = USART1_RECV_START; // Изменение состояния
		// Какой именно символ пришёл
		switch(usart1_recv_buff[usart1_recv_index]) {
			case '\r': // Enter
				// Присвоение символа конца строки
				usart1_recv_buff[usart1_recv_index] = '\0';
				usart1_recv_index = 0; // Сброс индекса элемента буфера
				usart1_recv_state = USART1_RECV_END; // Завершение работы
				break;
			case '\b': // Backspace 
				// Сдвиг индекса на предыдущий элемент буфера
				usart1_recv_index--;
				break;
			default: // По умолчанию
				// Если индекс корректный
				if (usart1_recv_index < USART1_RECV_BUFF_SIZE)
					// Сдвиг индекса на следующий элемент буфера
					usart1_recv_index++; 
				break;
		}
}