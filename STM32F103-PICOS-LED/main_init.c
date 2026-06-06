// Дата последней редакции: 23 апреля 2025 года

// ====================== Инициализация микроконтроллера ======================
static __INLINE void main_init() {
// Буфер предвыборки разрешён, 2 цикла задержки для FLASH, см. PM0075
FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2;
RCC->CFGR |= RCC_CFGR_HPRE_DIV1; // HCLK = SYSCLK
RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; // PCLK1 = SYSCLK / 2 (APB1)
RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; // PCLK2 = SYSCLK (APB2)
RCC->CFGR |= RCC_CFGR_SW_PLL; // Выбираем работу от PLL
RCC->CFGR |= RCC_CFGR_PLLMULL9; // SYSCLK = 72 MHz
RCC->CFGR |= RCC_CFGR_PLLSRC_HSE; // Тактирования PLL от кварца
RCC->CR |= RCC_CR_HSEON; //Запускаем генератор HSE
while (!(RCC->CR & RCC_CR_HSERDY)) {};
RCC->CR |= RCC_CR_PLLON; // Запускаем PLL
while ((RCC->CR & RCC_CR_PLLRDY) == 0) {};
// Ожидаем когда PLL станет источником тактирования
while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_1) {};
//RCC->CR &= ~RCC_CR_HSION; // Отключаем внутренний RC-генератор
// JTAG-DP запрещён and SW-DP разрешён
AFIO->MAPR = (AFIO_MAPR_SWJ_CFG_JTAGDISABLE);

// =================================== DWT ====================================
SCB_DEMCR |= 0x01000000; // Разрешаем использовать счётчик
DWT_CYCCNT = 0; // Обнуляем значение счётного регистра
DWT_CONTROL |= 1; // Запускаем счётчик  

// ================================== ПОРТЫ ===================================
// Включаем тактирование подсистем.
// Настройка подсистем только после включения тактирования!
RCC->APB2ENR = (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN \
| RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN);

// Настраиваем PC13 на выход, 10 МГц
GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
GPIOC->CRH |= GPIO_CRH_MODE13_0;

// ================================== USART1 ==================================
// Тактирование порта GPIOA и альтернативных функций должно быть включено
RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Включаем тактирование USART1
// PA9 (TX1) AFIO Push-Pull, 10MHz. PA10 (RX1) HiZ, 10MHz
// Вначале устанавливаем пары бит в "00", это делать обязательно!
GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
// Потом нужные биты устанавливаем в '1'
GPIOA->CRH |= (GPIO_CRH_MODE9_0 | GPIO_CRH_CNF9_1 | GPIO_CRH_CNF10_0); 
USART1->BRR = 7500; // PCLK2 / Baud = 72000000 / 9600 бод
// Включаем USART, передатчик и приемник
USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
// Настройки по умолчанию: 8 инф. бит, 1 стоповый бит, контроля чётности нет
} 
// КОНЕЦ
