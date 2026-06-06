#ifndef VAR_SET
#define VAR_SET
//Дата последней редакции: 9 октября 2025 года

// Библиотека с реализацией функции atof() и ftoa()
#include "../Lib/xprintf_float.h"

/* ======================== Краткое описание автомата =========================
   Изменяет значение переменной и выводит его вместе с переменной, с помощью 
автомата var_get. */
   
// ======================= Описание переменных автомата =======================
enum var_set_state { // Перечисление состояний автомата
	VAR_SET_NOT_ACTIVATED,
	VAR_SET_START,
	VAR_SET_ERROR,
	VAR_SET_CHECK,
	VAR_SET_CHECK_VAR_GET,
	VAR_SET_END
};

enum var_type { // Перечисление типов переменных
	TYPE_UINT16_T,
	TYPE_FLOAT
};

uint8_t var_set_state = VAR_SET_NOT_ACTIVATED; // Начальное состояние 
uint8_t var_set_index = 0; // Номер изменяемой переменной в таблице var_table[]
char var_set_buff[16]; // Буфер для сохранения подстрок  
char* var_set_buff_addr; // Указатель (курсор) буфера
float var_set_float; // Число с плавающей точкой, введенное пользователем 
long int var_set_long; // Переменная, для работы с функциями xatof, xatoi
char* var_set_string = 0; // Указатель на строку, с новым значением переменной

#endif