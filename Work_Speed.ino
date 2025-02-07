#include <SoftwareSerial.h>
#include "TinyGPS++.h"
#include "LedControl.h"

#define GPS_RX 2
#define GPS_TX 3
#define SPEED_SENSOR_PIN 6
#define BUZZER_PIN 5
#define MAX7219_DIN 11
#define MAX7219_CLK 13
#define MAX7219_CS 10

SoftwareSerial gpsSerial(GPS_RX, GPS_TX);
TinyGPSPlus gps;
LedControl display = LedControl(MAX7219_DIN, MAX7219_CLK, MAX7219_CS, 1);

volatile int pulseCount = 0;
unsigned long lastSpeedUpdate = 0;
unsigned long lastGPSCheck = 0;
bool usingGPS = true;
int lastDisplayedSpeed = -1;

const byte digitTable[10] = {
0b01110111, // 0
0b00010010, // 1
0b01011101, // 2
0b01011011, // 3
0b00111010, // 4
0b01101011, // 5
0b01101111, // 6
0b01010010, // 7
0b01111111, // 8
0b01111011  // 9
};

void setGPSRate10Hz() {
    uint8_t rate_10Hz[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0x64, 0x00, 0x01, 0x00, 0x01, 0x00, 0x7A, 0x12};
    gpsSerial.write(rate_10Hz, sizeof(rate_10Hz));
    Serial.println("Частота обновления GPS установлена на 10 Гц");
}

void countPulses() {
    pulseCount++;
}

void setup() {
    Serial.begin(115200);
    gpsSerial.begin(9600);
    pinMode(SPEED_SENSOR_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(SPEED_SENSOR_PIN), countPulses, RISING);
    display.shutdown(0, false);
    display.setIntensity(0, 12);
    display.clearDisplay(0);
    delay(4000);
    setGPSRate10Hz();
}

void loop() {
    if (usingGPS) {
        while (gpsSerial.available()) {
            gps.encode(gpsSerial.read());
        }
        if (gps.speed.isValid()) {
            float speed = gps.speed.kmph();
            displaySpeed(speed);
            Serial.print("Скорость: ");
            Serial.print(gps.speed.kmph());
            Serial.println(" км/ч");
            lastGPSCheck = millis();
        } else if (millis() - lastGPSCheck > 5000) {
            playBuzzer(true);
            usingGPS = false;
        }
    } else {
        unsigned long elapsed = millis() - lastSpeedUpdate;
        float speed = (pulseCount / 4.0) / (elapsed / 1000.0);
        pulseCount = 0;
        lastSpeedUpdate = millis();
        displaySpeed(speed);
        if (millis() - lastGPSCheck > 5000) {
            while (gpsSerial.available()) {
                gps.encode(gpsSerial.read());
            }
            if (gps.speed.isValid()) {
                playBuzzer(false);
                usingGPS = true;
            }
            lastGPSCheck = millis();
        }
    }
}

void displaySpeed(int speed) {
    if (speed == lastDisplayedSpeed) return;
    lastDisplayedSpeed = speed;
    display.clearDisplay(0);
    if (speed < 10) {
        display.setRow(0, 0, digitTable[speed]);
    } else if (speed < 100) {
        display.setRow(0, 1, digitTable[speed / 10]);
        display.setRow(0, 0, digitTable[speed % 10]);
    } else {
        display.setRow(0, 2, digitTable[speed / 100]);
        display.setRow(0, 1, digitTable[(speed / 10) % 10]);
        display.setRow(0, 0, digitTable[speed % 10]);
    }
}

void playBuzzer(bool inverted) {
    int tones[] = {2500, 3141, 3984, 4800, 5766};
    for (int i = 0; i < 5; i++) {
         tone(BUZZER_PIN, inverted ? tones[4 - i] : tones[i]);
        delay(150);
        noTone(BUZZER_PIN);
    }
}
