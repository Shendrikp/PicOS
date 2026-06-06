#include "var_get.h"

// =============== Автомат вывода значений переменных в USART1 ================
switch(var_get_state) {
	case VAR_GET_START: // Включение подсистемы вывода
		usart1_send_state = USART1_SEND_START; // Включение usart1_send
		// Вывод стандартного шаблона
		usart1_send_str(var_table[var_get_index].name);
		usart1_send_str(" = ");
		var_get_state = VAR_GET_ANALYSIS; // Изменение состояния
		break;
	case VAR_GET_ANALYSIS: // Обработка типа переменной
		switch(var_table[var_get_index].type) {
			case TYPE_UINT16_T: // Тип переменной: uint16_t
				// Преобразование значения переменной для вывода на экран
				Uint16ToStr(*((uint16_t*)var_table[var_get_index].ptr\
				), var_get_buff); 
				usart1_send_str(var_get_buff); // Вывод на экран
				break;
			case TYPE_FLOAT: // Тип переменной: float
				// Преобразование значения переменной для вывода на экран
				xftoa(var_get_buff, *((float*)var_table[\
				var_get_index].ptr), 3, 'E');
				usart1_send_str(var_get_buff); // Вывод на экран
				break;
			default: // Тип переменной: неизвестный
				usart1_send_str("UNKNOWN_TYPE"); // Вывод на экран
				break;
		}
		var_get_state = VAR_GET_CHECK; // Изменение состояния
		break;
	case VAR_GET_CHECK: // Проверка на завершение работы
		// Если всё передано, то автомат завершил свою работу
		if (usart1_send_ready()) var_get_state = VAR_GET_END;
		break;
}