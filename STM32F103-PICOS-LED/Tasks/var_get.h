#ifndef VAR_GET
#define VAR_GET
//Дата последней редакции: 9 октября 2025 года

// Библиотека с реализацией функции atof() и ftoa()
#include "../Lib/xprintf_float.h"

/* ======================== Краткое описание автомата =========================
	Стандартный вывод на экран значения переменной из таблицы var_table[] */
	
// ======================= Описание переменных автомата =======================
enum var_get_state { // Перечисление состояний автомата
	VAR_GET_NOT_ACTIVATED,
	VAR_GET_START,
	VAR_GET_ANALYSIS,
	VAR_GET_CHECK,
	VAR_GET_END
};

uint8_t var_get_state = VAR_GET_NOT_ACTIVATED; // Начальное состояние 
uint8_t var_get_index = 0; // Номер переменной в таблице os_var_table[]
char var_get_buff[16]; // Буфер для хранения новых значений переменных

#endif