#include "NimBLEDevice.h"
#include <BleKeyboard.h>

#include <Arduino.h>

// #include <BleMouse.h>
// #define USE_NIMBLE

#include "SPIFFS.h"
#include "FS.h"
#include <ArduinoJson.h>

#include <GyverOLED.h>
GyverOLED<SSH1106_128x64> oled;

#include <Wire.h>
#include <EncButton.h>

VirtButton up;
VirtButton down;
VirtButton left;
VirtButton right;
VirtButton ok;
VirtButton back;
VirtButton top_left;
VirtButton top_right;

#include <global_vars.h>

#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();



#include <logo.h>
#include <fileSystem.h>
#include <tick.h>
#include <buzzer.h>
#include <pointer.h>
#include <adc.h>
#include <GPIO.h>
#include <settings.h>
#include <tools.h>
#include <view.h>
#include <nothing.h>
#include <games.h>
#include <radio.h>
#include <bluetooth.h>





// typedef struct
// {
//   uint32_t frequency_coarse;
//   int rssi_coarse;
//   uint32_t frequency_fine;
//   int rssi_fine;
// } FrequencyRSSI;
// const int rssi_threshold = -70;



//  RAW DATA
JsonDocument docConfigFile;
JsonDocument doc;
JsonDocument docRC;
JsonObject objectConfigFile = docConfigFile["config"].add<JsonObject>();
JsonObject objectDoc = doc["sensorData"].add<JsonObject>();
JsonObject objectRC = docRC["data"].add<JsonObject>();



void mainn();
void rcLay();
void rawLay();
void detectSignal();
void rc12bitBruteForce();
void bluetoothLay();
void rawMenu(uint8_t pointer1);
void func_trans(uint8_t codek);
void menuRc(uint8_t pointer1);
void func_reciv(uint8_t cutr);
void reNameRC(int pointer);
void changeRC(int pointer);
void deleteRC(int pointer1);
void select_deleteRc(uint8_t pointer);


const char* main_lay[8] = {
  "RC Custom",
  "RawData Read",
  "Frequency Analyzer",
  "RC 12bit bruteForce",
  "BLUETOOTH",
  "GPIO",
  "Settings",
  "Games",
};

const char* blue_lay[8] = {
  "Keyboard",
  "Mouse",
  "",
  "",
  "",
  "",
  "",
  "",
};


void setupRx() {
  ELECHOUSE_cc1101.Init();
  // ELECHOUSE_cc1101.setRxBW(812.50);
  // ELECHOUSE_cc1101.setRxBW(256);
  ELECHOUSE_cc1101.setMHZ(signalDetectionFrequencies[FrequencyPointer]);
  // if (!RxTxMode) {
  pinMode(RCPin, INPUT);
  mySwitch.disableTransmit();
  ELECHOUSE_cc1101.SetRx();
  mySwitch.enableReceive(RCPin);
  RxTxMode = !RxTxMode;
  // }
}
void setupTx() {
  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.setMHZ(signalDetectionFrequencies[FrequencyPointer]);
  // if (RxTxMode) {
  pinMode(RCPin, OUTPUT);
  // mySwitch.disableReceive();  // trable
  ELECHOUSE_cc1101.SetTx();
  mySwitch.enableTransmit(RCPin);
  RxTxMode = !RxTxMode; 
  // }
}

