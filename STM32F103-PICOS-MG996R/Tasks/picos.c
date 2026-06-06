#include "picos.h"

// ========================== Автомат системы PicOS ===========================
switch(picos_state) {
	case PICOS_START: // Включение подсистемы вывода
		usart1_send_state = USART1_SEND_START; // Включение usart1_send
		// Отправка приветственного сообщения
		usart1_send_str("Welcome to PicOS v");
		usart1_send_str(version);
		picos_state = PICOS_WAIT_MESSADGE; // Изменение состояния
		break;
	case PICOS_WAIT_MESSADGE: // Ожидание отправки всех сообщений
		if (usart1_send_ready()) { // Если всё передано
			usart1_recv_state = USART1_RECV_START; // Включение usart1_recv
			picos_state = PICOS_WAIT_INPUT; // Изменение состояния
			// Вывод символа ожидания ввода строки ">"
			usart1_send_str("\n\r> "); 
		} 
		break;
	case PICOS_WAIT_INPUT: // Ожидание ввода строки
		// Если автомат приёма строки завершил работу
		if (usart1_recv_state == USART1_RECV_END) 
			picos_state = PICOS_STRING_ANALYSIS; // Изменение состояния 
		break;
	case PICOS_STRING_ANALYSIS: // Обработка строки
		picos_var_index = 0xAA; // Очистка индекса переменной
		// Поиск названия переменной
		picos_search_state = search(usart1_recv_buff, picos_subbuff, 0);
		// Если команды не было введено, значит ничего не введено
		if (!picos_search_state) {
			usart1_send_str("Nothing entered. Enter \"help\" for help.");
			picos_state = PICOS_WAIT_MESSADGE; // Изменение состояния
			break;
		}
		// Команда отображения справочной информации
		else if (my_strcmp("help", picos_subbuff)) {
			picos_state = PICOS_HELP_START; // Изменение состояния
			break;
		}
		// Проверка существования имени переменной
		for (uint8_t j = 0; j < COUNT_OF(var_table); j++) {
			if (my_strcmp(var_table[j].name, picos_subbuff)) {
				picos_var_index = j;
				break;
			}
		}
		// Переменная с таким именем существует?
		if (picos_var_index == 0xAA) { // Нет: переменная не найдена в таблице
			usart1_send_str("var \"");
			usart1_send_str(picos_subbuff);
			usart1_send_str("\" not found. Enter \"help\" for help.");
			picos_state = PICOS_WAIT_MESSADGE; // Изменение состояния
			break;
		} else { // Да: поиск нового значения переменной
			picos_search_state = search(usart1_recv_buff, picos_subbuff, 1);
			// Новое значение найдено? Да: изменение состояния
			if (picos_search_state) picos_state = PICOS_VAR_SET_START;
			else { // Нет: значит строка закончилась
				// Запрос автомату var_get вывести содержимое переменной
				var_get_index = picos_var_index;
				var_get_state = VAR_GET_START; 
				// Ожидание вывода результата на экран
				picos_state = PICOS_VAR_GET; // Изменение состояния
			}	
		}
		break;
	case PICOS_VAR_SET_START: // Изменение значения переменной
		// Запрос автомату var_set на изменение содержимого переменной
		var_set_index = picos_var_index;
		var_set_string = picos_subbuff;
		var_set_state = VAR_SET_START; 
		picos_state = PICOS_VAR_SET_END; // Ожидать завершения работы var_set
		break;
	case PICOS_VAR_GET: // Вывод значения переменной
		// Если автомат var_get завершил свою работу, то изменить состояние
		if (var_get_state == VAR_GET_END) picos_state = PICOS_WAIT_MESSADGE;
		break;
	case PICOS_VAR_SET_END: // Изменения значения переменной
		// Автомат var_set завершил свою работу?
		if (var_set_state == VAR_SET_END) { 
			// Да: запрос автомату var_get вывести содержимое переменной
			var_get_index = picos_var_index;
			var_get_state = VAR_GET_START; 
		} else picos_state = PICOS_WAIT_MESSADGE; // Изменение состояния
		break;
	case PICOS_HELP_START: //Вывод инфомационной справки
		usart1_send_str("Available variables:");
		picos_var_index = 0; // Сброс индекса выбранной переменной
		picos_state = PICOS_HELP_WAIT; // Изменение состояния
		break;
	case PICOS_HELP_WAIT: // Ожидание отправки всех сообщений
		if (usart1_send_ready()) picos_state = PICOS_HELP_TABLE;
		break;
	case PICOS_HELP_TABLE: // Вывод таблицы переменных
		// Если есть, какую переменную показывать
		if (picos_var_index < COUNT_OF(var_table)) {
			usart1_send_str("\n\r\t");
			usart1_send_str(var_table[picos_var_index].name);
			usart1_send_str(": ");
			usart1_send_str(var_table[picos_var_index].description);
			usart1_send_str("; type: ");
			if (var_table[picos_var_index].type == 0) 
				usart1_send_str("uint16_t");
			else usart1_send_str("float");
			picos_var_index++; // Выбор следующей переменной
			picos_state = PICOS_HELP_WAIT; // Изменение состояния
			break;
		} 
		else picos_state = PICOS_WAIT_MESSADGE; // Изменение состояния
		break;
}