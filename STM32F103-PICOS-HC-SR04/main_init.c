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

GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13); // Сброс настроек пина PC13
GPIOC->CRH |= GPIO_CRH_MODE13_0; // Настройка PC13 на выход, 10 МГц
GPIOC->BSRR = GPIO_BSRR_BS13; // Выключение светодиода

GPIOB->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10); // Сброс настроек пина PB10
GPIOB->CRH |= GPIO_CRH_MODE10_0; // Настройка PB10 на выход, 10 МГц
GPIOB->CRH |= GPIO_CRH_CNF10_0; // Настрока PB10 в режиме open-drain
GPIOB->BSRR = GPIO_BSRR_BR10; // Запись логического нуля

GPIOA->CRH &= ~(GPIO_CRH_MODE8 | GPIO_CRH_CNF8); // Сброс настроек пина PA8
GPIOA->CRH |= GPIO_CRH_CNF8_0; // Настрока PA8 как floating-input

// ================================== USART1 ==================================
// Тактирование порта GPIOA и альтернативных функций должно быть включено
RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Включаем тактирование USART1
// PA9 (TX1) AFIO Push-Pull, 10MHz. PA10 (RX1) HiZ, 10MHz
// Вначале устанавливаем пары бит в "00", это делать обязательно !
GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
// Потом нужные биты устанавливаем в '1'
GPIOA->CRH |= (GPIO_CRH_MODE9_0 | GPIO_CRH_CNF9_1 | GPIO_CRH_CNF10_0); 
USART1->BRR = 7500; // PCLK2 / Baud = 72000000 / 9600 бод
// Включаем USART, передатчик и приемник
USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
// Настройки по умолчанию: 8 инф. бит, 1 стоповый бит, контроля чётности нет

// ================================== FLASH ===================================
FLASH->ACR &= ~FLASH_ACR_HLFCYA; // Выключаем режим половинного цикла доступа

// =================================== TIM3 ===================================
// Разрешаем тактирование, PCLK1 = 36 МГц, TIM3 получается x 2 = 72 МГц
RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; 
TIM3->SMCR &= ~TIM_SMCR_SMS; // Выбираем внутреннее тактирование
TIM3->PSC = 72 - 1; // Шаг (квант) счёта 1 мкс
TIM3->ARR = 1000 - 1; // Переполнение каждую 1 мс
TIM3->CR1 = TIM_CR1_CEN; // Разрешаем счёт
TIM3->DIER |= TIM_DIER_UIE; // Разрешаем прерывание по переполнению таймера
NVIC_EnableIRQ(TIM3_IRQn); // Разрешаем прерывание в контроллере прерываний

// =================================== TIM1 ===================================
RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; // Разрешаем тактирование, PCLK1 = 72 МГц
TIM1->SMCR &= ~TIM_SMCR_SMS; // Выбираем внутреннее тактирование
TIM1->PSC = 36 - 1; // Шаг (квант) счёта 0.5 мкс 
TIM1->CCMR1 |= TIM_CCMR1_CC1S_0; // Выбор входа TI1 для TIM1_CH1
TIM1->CCMR1 |= TIM_CCMR1_CC2S_1; // Выбор входа TI1 для TIM1_CH2
// Не фильтровать и делитель не использовать
TIM1->CCMR1 &= ~(TIM_CCMR1_IC1F | TIM_CCMR1_IC1PSC);
// Не фильтровать и делитель не использовать
TIM1->CCMR1 &= ~(TIM_CCMR1_IC2F | TIM_CCMR1_IC2PSC);
TIM1->CCER &= ~TIM_CCER_CC1P; // Захват по переднему фронту
TIM1->CCER |= TIM_CCER_CC2P; // Захват по заднему фронту	
TIM1->CCER |= TIM_CCER_CC1E; // Включение режима захвата
TIM1->CCER |= TIM_CCER_CC2E; // Включение режима захвата
TIM1->SR &= ~(TIM_SR_CC1IF); // Сброс флага захвата сигнала
TIM1->SR &= ~(TIM_SR_CC2IF); // Сброс флага захвата сигнала
}
// КОНЕЦ
