#ifndef USART1_SEND
#define USART1_SEND
//Дата последней редакции: 20 апреля 2025 года

/* ======================== Краткое описание автомата =========================
   Отправляет строку по интерфейсу USART. Для передачи строк используется 
функция usart1_send_str(). */
	
// ======================== Описание констант автомата ========================
#define USART1_SEND_RBUFF_SIZE 10 // Размер буфера для указателей на строки

// ======================= Описание переменных автомата =======================
enum usart1_send_state { // Перечисление состояний автомата
	USART1_SEND_NOT_ACTIVATED,
	USART1_SEND_START,
	USART1_SEND_ANALYSIS,
	USART1_SEND_TRANSMISSION
};

uint8_t usart1_send_state = USART1_SEND_NOT_ACTIVATED; // Начальное состояние
// Буфер указателей на строки, которые нужно отправить
volatile void* usart1_send_rbuff[USART1_SEND_RBUFF_SIZE];
uint8_t usart1_send_read_index = 0; // Индекс элемента буфера для чтения
uint8_t usart1_send_write_index = 0; // Индекс элемента буфера для записи

// ======================== Описание функций автомата =========================
// Получить индекс следующего элемента кольцевого буфера
#define RBUFF_NEXT(size,curr_index) ((curr_index) < (size - 1) ? \
(curr_index + 1) : 0)
// Переместить индекс элемента кольцевого буфера вперед
#define RBUFF_SETNEXT(size,curr_index) ((curr_index) = \
							(RBUFF_NEXT(size, curr_index)))	
// Переданы ли все массивы (или строки)
#define usart1_send_ready() ((usart1_send_read_index) == \
							(usart1_send_write_index))
/*
Описание: 
	Передаёт строку по USART1.
Входные параметры: 
	char *str	- указатель на строку для передачи;  
*/
void usart1_send_str(char *str) {
	// Записываем адрес строки, которую нужно передать
	usart1_send_rbuff[usart1_send_write_index] = str;
	// Сдвиг индекса на следующий элемент
	RBUFF_SETNEXT(USART1_SEND_RBUFF_SIZE, usart1_send_write_index);
}
#endif