#pragma region SETUP
void setup() {

  adcAttachPin(34);
  // analogSetCycles(16);
  // if (SPIFFS.exists("/raw.json")) {
  //   SPIFFS.remove("/raw.json");
  //   Serial.println("Файл удален");
  // } else {
  //   Serial.println("Файл не существует");
  // }
  Wire.setClock(800000L);

  oled.init();
  oled.clear();
  oled.setContrast(255);
  drawLogo();
  oled.update();

  for (int i = 0; i < pinsCount - 1; i++) {  // file GPIO.ino
    OutPinModeG[i] = 0;
  }


  pinMode(top_left_pin, INPUT_PULLUP);
  pinMode(top_right_pin, INPUT_PULLUP);
  pinMode(led_pin, OUTPUT);

  digitalWrite(led_pin, LOW);

  Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
    Serial.println("Ошибка инициализации SPIFFS");
  }

  readJsonFromFile("/raw.json", doc);
  readJsonFromFile("/rc.json", docRC);
  readJsonFromFile("/config.json", docConfigFile);
  Serial.println("ready");

  // first start - ".as" to ".to"
  objectDoc = doc["sensorData"].as<JsonObject>();  // frequency, name, RawData[]
  objectRC = docRC["data"].as<JsonObject>();       // frequency, name, data, bytes
  objectConfigFile = docConfigFile["config"].as<JsonObject>();       // frequency, name, data, bytes

  // objectDoc = doc["sensorData"].to<JsonObject>();  // frequency, name, RawData[]
  // objectRC = docRC["data"].to<JsonObject>();       // frequency, name, data, bytes
  // objectConfigFile = docConfigFile["config"].to<JsonObject>();       // frequency, name, data, bytes

  // Buzzer
  pik = objectConfigFile["pik"].as<bool>();
  pikHz = objectConfigFile["pikHz"].as<int>();

  if (pik) setupBuzzer(pikHz);
  
  ELECHOUSE_cc1101.setSpiPin(18, 19, 23, (objectConfigFile["externalССModule"].as<bool>() ? 13 : 5));
  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.setMHZ(signalDetectionFrequencies[FrequencyPointer]);
  // ELECHOUSE_cc1101.setRxBW(58);
  mySwitch.enableReceive(RCPin);
  ELECHOUSE_cc1101.setGDO0(RCPin);
  // mySwitch.setPulseLength();
  ELECHOUSE_cc1101.SetRx();
  ELECHOUSE_cc1101.goSleep();
}

#pragma region LOOP
void loop() {
  mainn();
}

void mainn() {
  bool displayUpdate = 1;
  tk();
  while (1) {
    static uint8_t pointer = 0;
    tk();
    if (displayUpdate) {
      displayUpdate = 0;
      oled.clear();
      oled.home();
      for (uint8_t i = 0; i < 8; i++) {
        oled.setCursor(14, i);
        oled.print(main_lay[i]);
      }
      oled.setCursor(100, 0);
      oled.print(getVolltage());  // volt * 3.26 * ((r1 + r2) / r2) / 4096
      printPointer(pointer);
      oled.update();
    }

    // ||||| Кнопки |||||
    if (up.click() or up.step()) {
      pointer = constrain(pointer - 1, 0, ITEMS - 1);
      displayUpdate = 1;
    }
    if (down.click() or down.step()) {
      pointer = constrain(pointer + 1, 0, ITEMS - 1);
      displayUpdate = 1;
    }
    if (ok.click() or ok.hold()) {
      switch (pointer) {
        case 0: rcLay(); break;
        case 1: rawLay(); break;
        case 2: detectSignal(); break;
        case 3:
          rc12bitBruteForce();
          ELECHOUSE_cc1101.goSleep();
          break;
        case 4: bluetoothLay(); break;
        case 5: gpioLay(); break;
        case 6: settings(); break;
        case 7: games(); break;
      }
      displayUpdate = 1;
    }
  }
}

#pragma region RC RAW LAY
void rawLay() {
  bool updDisplay = true;
  while (1) {
    static uint8_t pointer = 0;
    tk();
    if (updDisplay) {
      updDisplay = false;
      oled.clear();
      oled.home();
      for (int i = 0; i < 8; i++) {
        int ui = i + ((pointer / 8) * 8);
        oled.setCursor(14, i);
        String j = String(ui);
        if (objectDoc[j].as<JsonObject>().isNull()) {
          oled.print("__");
        } else {
          oled.print(objectDoc[j]["name"].as<const char*>());
        }
      }
      printPointer(pointer);
      oled.update();
    }

    if (ok.click() or ok.hold()) {
      String j = String(pointer);
      if (objectDoc[j]) {
        digitalWrite(led_pin, HIGH);

        oled.clear();
        oled.home();
        oled.print("Translation..");
        oled.update();

        JsonDocument rawSignal1;
        readJsonFromFile(objectDoc[j]["RawData"].as<const char*>(), rawSignal1); // Текст null
        sendSamples(rawSignal1["RAW"].as<JsonArray>());
        digitalWrite(led_pin, LOW);
        ELECHOUSE_cc1101.goSleep();
      }
      updDisplay = 1;
    }

    if (up.click() or up.step()) {
      pointer = constrain(pointer - 1, 0, RC_ITEMS - 1);
      updDisplay = 1;
    }
    if (down.click() or down.step()) {
      pointer = constrain(pointer + 1, 0, RC_ITEMS - 1);
      updDisplay = 1;
    }
    if (right.click() or right.hold()) {
      String j = String(pointer);
      JsonDocument rawSignal;
      JsonArray RawSignalArray = rawSignal["RAW"].to<JsonArray>();
      RecordSignal(pointer, RawSignalArray, objectDoc);
      objectDoc[j]["RawData"] = "/raw"+j+".json";
      saveJsonToFile("/raw.json", doc);
      saveJsonToFile(objectDoc[j]["RawData"].as<const char*>(), rawSignal);

      ELECHOUSE_cc1101.goSleep();
      tk();
      updDisplay = 1;
    }
    if (back.click() or back.hold()) break;
    if (left.hold()) {
      rawMenu(pointer);
      updDisplay = 1;
    }
    if(top_left.click() or top_left.hold()) {
      settingsRawBW();
      updDisplay = 1;
    }
  }
}

