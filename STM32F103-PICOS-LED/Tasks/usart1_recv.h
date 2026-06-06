#ifndef USART1_RECV
#define USART1_RECV
//Дата последней редакции: 20 апреля 2025 года

/* ======================== Краткое описание автомата =========================
	Побайтово получает строку по интерфейсу USART и помещает её в буфер 
usart1_recv_buff. */
	
// ======================== Описание констант автомата ========================
#define USART1_RECV_BUFF_SIZE 32 // Размер буфера для сохранения строки

// ======================= Описание переменных автомата =======================
enum usart1_recv_state { // Перечисление состояний автомата
	USART1_RECV_NOT_ACTIVATED,
	USART1_RECV_START,
	USART1_RECV_OUTPUT,
	USART1_RECV_ANALYSIS,
	USART1_RECV_END
};

uint8_t usart1_recv_state = USART1_RECV_NOT_ACTIVATED; // Начальное состояние
char usart1_recv_buff[USART1_RECV_BUFF_SIZE]; // Буфер для записи новой строки
uint8_t usart1_recv_index = 0; // Индекс буфера для записи новой строки

#endif