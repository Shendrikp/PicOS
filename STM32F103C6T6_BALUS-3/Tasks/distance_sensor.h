#ifndef DISTANCE_SENSOR
#define DISTANCE_SENSOR
// Дата последней редакции: 20 апреля 2025 года

/* ======================= Описание конечного автомата  =======================
	Производит замер растояния до объекта и помещает результат 
в distance_sensor_value_mm. */
	
// ===================== Описание переменных автомата  ========================
enum distance_sensor_state { // Перечисление состояний автомата
	DISTANCE_SENSOR_NOT_ACTIVATED,
	DISTANCE_SENSOR_START,
	DISTANCE_SENSOR_WORK,
	DISTANCE_SENSOR_END
};

uint8_t distance_sensor_state = DISTANCE_SENSOR_NOT_ACTIVATED; // Не активен 
uint16_t distance_sensor_value_mm = 0; // Расстояние до объекта в мм
uint16_t distance_sensor_value_mcs = 0; // Длительность звукового сигнала в мкс

// ======================== Описание функций автомата =========================
// Расстояние до объекта в миллиметрах
#define distance_sensor_result_mm() (distance_sensor_value_mm)

#endif