#pragma region RC RAW MENU
void rawMenu(uint8_t pointer1) {
  String j = String(pointer1);
  bool updDisplay = true;
  tk();
  while (1) {
    static uint8_t pointer = 2;
    tk();
    if (updDisplay) {
      updDisplay = 0;
      oled.clear();
      oled.home();
      oled.setCursor(14, 0);
      oled.print("OBJ: ");
      if (objectDoc[j].as<JsonObject>().isNull()) {
        oled.print("[EMPTY]");
      } else {
        oled.print(objectDoc[j]["name"].as<const char*>());
      }
      oled.setCursor(14, 1);
      oled.print("size: ");
      oled.print(objectDoc[j]["size"].as<int>());
      oled.setCursor(14, 2);
      oled.print(" Rename");
      oled.setCursor(14, 3);
      oled.print(" Viewer");
      oled.setCursor(14, 4);
      oled.print(" Delete");

      printPointer(pointer);
      oled.update();
    }
    if (up.click() or up.step()) {
      pointer = constrain(pointer - 1, 2, 4);
      updDisplay = 1;
    }
    if (down.click() or down.step()) {
      pointer = constrain(pointer + 1, 2, 4);
      updDisplay = 1;
    }
    if (back.click() or back.hold()) break;
    if (ok.click() or ok.hold()) {
      switch (pointer) {
        case 2:
          objectDoc[j]["name"] = setName("Raw");
          break;
        case 3:
          viewer(j);
          break;
        case 4:
          if (SPIFFS.exists(objectDoc[j]["RawData"].as<const char*>())) {
            SPIFFS.remove(objectDoc[j]["RawData"].as<const char*>());
          }
          objectDoc.remove(j);
          saveJsonToFile("/raw.json", doc);
          return;
          break;
      }
      saveJsonToFile("/raw.json", doc);
      updDisplay = 1;
    }
  }
}

#pragma region RC LAY
void rcLay() {
  bool displayUpdate = 1;
  while (1) {
    static uint8_t pointer = 0;
    tk();
    if (displayUpdate != 0) {
      displayUpdate = 0;
      oled.clear();
      oled.home();
      for (uint8_t i = 0; i < 8; i++) {
        int ui = i + ((pointer / 8) * 8);
        oled.setCursor(14, i);
        // oled.print(trans_lay[ui]);
        String j = String(ui);
        if (objectRC[j].isNull()) {
          oled.print("__");
        } else {
          oled.print(objectRC[j]["name"].as<const char*>());
        }
      }
      printPointer(pointer);
      oled.update();
    }


    if (up.click() or up.step()) {
      pointer = constrain(pointer - 1, 0, RC_ITEMS - 1);
      displayUpdate = 1;
    }
    if (down.click() or down.step()) {
      pointer = constrain(pointer + 1, 0, RC_ITEMS - 1);
      displayUpdate = 1;
    }
    if (back.click() or back.hold()) break;
    if (ok.click() or ok.hold()) {
      func_trans(pointer);
      ELECHOUSE_cc1101.goSleep();
      displayUpdate = 1;
    }
    if (left.hold()) {
      menuRc(pointer);
      displayUpdate = 1;
    }
    if (right.click() or right.hold()) {
      String j = String(pointer);
      if (!objectRC[j].isNull()) {
        if (!confirmReWrite()) return;
      }
      func_reciv(pointer);
      ELECHOUSE_cc1101.goSleep();
      displayUpdate = 1;
    }
    delay(1);
  }
}

