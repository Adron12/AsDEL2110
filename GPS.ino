#include <SoftwareSerial.h>
#include <TinyGPS++.h>

SoftwareSerial gpsSerial(2, 3);  // RX, TX
TinyGPSPlus gps;
unsigned long lastUpdate = 0;
unsigned long lastCheck = 0;
bool gpsConnected = false;

void setGPSRate1Hz() {
    uint8_t rate_1Hz[] = {
        0xB5, 0x62, 0x06, 0x08, 0x06, 0x00,
        0xE8, 0x03, 0x01, 0x00, 0x01, 0x00,
        0x01, 0x39
    };
    gpsSerial.write(rate_1Hz, sizeof(rate_1Hz));
    Serial.println("Частота обновления GPS установлена на 1 Гц");
}

void setup() {
    Serial.begin(115200);
    gpsSerial.begin(9600);
    delay(1000);
    setGPSRate1Hz();
    Serial.println("Ожидание GPS-данных...");
}

void loop() {
    while (gpsSerial.available()) {
        char c = gpsSerial.read();
        gps.encode(c);
        gpsConnected = true;
    }

    if (millis() - lastCheck >= 5000) {
        lastCheck = millis();
        if (!gpsConnected) {
            Serial.println("Ошибка: GPS-модуль не отвечает!");
        } else if (!gps.satellites.isValid() || gps.satellites.value() == 0) {
            Serial.println("GPS работает, но спутников нет. Проверьте антенну или откройте небо.");
        } else {
            Serial.print("Спутников найдено: ");
            Serial.println(gps.satellites.value());
        }
        gpsConnected = false;
    }

    if (millis() - lastUpdate >= 1000) {
        lastUpdate = millis();
        if (gps.speed.isValid()) {
            Serial.print("Скорость: ");
            Serial.print(gps.speed.kmph());
            Serial.println(" км/ч");
        } else {
            Serial.println("Скорость недоступна...");
        }
    }
}

