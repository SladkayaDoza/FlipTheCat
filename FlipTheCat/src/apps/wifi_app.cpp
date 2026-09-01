#include <Arduino.h>
#include <WiFi.h>
#include "esp_wifi.h"
#include <display.h>
#include <button.h>
#include <tick.h>
#include <pointer.h>
#include <menu.h>
#include "wifi_app.h"

// =============================================================================
//  Wi-Fi модуль — только пассивный приём. Ничего не передаёт в эфир.
// =============================================================================

// --- Строковое имя типа шифрования ---
static const char* wifiEncStr(wifi_auth_mode_t e) {
  switch (e) {
    case WIFI_AUTH_OPEN:            return "OPEN";
    case WIFI_AUTH_WEP:             return "WEP";
    case WIFI_AUTH_WPA_PSK:         return "WPA";
    case WIFI_AUTH_WPA2_PSK:        return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK:    return "WPA/2";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2-E";
#ifdef WIFI_AUTH_WPA3_PSK
    case WIFI_AUTH_WPA3_PSK:        return "WPA3";
#endif
#ifdef WIFI_AUTH_WPA2_WPA3_PSK
    case WIFI_AUTH_WPA2_WPA3_PSK:   return "WPA2/3";
#endif
    default:                        return "?";
  }
}

// Обрезка SSID до n символов (иначе выходит за край экрана).
static String wifiShortSsid(const String& s, uint8_t n) {
  if (s.length() == 0) return String("<hidden>");
  if (s.length() <= n) return s;
  return s.substring(0, n);
}

// Подробности выбранной сети.
static void wifiScanDetail(int idx) {
  bool upd = true;
  while (1) {
    tk();
    if (upd) {
      upd = false;
      oled.clear();
      oled.home();
      oled.setCursor(0, 0);
      oled.print(wifiShortSsid(WiFi.SSID(idx), 20));
      oled.setCursor(0, 1);
      oled.print(WiFi.BSSIDstr(idx));
      oled.setCursor(0, 2);
      oled.print("ch ");
      oled.print(WiFi.channel(idx));
      oled.setCursor(0, 3);
      oled.print("rssi ");
      oled.print(WiFi.RSSI(idx));
      oled.setCursor(0, 4);
      oled.print("enc ");
      oled.print(wifiEncStr(WiFi.encryptionType(idx)));
      oled.setCursor(0, 7);
      oled.print("back/ok - назад");
      oled.update();
    }
    if (back.click() || back.hold() || ok.click()) return;
  }
}

// --- Сканер точек доступа ---
static void wifiScanner() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(50);

  oled.clear();
  oled.home();
  oled.print("WiFi scan...");
  oled.update();

  int n = WiFi.scanNetworks();  // блокирующий скан (~2-4 c)

  uint8_t pointer = 0;
  bool upd = true;
  while (1) {
    tk();
    if (upd) {
      upd = false;
      oled.clear();
      oled.home();
      if (n <= 0) {
        oled.setCursor(0, 0);
        oled.print("Сетей не найдено");
        oled.setCursor(0, 2);
        oled.print("ok - пересканировать");
        oled.setCursor(0, 3);
        oled.print("back - выход");
      } else {
        uint8_t page = (pointer / 8) * 8;
        for (uint8_t row = 0; row < 8; row++) {
          int idx = page + row;
          if (idx >= n) break;
          oled.setCursor(8, row);
          oled.print(WiFi.channel(idx));
          oled.print(" ");
          oled.print(WiFi.RSSI(idx));
          oled.print(" ");
          oled.print(wifiShortSsid(WiFi.SSID(idx), 12));
        }
        printPointer(pointer);
      }
      oled.update();
    }

    if (n > 0) {
      if (up.click() || up.step())   { pointer = constrain(pointer - 1, 0, n - 1); upd = true; }
      if (down.click() || down.step()){ pointer = constrain(pointer + 1, 0, n - 1); upd = true; }
      if (ok.click()) { wifiScanDetail(pointer); upd = true; }
    } else {
      if (ok.click()) {                       // пересканировать
        oled.clear(); oled.home(); oled.print("WiFi scan..."); oled.update();
        WiFi.scanDelete();
        n = WiFi.scanNetworks();
        pointer = 0;
        upd = true;
      }
    }
    if (back.click() || back.hold()) break;
  }

  WiFi.scanDelete();
  WiFi.mode(WIFI_OFF);
}