#pragma region DETECT SYGNAL
void detectSignal() {
  setupRx();
  // ELECHOUSE_cc1101.setRxBW(64);
  byte signalDetected = 0;
  bool changed = false;
  detectedRssi = -100;
  detectedFrequency = 0.0;
  fineRssi = -100;
  fineFrequency = 0.0;
  oled.clear();
  oled.home();
  oled.print("Frequency Analyzer..");
  oled.setCursor(0, 1);
  oled.print("minRssi: ");
  oled.print(minRssi);
  oled.update();
  while (signalDetected <= 5) {
    tk();
    if (back.click()) return;
    if (ok.click()) {
      detectSignal();
      return;
    }
    if (up.click() or up.step()) {
      minRssi++;
      changed = true;
    }
    if (down.click() or down.step()) {
      minRssi--;
      changed = true;
    }
    if (changed) {
      changed = false;
      oled.setCursor(0, 1);
      oled.print("minRssi: ");
      oled.print(minRssi);
      oled.update();
    }
    detectedRssi = -100;

    for (float fMhz : signalDetectionFrequencies) {
      ELECHOUSE_cc1101.setMHZ(fMhz);
      int rssi = ELECHOUSE_cc1101.getRssi();
      if (rssi >= detectedRssi) {
        detectedRssi = rssi;
        detectedFrequency = fMhz;
      }
    }
    for (float fMhz : signalDetectionFrequencies) {
      ELECHOUSE_cc1101.setMHZ(fMhz);
      int rssi = ELECHOUSE_cc1101.getRssi();
      if (rssi >= detectedRssi) {
        detectedRssi = rssi;
        detectedFrequency = fMhz;
      }
    }

    if (detectedRssi >= minRssi) {

      for (float i = detectedFrequency - 0.3; i < detectedFrequency + 0.3; i += 0.02) {
        float frequency = i;
        ELECHOUSE_cc1101.setMHZ(frequency);
        int rssi = ELECHOUSE_cc1101.getRssi();

        if (rssi >= fineRssi) {
          fineRssi = rssi;
          fineFrequency = frequency;
        }
      }

      signalDetected++;

      // DEBUG
      Serial.print("MHZ: ");
      Serial.print(detectedFrequency);
      Serial.print(" RSSI: ");
      Serial.println(detectedRssi);

      Serial.print("Fine MHZ: ");
      Serial.print(fineFrequency);
      Serial.print(" Fine RSSI: ");
      Serial.println(fineRssi);

      oled.setCursor(0, 1 + signalDetected);
      // oled.print("M: ");
      oled.print(detectedFrequency);
      oled.print(",");
      oled.print(detectedRssi);
      oled.print(" ");
      oled.print(fineFrequency);
      oled.print(",");
      oled.println(fineRssi);
      oled.update();
      delay(1);
    }
  }
  while (1) {
    tk();
    if (back.click()) return;
    if (ok.click()) {
      detectSignal();
      return;
    }
    if (up.click()) {
      minRssi++;
      changed = true;
    }
    if (down.click()) {
      minRssi--;
      changed = true;
    }
    if (changed) {
      changed = false;
      oled.setCursor(0, 1);
      oled.print("minRssi: ");
      oled.print(minRssi);
      oled.update();
    }
  }
}

#pragma region BLUETOOTH
void bluetoothLay() {
  static uint8_t pointer = 0;
  bool displayUpdate = 1;
  while (1) {
    tk();
    if (displayUpdate != 0) {
      displayUpdate = 0;
      oled.clear();
      oled.home();
      for (uint8_t i = 0; i < 8; i++) {
        oled.setCursor(14, i);
        oled.print(blue_lay[i]);
      }
      printPointer(pointer);
      oled.update();
    }

    if (up.click() or up.step()) {
      pointer = constrain(pointer - 1, 0, ITEMS - 1);
      displayUpdate = 1;
    }
    if (down.click() or down.step()) {
      pointer = constrain(pointer + 1, 0, ITEMS - 1);
      displayUpdate = 1;
    }
    if (back.click() or back.hold()) break;
    if (ok.click() or ok.hold()) {
      switch (pointer) {
        case 0: selectKeyboard(); break;  // selectKeyboard()
        case 1: selectMouse(); break;
        case 2: nothing(); break;
        case 3: nothing(); break;
        case 4: nothing(); break;
        case 5: nothing(); break;
        case 6: nothing(); break;
        case 7: nothing(); break;
      }
      displayUpdate = 1;
    }
  }
}




