#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include <display.h>
#include <button.h>
#include <tick.h>
#include <global_vars.h>   // objectConfigFile, docConfigFile, OLED_I2C_CLOCK (через pins.h)
#include <fileSystem.h>    // saveJsonToFile
#include "temp.h"

// =============================================================================
//  MLX90614 (GY-906) — ИК-термометр по I2C/SMBus, адрес 0x5A.
// =============================================================================
static const uint8_t MLX_ADDR  = 0x5A;
static const uint8_t MLX_TA     = 0x06;  // RAM: температура окружения (ambient)
static const uint8_t MLX_TOBJ1  = 0x07;  // RAM: температура объекта
static const uint32_t MLX_I2C_CLOCK = 100000;  // SMBus: не более 100 кГц

// Пресеты поверхностей (эмиссивность ε). Коррекция применяется программно,
// в предположении, что заводская ε датчика = 1.0 (по умолчанию у MLX90614).
struct Surface {
  const char* name;
  float emis;
};
static const Surface surfaces[] = {
  {"Default 1.00", 1.00f},
  {"Skin 0.98",    0.98f},
  {"Water 0.96",   0.96f},
  {"Plastic 0.95", 0.95f},
  {"Paper 0.93",   0.93f},
  {"Glass 0.92",   0.92f},
  {"Concrete .92", 0.92f},
  {"Wood 0.90",    0.90f},
  {"Metal ox .80", 0.80f},
  {"Metal shiny.2",0.20f},
};
static const int SURF_COUNT = sizeof(surfaces) / sizeof(surfaces[0]);

// Чтение 16-битного слова по SMBus: адрес+команда, повторный старт, 3 байта
// (LSB, MSB, PEC). PEC не проверяем, но байт вычитываем.
static bool mlxRead16(uint8_t reg, uint16_t& out) {
  Wire.beginTransmission(MLX_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;      // repeated start (без STOP)
  if (Wire.requestFrom((uint8_t)MLX_ADDR, (uint8_t)3) != 3) return false;
  uint8_t lo = Wire.read();
  uint8_t hi = Wire.read();
  (void)Wire.read();                                        // PEC
  out = (uint16_t)lo | ((uint16_t)hi << 8);
  return true;
}

static bool mlxPresent() {
  Wire.beginTransmission(MLX_ADDR);
  return Wire.endTransmission() == 0;
}

// Экран "датчик не найден" — ждём back.
static void tempNotFound() {
  bool upd = true;
  while (1) {
    tk();
    if (upd) {
      upd = false;
      oled.clear();
      oled.home();
      oled.setCursor(0, 0);
      oled.print("MLX90614 (GY-906)");
      oled.setCursor(0, 2);
      oled.print("Sensor not found");
      oled.setCursor(0, 3);
      oled.print("addr 0x5A, I2C");
      oled.setCursor(0, 5);
      oled.print("Check wiring / SDA-SCL");
      oled.setCursor(0, 7);
      oled.print("back - exit");
      oled.update();
    }
    if (back.click() || back.hold()) return;
  }
}

void tempLay() {
  // Загружаем сохранённую калибровку/поверхность из config.json.
  float offset = objectConfigFile["tempOffset"] | 0.0f;
  int surf = objectConfigFile["tempSurface"] | 0;
  surf = constrain(surf, 0, SURF_COUNT - 1);

  // Детекция датчика (на пониженной частоте шины, как требует SMBus).
  Wire.setClock(MLX_I2C_CLOCK);
  bool present = mlxPresent();
  Wire.setClock(OLED_I2C_CLOCK);

  if (!present) {
    tempNotFound();
    return;
  }

  bool upd = true;
  bool readOk = false;
  float tObjC = 0, tAmbC = 0;
  uint32_t last = millis() - 1000;  // первый замер сразу

  while (1) {
    tk();

    // Пресет поверхности (эмиссивность) — по кругу.
    if (left.click() || left.step())   { surf = (surf + SURF_COUNT - 1) % SURF_COUNT; upd = true; }
    if (right.click() || right.step())  { surf = (surf + 1) % SURF_COUNT; upd = true; }
    // Калибровочное смещение, шаг 0.1 °C.
    if (up.click() || up.step())        { offset = constrain(offset + 0.1f, -20.0f, 20.0f); upd = true; }
    if (down.click() || down.step())    { offset = constrain(offset - 0.1f, -20.0f, 20.0f); upd = true; }
    if (back.click() || back.hold()) break;

    // Периодический замер (SMBus на 100 кГц, затем обратно 800 кГц для OLED).
    if (millis() - last > 300) {
      last = millis();
      uint16_t rawObj = 0, rawAmb = 0;
      Wire.setClock(MLX_I2C_CLOCK);
      bool ok1 = mlxRead16(MLX_TOBJ1, rawObj);
      bool ok2 = mlxRead16(MLX_TA, rawAmb);
      Wire.setClock(OLED_I2C_CLOCK);

      readOk = ok1 && ok2 && !(rawObj & 0x8000) && !(rawAmb & 0x8000);
      if (readOk) {
        double tObjK = rawObj * 0.02;   // K = raw * 0.02 (датчик при ε=1.0)
        double tAmbK = rawAmb * 0.02;
        double e = surfaces[surf].emis;
        // Программная коррекция эмиссивности (радиометрическая, 4-я степень):
        //   Tc^4 = (Tobj^4 - (1-e)*Ta^4) / e
        double arg = (pow(tObjK, 4) - (1.0 - e) * pow(tAmbK, 4)) / e;
        if (arg < 0) arg = 0;
        double tcK = pow(arg, 0.25);
        tObjC = (float)(tcK - 273.15) + offset;
        tAmbC = (float)(tAmbK - 273.15);
      }
      upd = true;
    }

    if (upd) {
      upd = false;
      oled.clear();
      oled.home();

      oled.setCursor(0, 0);
      oled.print("Object temp:");

      oled.setScale(2);
      oled.setCursor(0, 2);
      if (readOk) {
        oled.print(tObjC, 1);
        oled.print(" C");
      } else {
        oled.print("-- err");
      }
      oled.setScale(1);

      oled.setCursor(0, 4);
      oled.print("Ambient: ");
      if (readOk) { oled.print(tAmbC, 1); oled.print(" C"); }

      oled.setCursor(0, 5);
      oled.print("Surf: ");
      oled.print(surfaces[surf].name);

      oled.setCursor(0, 6);
      oled.print("Cal: ");
      if (offset >= 0) oled.print("+");
      oled.print(offset, 1);
      oled.print(" C");

      oled.setCursor(0, 7);
      oled.print("<>surf ^v cal back");
      oled.update();
    }
  }

  // Сохраняем калибровку и выбранную поверхность.
  objectConfigFile["tempOffset"] = offset;
  objectConfigFile["tempSurface"] = surf;
  saveJsonToFile("/config.json", docConfigFile);
}
