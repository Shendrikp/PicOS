/* =============================================================================
Дата последней редакции: 8 октября 2025 года
Заказчик: НГТУ
Плата: Blue Pill
Микроконтроллер: STM32F103C6T6
Язык программирования: СИ + РЕГИСТРЫ + CMSIS
Авторы: Трубин В.Г., Вдовин В.А., Шендрик П.Д.

В проекте, использованы наработки из библиотеки Xprintf 
(http://elm-chan.org/fsw/strf/xprintf.html). 
Реализация программы, управляемой данными. Принимает по USART1 команды 
по типу "var = 0.5" или "var". Команда "var" позволяет считать значение
переменной, а команда "var ..." или "var = ..." задает ей значение. Переменные 
описываются в виде таблицы var_table[]. 
Для изменения коэффицентов регулирования ПИД-регулятора можно написать 
"pid_p ...", "pid_i ..." или "pid_d ...".

============================== Выходы дискретные ===============================
PA4 - Внешний светодиод 1, push-pull: 1 - горит, 0 - не горит
PA5 - Внешний светодиод 2, push-pull: 1 - горит, 0 - не горит
PA6 - Внешний светодиод 3, push-pull: 1 - горит, 0 - не горит
PA7 - Внешний светодиод 4, push-pull: 1 - горит, 0 - не горит
PB10 - HC-SR04: Контакт Trig, open-drain
PB11 - MG996R: Контакт Signal, open-drain 
PC13 - Встроенный светодиод, push-pull: 0 - горит, 1 - не горит

============================== Входы дискретные ================================
PA8 - HC-SR04: Контакт Echo

================================== Служебные ===================================
USART1: PA9 = TX1, PA10 = RX1 */

// =========================== Определение констант ============================
#define VERSION 34 // Версия данной прошивки - 3.4
#define SystemCoreClock 72000000 // Частота работы ядра, Гц

// ========================== Подключение библиотек ============================
#include "System/stm32f10x.h" // Библиотека с объявлением всех регистров МК
#include "main_functions.c" // Библиотека с объявлением базовых функций
#include "main_init.c" // Библиотека с инициализацией систем МК
#include "Lib/xprintf_float.c" // Библиотека для работы с типом float

// ===================== Объявление глобальных переменных ======================
uint16_t fm_version = VERSION; // Версия данной прошивки  

// ========================== Подключение автоматов ============================
#include "Tasks/usart1_send.h" // Передача данных по USART
#include "Tasks/usart1_recv.h" // Получение строки по USART1
#include "Tasks/picos.h" // PicOS
#include "Tasks/var_get.h" // Вывод значений переменных из таблицы по USART1
#include "Tasks/var_set.h" // Установка значений переменных из таблицы
#include "Tasks/distance_sensor.h" // Измерение расстояния до объекта
#include "Tasks/pid.h" // Работа ПИД-регулятора
#include "Tasks/button.h" // Обрабготка кнопки

// =================== Описание таблицы переменных автомата ====================
typedef struct { // Структура для создания таблиц переменных
  char name[12]; // Отображаемое имя переменной
  char description[32]; // Краткое описание переменной
  uint8_t type; // Тип переменной: TYPE_UINT16_T, TYPE_FLOAT
  void *ptr; // Указатель на саму переменную
} var_table_t;

// ===================== Инициализация таблицы переменных ======================
var_table_t var_table[] = {
  {"version", "Version of the firmware", TYPE_UINT16_T, &fm_version},
  {"sensor", "HC-SR04 reading, mm", TYPE_UINT16_T, &distance_sensor_value_mm},
  {"error", "regulatory error", TYPE_FLOAT, &pid_error},
  {"k_p", "Value of the P coeff", TYPE_FLOAT, &k_p},
  {"k_i", "Value of the I coeff", TYPE_FLOAT, &k_i},
  {"k_d", "Value of the D coeff", TYPE_FLOAT, &k_d},
  {"f_d", "Value of the d filter coeff", TYPE_FLOAT, &filter_d},
  {"f_t", "Value of the T filter coeff", TYPE_FLOAT, &filter_T},
  {"dt", "Sampling interval, >= 10 ms", TYPE_UINT16_T, &pid_period},
  {"target", "Target cart distance, mm", TYPE_UINT16_T, &target_distance},
  {"servo", "Control signal, 1500..4000", TYPE_UINT16_T, &servo_drive_switch}
};

// ================================ TIM3 1 мс ==================================
void TIM3_IRQHandler() {
  if ((TIM3->SR & TIM_SR_UIF) != 0) { // Прерывание по переполнению
    TIM3->SR &= ~ TIM_SR_UIF; // Сбрасываем флаг по переполнению
	if (pid_timer > 0) pid_timer--; // Отсчет времени для автомата pid
	if (button_timer > 0) button_timer--; // Отсчет времени для автомата button
  }
} 

// =================================== MAIN ====================================
int main(void) {
  main_init(); // Инициализация МК
  delay_ms(100); // Задержка для нормализации питания
  // Преобразование целочисленного 00034 в строку "3.4"
  Uint16ToStr(fm_version * 100, version); // Перевод uint16_t в str
  version[3] = '\0'; // "03400" -> "034"
  version[0] = version[1]; // "034" -> "334"
  version[1] = '.'; // "334" -> "3.4"

// =============================== ГЛАВНЫЙ ЦИКЛ ================================
  while (1) {
    #include "Tasks/usart1_send.c" // Отправка данных по USART1
    #include "Tasks/usart1_recv.c" // Принятие данных по USART1
    #include "Tasks/picos.c" // Операционная система PicOS
    #include "Tasks/var_get.c" // Вывод значений переменных по USART1
    #include "Tasks/var_set.c" // Изменение значений переменных
    #include "Tasks/distance_sensor.c" // Измерение расстояния до вагонетки
    #include "Tasks/pid.c" // ПИД-регулятор
    #include "Tasks/button.c" // Обработка нажатия кнопки
  }
} // КОНЕЦ