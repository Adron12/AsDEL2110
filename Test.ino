#include <LedControl.h>

// Пины для подключения MAX7219
const int DIN_PIN = 11; // Пин DIN
const int CLK_PIN = 13; // Пин CLK
const int CS_PIN = 10;  // Пин CS

// Инициализация MAX7219 (1 устройство)
LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 1);

// Таблица сегментов для цифр от 0 до 9
const byte digitAddress[10] = {
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

// Код для отключения сегмента (пустой символ)
const byte blankDigit = 0b00000000;

// Переменные для таймера
unsigned long previousMillis = 0;  
const unsigned long interval = 50; // Интервал обновления (0.2 секунды)

// Число для отображения
int currentNumber = 0;  

void setup() {
  // Инициализация MAX7219
  lc.shutdown(0, false);       // Включаем MAX7219
  lc.setIntensity(0, 10);       // Устанавливаем яркость (0-15)
  lc.clearDisplay(0);          // Очищаем дисплей
}

void loop() {
  unsigned long currentMillis = millis();

  // Если прошло 0.2 секунды, обновляем число
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // Обновляем таймер

    // Разбиваем число на разряды
    int ones = currentNumber % 10;         // Единицы
    int tens = (currentNumber / 10) % 10;  // Десятки
    int hundreds = (currentNumber / 100);  // Сотни

    // Выводим цифры, гасим неиспользуемые разряды
    lc.setRow(0, 0, digitAddress[ones]);   // Единицы (DIG0)
    
    if (currentNumber >= 10) {
      lc.setRow(0, 1, digitAddress[tens]); // Десятки (DIG1)
    } else {
      lc.setRow(0, 1, blankDigit); // Гасим десятки
    }
    
    if (currentNumber >= 100) {
      lc.setRow(0, 2, digitAddress[hundreds]); // Сотни (DIG2)
    } else {
      lc.setRow(0, 2, blankDigit); // Гасим сотни
    }

    // Увеличиваем число
    currentNumber++;
    if (currentNumber > 999) {
      currentNumber = 0;  // Сброс при достижении 999
    }
  }
}

