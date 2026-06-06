// Дата последней редакции: 3 мая 2025 года

#define    DWT_CYCCNT    *(volatile uint32_t *)0xE0001004
#define    DWT_CONTROL   *(volatile uint32_t *)0xE0001000
#define    SCB_DEMCR     *(volatile uint32_t *)0xE000EDFC

// Функции для генерирования программной задержки с использованием DWT
// Не забывайте, что разные прерывания могу мешать точному отсчёту времени

/* Описание:
	 Функция задержки в мкс (можно от 1 мкс). Получите: задержка + 1 мкс.
   Входные параметры:
	 uint32_t us - время задержки в мкс. */
static __INLINE  void delay_us(uint32_t us) {
static volatile uint32_t n;
n =  us * 72; // SystemCoreClock/1000000 = 72
DWT_CYCCNT = 0; while (DWT_CYCCNT < n) ;
}

/* Описание:
	 Функция задержки в мс от 1 мс до 59 сек. (дальше переполнение)
   Входные параметры:
	 uint32_t ms - время задержки в мс. */
static __INLINE void delay_ms(uint32_t ms) {
static volatile uint32_t n;
n =  ms * 72000; DWT_CYCCNT = 0; while (DWT_CYCCNT < n) ;
}

/* Описание: 
	 Функция преобразования беззнакового 8 битного числа в строку.
	Входные параметры:
	 uint8_t Number - беззнаковое 8 битное число;
	 char *pStr - указатель на результирующую строку. */
void Uint8ToStr(uint8_t Number, char *pStr) {
static int8_t i;
for(i = 2; i != -1; i--) { pStr[i] = (Number % 10) + 0x30; Number /= 10; }
pStr[3] = 0; // конец строки
}

/* Описание: 
	 Функция преобразования беззнакового 16 битного числа в строку.
   Входные параметры:
	 uint16_t Number - беззнаковое 16 битное число;
	 char *pStr - указатель на результирующую строку. */
void Uint16ToStr(uint16_t Number, char *pStr) {
static int8_t i;
for(i = 4; i != -1; i--) { pStr[i] = (Number % 10) + 0x30; Number /= 10; }
pStr[5] = 0; // конец строки
}

/* Описание: 
	 Функция преобразования знакового 16 битного числа в строку.
   Входные параметры:
	 int16_t Number - знаковое 16 битное число;
	 uint8_t *pStr - указатель на результирующую строку. */
void Int16ToStr(int16_t Number, uint8_t *pStr) {
static int8_t i;
if (Number >=0) pStr[0] = '+'; 
  else { pStr[0] = '-'; Number = -Number; }
for(i = 5; i != 0; i--) { pStr[i] = (Number % 10) + 0x30; Number /= 10; }
pStr[6] = 0; // конец строки
}

/* Описание: 
	 Функция преобразования беззнакового 32 битного числа в строку.
   Входные параметры:
	 uint32_t Number - беззнаковое 32 битное число 0 .. 4 294 967 295;
	 uint8_t *pStr - указатель на результирующую строку. */
void Uint32ToStr(uint32_t Number, uint8_t *pStr) {
static int8_t i;
for(i = 9; i != -1; i--) { pStr[i] = (Number % 10) + 0x30; Number /= 10; }
pStr[10] = 0;
}

/* Описание: 
	 Функция “зависает” до готовности передачи и отправляет символ по USART1.
   Входные параметры:
	 uint8_t Number - беззнаковое 16 битное число. */
void Tx1(uint8_t Symbol) {
while ((USART1->SR & USART_SR_TXE) == 0) {};
USART1->DR = Symbol;
}

/* Описание: 
	 Функция “зависает” до прихода символа по USART1, после получения символа
возвращает его.
   Выходные параметры:
	 Возвращает байт формата uint8_t. */
uint8_t Rx1() {
while ((USART1->SR & USART_SR_RXNE) == 0) {};
return USART1->DR;
}


/* Описание: 
	 Функция передачи массива символов (строки) по USART1, пока не 
встретится "0" байт.
   Входные параметры:
	 uint8_t *pStr - указатель на строку. */
void TxStr1(uint8_t *pStr) {
static uint8_t i;
i = 0; do Tx1( pStr[i++] ); while (pStr[i] != 0);
}
// КОНЕЦ