#pragma region RC MENU
void menuRc(uint8_t pointer1) {
  String j = String(pointer1);
  static uint8_t pointer = 3;
  bool updDisplay = true;
  while (1) {
    tk();

    if (updDisplay) {
      updDisplay = 0;
      oled.clear();
      oled.home();
      oled.setCursor(14, 0);
      oled.print("OBJ: ");
      if (objectRC[j].isNull()) {
        oled.print("[EMPTY]");
      } else {
        oled.print(objectRC[j]["name"].as<const char*>());
      }
      oled.setCursor(14, 1);
      oled.print("data: ");
      oled.print(objectRC[j]["data"].as<unsigned long>());

      oled.setCursor(14, 2);
      oled.print("bytes: ");
      oled.print(objectRC[j]["bytes"].as<unsigned int>());

      oled.setCursor(14, 3);
      oled.print(" Rename");

      oled.setCursor(14, 4);
      oled.print(" Edit");

      oled.setCursor(14, 5);
      oled.print(" Delete");

      printPointer(pointer);
      oled.update();
    }

    if (up.click() or up.step()) {
      pointer = constrain(pointer - 1, 3, 5);
      updDisplay = true;
    }
    if (down.click() or down.step()) {
      pointer = constrain(pointer + 1, 3, 5);
      updDisplay = true;
    }
    if (back.click() or back.hold()) break;
    if (ok.click() or ok.hold()) {
      switch (pointer) {
        case 3:
          reNameRC(pointer1);
          return;
          break;
        case 4:
          changeRC(pointer1);
          return;
          break;
        case 5:
          deleteRC(pointer1);
          return;
          break;
      }
    }
  }
}

#pragma region DELETE RC LAY
void deleteRC(int pointer1) {
  String j = String(pointer1);
  static uint8_t pointer = 1;
  bool updDisplay = 1;
  while (1) {
    tk();

    if (updDisplay) {
      updDisplay = 0;
      oled.clear();
      oled.home();
      oled.setCursor(28, 0);
      oled.print("Delete RC?");
      oled.setCursor(14, 1);
      oled.print("No");
      oled.setCursor(14, 2);
      oled.print("Yes");

      printPointer(pointer);
      oled.update();
    }

    if (ok.click() or ok.hold()) {
      switch (pointer) {
        case 1:
          return;
          break;
        case 2:
          select_deleteRc(pointer1);
          return;
          break;
      }
    }
    if(back.click()) return;

    if (up.click() or up.step()) {
      pointer = constrain(pointer - 1, 1, 2);
      updDisplay = true;
    }
    if (down.click() or down.step()) {
      pointer = constrain(pointer + 1, 1, 2);
      updDisplay = true;
    }
  }
}

#pragma region RENAME RC LAY
void reNameRC(int pointer) {
  String j = String(pointer);
  String mStr = setName("Raw");
  if (mStr.startsWith("tmp - ")) {
    return;
  }
  objectRC[j]["name"] = mStr;
  saveJsonToFile("/rc.json", docRC);
}

#pragma region MODIFY RC LAY
void changeRC(int pointer) {
  String j = String(pointer);
  objectRC[j]["data"] = setNumber("Change code", objectRC[j]["data"]);
  saveJsonToFile("/rc.json", docRC);

  // int data = setNumber("Change code", save_data.data_prog[pointer]);
  // save_data.data_prog[pointer] = data;
  // snprintf(trans_lay[pointer], colls, "%lu - %hu", save_data.data_prog[pointer], save_data.data_prog1[pointer]);
  // write_eeprom();
}


void select_deleteRc(uint8_t pointer) {
  String j = String(pointer);
  objectRC.remove(j);
  saveJsonToFile("/rc.json", docRC);
  // save_data.data_prog[pointer] = 0;
  // save_data.data_prog1[pointer] = 0;
  // trans_lay[pointer] = "__";
  // write_eeprom();
}

