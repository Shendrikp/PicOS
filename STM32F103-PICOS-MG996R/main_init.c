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

// =================================== DWT ====================================
SCB_DEMCR |= 0x01000000; // разрешаем использовать счётчик
DWT_CYCCNT = 0; // обнуляем значение счётного регистра
DWT_CONTROL |= 1; // запускаем счётчик  

// ================================== ПОРТЫ ===================================
// Включаем тактирование подсистем.
// Настройка подсистем только после включения тактирования!
RCC->APB2ENR = (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN \
| RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN);

// JTAG-DP запрещён and SW-DP разрешён
AFIO->MAPR = (AFIO_MAPR_SWJ_CFG_JTAGDISABLE);

// Переназначение выводов канала 2 таймера 2
AFIO->MAPR |= AFIO_MAPR_TIM2_REMAP_FULLREMAP;

GPIOB->CRH &= ~(GPIO_CRH_MODE11 | GPIO_CRH_CNF11); // Сброс настроек пина PB11
GPIOB->CRH |= GPIO_CRH_MODE11_0; // Настройка PB11 на выход, 10 МГц
GPIOB->CRH |= GPIO_CRH_CNF11; // Настрока PB11 как альтернативный open-drain
GPIOB->BSRR = GPIO_BSRR_BR11; // Запись логического нуля

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

// =================================== TIM2 ===================================
RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Разрешаем тактирование, PCLK1 = 72 МГц
TIM2->SMCR &= ~TIM_SMCR_SMS; // Выбираем внутреннее тактирование 
TIM2->PSC = 72 - 1; // Шаг (квант) счёта 1 мкс 
TIM2->ARR = 40000; // Переполнение каждые 20 мс (50 Гц)
TIM2->CCMR2 &= ~TIM_CCMR2_CC4S; // Выбор выхода TIM2_CH4
TIM2->CCMR2 |= (TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1); // Режим PWM
TIM2->CCER |= TIM_CCER_CC4E; // Включение выхода канала TIM2_CH4 на порт
TIM2->CCR4 = 3000; // По умолчанию импульс 1 мс 
TIM2->CR1 |= TIM_CR1_CEN; // Включение таймера-счетчика
} 
// КОНЕЦ
