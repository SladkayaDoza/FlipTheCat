#include "gpioPins.h"

// Единственное определение объектов пинов (extern-объявления — в gpioPins.h).
// Аргументы: (GPIO, частота ШИМ, LEDC-канал). Каналы 2/3/4 — по одному на пин.
OUTPIN pin1(GPIO_OUT_PIN_1, 80000, 2);
OUTPIN pin2(GPIO_OUT_PIN_2, 80000, 3);
OUTPIN pin3(GPIO_OUT_PIN_3, 80000, 4);