#pragma region TRANSMIT
void func_trans(uint8_t codek) {
  // String j = String(codek);
  char j[25];
  itoa(codek, j, 10);
  if (objectRC[j]["data"].isNull()) return;
  setupTx();
  digitalWrite(led_pin, HIGH);
  oled.clear();
  oled.setCursor(0, 0);
  oled.print("Transmition..");
  oled.update();
  // mySwitch.send(save_data.data_prog[codek], save_data.data_prog1[codek]);
  mySwitch.send(objectRC[j]["data"], objectRC[j]["bytes"]);
  // delay(400);
  digitalWrite(led_pin, LOW);
}


void func_custom_translate(unsigned long address, uint8_t byte) {
  setupTx();
  digitalWrite(led_pin, HIGH);
  oled.clear();
  oled.setCursor(0, 0);
  oled.print("Transmition..");
  oled.update();
  mySwitch.send(address, byte);
  // delay(700);
  digitalWrite(led_pin, LOW);
}

#pragma region RECEIVE
void func_reciv(uint8_t cutr) {
  setupRx();
  tk();
  int uo = 1;
  oled.clear();
  oled.home();
  oled.print("Reading..");
  // oled.print(ELECHOUSE_cc1101.getMode());
  oled.update();
  digitalWrite(led_pin, HIGH);
  while (uo == 1) {
    tk();
    if (back.click() or back.hold()) {
      digitalWrite(led_pin, LOW);
      return;
    }
    if (mySwitch.available()) {
      store1 = mySwitch.getReceivedValue();
      bitR1 = mySwitch.getReceivedBitlength();
      protoc = mySwitch.getReceivedProtocol();
      Serial.print("Received ");
      Serial.print(store1);
      Serial.print(" / ");
      Serial.print(bitR1);
      Serial.print("bit ");
      Serial.print("Protocol: ");
      Serial.println(protoc);

      mySwitch.resetAvailable();
      uo = 0;
      digitalWrite(led_pin, LOW);
    }
  }
  String j = String(cutr);
  char def[20];
  snprintf(def, 20, "%lu - %hu", store1, bitR1);
  String mStr = setName("Signal");  // <----
  if (mStr.startsWith("tmp - ")) {
    mStr = def;
  }
  objectRC[j]["name"] = mStr;
  objectRC[j]["frequency"] = FrequencyPointer;
  objectRC[j]["data"] = store1;
  objectRC[j]["bytes"] = bitR1;

  saveJsonToFile("/rc.json", docRC);
}

#pragma region BRUTE FORCE
void rc12bitBruteForce() {
  setupTx();
  // mySwitch.setPulseLength(250);
  oled.clear();
  oled.home();
  oled.print("Rc 12bit brute force");
  // oled.setCursor(0, 4);
  // oled.print("        0/4096      ");
  oled.update();

  uint8_t byt = 12;
  int iy = 1;
  for (int i = start12bitBruteForce; i < 4096; i++) {
    tk();
    if (back.click() or back.hold()) {

      // mySwitch.setPulseLength(350);
      return;
    }
    if (ok.click() or ok.hold()) break;
    // oled.clear();
    mySwitch.send(i, byt);
    iy = i;
    oled.setCursor(0, 4);
    oled.print("     ");
    int draw = i;
    while (draw < 1000) {
      draw *= 10;
      oled.print(" ");
    }
    oled.print(i);
    oled.print("/4096      ");
    oled.update();
  }

  while (1) {
    static bool upd = 1;
    tk();
    if (back.click() or back.hold()) {
      // mySwitch.setPulseLength(350);
      return;
    };
    if (ok.click() or ok.hold()) {
      func_custom_translate(iy, byt);
      ELECHOUSE_cc1101.goSleep();
      upd = 1;
    }
    if (up.click()) {
      iy++;
      upd = 1;
    }
    if (down.click()) {
      iy--;
      upd = 1;
    }
    if (upd) {
      upd = 0;
      oled.setCursor(0, 4);
      oled.print("     ");
      long draw = iy;
      while (draw < 1000) {
        draw *= 10;
        oled.print(" ");
      }
      oled.print(iy);
      oled.print("/4096      ");
      oled.update();
    }
  }

  // mySwitch.setPulseLength(350);
}