// --- Монитор каналов (шумность по числу сетей) ---
static void wifiChannelMonitor() {
  const int CH = 13;
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(50);

  while (1) {
    oled.clear();
    oled.home();
    oled.print("Scanning...");
    oled.update();

    int n = WiFi.scanNetworks();
    int counts[CH + 1] = {0};
    for (int i = 0; i < n; i++) {
      int c = WiFi.channel(i);
      if (c >= 1 && c <= CH) counts[c]++;
    }
    WiFi.scanDelete();

    int maxc = 1;
    for (int c = 1; c <= CH; c++)
      if (counts[c] > maxc) maxc = counts[c];

    oled.clear();
    oled.home();
    oled.setCursor(0, 0);
    oled.print("Ch load, APs=");
    oled.print(n < 0 ? 0 : n);
    for (int c = 1; c <= CH; c++) {
      int x = (c - 1) * 9 + 2;                 // 13 столбцов по 9px
      int h = map(counts[c], 0, maxc, 0, 40);
      oled.rect(x, 62, x + 6, 62 - h);
    }
    oled.update();

    // Ждём ввод между сканами (скан блокирующий, поэтому реакция здесь).
    while (1) {
      tk();
      if (back.click() || back.hold()) { WiFi.mode(WIFI_OFF); return; }
      if (ok.click()) break;                   // пересканировать
    }
  }
}

// --- Пассивный сниффер пакетов (promiscuous) ---
static volatile uint32_t sniffTotal, sniffMgmt, sniffData, sniffCtrl, sniffBeacon;

static void wifiSniffReset() {
  sniffTotal = sniffMgmt = sniffData = sniffCtrl = sniffBeacon = 0;
}

// Колбэк выполняется в задаче Wi-Fi — только инкремент счётчиков, без блокировок.
static void wifiSniffCb(void* buf, wifi_promiscuous_pkt_type_t type) {
  sniffTotal++;
  const wifi_promiscuous_pkt_t* p = (const wifi_promiscuous_pkt_t*)buf;
  uint8_t fc = p->payload[0];                  // frame control (младший байт)
  uint8_t ftype = (fc >> 2) & 0x3;
  uint8_t fsub  = (fc >> 4) & 0xF;
  if (type == WIFI_PKT_MGMT) {
    sniffMgmt++;
    if (ftype == 0 && fsub == 8) sniffBeacon++;  // beacon = mgmt/subtype 8
  } else if (type == WIFI_PKT_DATA) {
    sniffData++;
  } else if (type == WIFI_PKT_CTRL) {
    sniffCtrl++;
  }
}

static void wifiSniffer() {
  int channel = 1;
  wifiSniffReset();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(50);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&wifiSniffCb);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  uint32_t lastDraw = millis();
  bool upd = true;
  while (1) {
    tk();

    if (up.click() || up.step()) {
      channel = constrain(channel + 1, 1, 13);
      esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
      wifiSniffReset();
      upd = true;
    }
    if (down.click() || down.step()) {
      channel = constrain(channel - 1, 1, 13);
      esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
      wifiSniffReset();
      upd = true;
    }
    if (back.click() || back.hold()) break;

    if (millis() - lastDraw > 300) { lastDraw = millis(); upd = true; }

    if (upd) {
      upd = false;
      oled.clear();
      oled.home();
      oled.setCursor(0, 0); oled.print("Sniffer ch "); oled.print(channel);
      oled.setCursor(0, 1); oled.print("total  "); oled.print(sniffTotal);
      oled.setCursor(0, 2); oled.print("mgmt   "); oled.print(sniffMgmt);
      oled.setCursor(0, 3); oled.print("beacon "); oled.print(sniffBeacon);
      oled.setCursor(0, 4); oled.print("data   "); oled.print(sniffData);
      oled.setCursor(0, 5); oled.print("ctrl   "); oled.print(sniffCtrl);
      oled.setCursor(0, 7); oled.print("up/dn=ch back=exit");
      oled.update();
    }
  }

  esp_wifi_set_promiscuous(false);
  WiFi.mode(WIFI_OFF);
}

// --- Список станций/устройств в эфире (пассивно, promiscuous) ---
// Собираем уникальные MAC-адреса передатчиков (addr2 802.11) на выбранном канале
// с их RSSI и числом кадров. Полезно как инвентаризация и поиск чужих устройств.
#define STA_MAX 32
struct StaEntry {
  uint8_t mac[6];
  int8_t rssi;
  uint16_t count;
};
static StaEntry staList[STA_MAX];
static volatile uint8_t staCount = 0;
static portMUX_TYPE staMux = portMUX_INITIALIZER_UNLOCKED;

