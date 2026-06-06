#include "var_set.h"

// ============= Автомат изменения значений переменных по USART1 ===============
switch(var_set_state) {
	case VAR_SET_START: // Обработка типа нового значения изменяемой переменной
		var_set_buff_addr = var_set_string;
		var_set_state = VAR_SET_ERROR; // Изменение состояния
		switch(var_table[var_set_index].type) {
			case TYPE_UINT16_T: // Тип переменной: uint16_t
				// Если не удалось преобразовать число
				if (!xatoi(&var_set_buff_addr, &var_set_long)) break;
				if (var_set_long > 0xFFFF) break; // Если число больше uint16_t
				*((uint16_t*)var_table[var_set_index].ptr) = var_set_long;
				// Вывод нового значения переменной (автомат var_get)
				var_get_index = var_set_index;
				var_get_state = VAR_GET_START; // Запуск автомата var_get
				var_set_state = VAR_SET_CHECK_VAR_GET; // Смена состояния
				break;
			case TYPE_FLOAT: // Тип переменной: float
				if (!xatof(&var_set_buff_addr, (float*)\
				var_table[var_set_index].ptr)) break;
				// Вывод нового значения переменной (автомат var_get)
				var_get_index = var_set_index;
				var_get_state = VAR_GET_START; // Запуск автомата var_get
				var_set_state = VAR_SET_CHECK_VAR_GET; // Смена состояния
				break;
			default: // Тип переменной: неизвестный
				usart1_send_str("Unknown type of the variable.\n\r");
				var_set_state = VAR_SET_CHECK; // Изменение состояния
				break;
		}
		break;
	case VAR_SET_ERROR: // Вывод сообщения о некорректном значении переменной
		usart1_send_str("Bad value for the variable. \
Enter \"help\" for help.");
		var_set_state = VAR_SET_CHECK; // Изменение состояния
		break;
	case VAR_SET_CHECK: // Если сообщение отправлено, то автомат завершил работу
		if (usart1_send_ready()) var_set_state = VAR_SET_END;
		break;
	case VAR_SET_CHECK_VAR_GET: // Сверка состояний var_get и var_set
		if (var_get_state != VAR_GET_END) break; 
		// Если var_get завершил работу
		var_get_state = VAR_GET_NOT_ACTIVATED;
		var_set_state = VAR_SET_END; // То var_set тоже завершил работу
		break;
}