static void staReset() {
  portENTER_CRITICAL(&staMux);
  staCount = 0;
  portEXIT_CRITICAL(&staMux);
}

// Колбэк в задаче Wi-Fi. Поиск — без блокировки (записи только добавляются),
// критическая секция — только на добавление новой записи.
static void wifiStaCb(void* buf, wifi_promiscuous_pkt_type_t type) {
  const wifi_promiscuous_pkt_t* p = (const wifi_promiscuous_pkt_t*)buf;
  if (p->rx_ctrl.sig_len < 16) return;       // слишком короткий кадр — нет addr2
  const uint8_t* a2 = p->payload + 10;        // addr2 = MAC передатчика
  int8_t rssi = p->rx_ctrl.rssi;

  uint8_t n = staCount;                        // снимок
  for (uint8_t i = 0; i < n; i++) {
    if (memcmp(staList[i].mac, a2, 6) == 0) {
      staList[i].rssi = rssi;
      if (staList[i].count < 0xFFFF) staList[i].count++;
      return;
    }
  }
  portENTER_CRITICAL(&staMux);
  if (staCount < STA_MAX) {
    memcpy(staList[staCount].mac, a2, 6);
    staList[staCount].rssi = rssi;
    staList[staCount].count = 1;
    staCount++;                                // инкремент после полного заполнения записи
  }
  portEXIT_CRITICAL(&staMux);
}

static void wifiStations() {
  int channel = 1;
  staReset();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(50);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&wifiStaCb);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  uint8_t pointer = 0;
  uint32_t lastDraw = millis();
  bool upd = true;
  while (1) {
    tk();

    // left/right — канал (сбрасывает список), up/down — прокрутка списка
    if (left.click()) {
      channel = constrain(channel - 1, 1, 13);
      esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
      staReset(); pointer = 0; upd = true;
    }
    if (right.click()) {
      channel = constrain(channel + 1, 1, 13);
      esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
      staReset(); pointer = 0; upd = true;
    }
    uint8_t n = staCount;
    if (n > 0) {
      if (up.click() || up.step())    { pointer = constrain(pointer - 1, 0, n - 1); upd = true; }
      if (down.click() || down.step()){ pointer = constrain(pointer + 1, 0, n - 1); upd = true; }
    }
    if (back.click() || back.hold()) break;

    if (millis() - lastDraw > 700) { lastDraw = millis(); upd = true; }

    if (upd) {
      upd = false;
      oled.clear();
      oled.home();
      oled.setCursor(0, 0);
      oled.print("Sta ch");
      oled.print(channel);
      oled.print(" n=");
      oled.print(n);

      if (n == 0) {
        oled.setCursor(0, 3);
        oled.print("Listening...");
        oled.setCursor(0, 5);
        oled.print("<> ch  ^v scroll");
      } else {
        uint8_t page = (pointer / 7) * 7;       // 7 строк списка (строка 0 — шапка)
        for (uint8_t row = 0; row < 7; row++) {
          uint8_t idx = page + row;
          if (idx >= n) break;
          StaEntry e;
          portENTER_CRITICAL(&staMux);
          e = staList[idx];
          portEXIT_CRITICAL(&staMux);
          char line[26];
          snprintf(line, sizeof(line), "%02X%02X%02X%02X%02X%02X %d",
                   e.mac[0], e.mac[1], e.mac[2], e.mac[3], e.mac[4], e.mac[5], e.rssi);
          oled.setCursor(6, row + 1);
          oled.print(line);
        }
        oled.setCursor(0, (pointer % 7) + 1);   // курсор
        oled.print(">");
      }
      oled.update();
    }
  }

  esp_wifi_set_promiscuous(false);
  WiFi.mode(WIFI_OFF);
}

// --- Подменю Wi-Fi (реестр модулей) ---
static const MenuItem wifiMenu[] = {
  {"Scanner", wifiScanner},
  {"Ch Monitor", wifiChannelMonitor},
  {"Sniffer", wifiSniffer},
  {"Stations", wifiStations},
};
static const uint8_t wifiMenuCount = sizeof(wifiMenu) / sizeof(wifiMenu[0]);

void wifiLay() {
  runMenu(wifiMenu, wifiMenuCount);
}
