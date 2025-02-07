const int piezoPin = 5;           // Пин, подключенный к базе транзистора через резистор
const int startFrequency = 3500;  // Начальная частота в Гц
const int lowerSignalFreqs[] = {2500, 3141, 3984, 4800, 5766}; // Частоты для начального сигнала
const int stepFrequency = 150;     // Шаг частоты
const int upperLimit = 5100;      // Верхняя граница частоты
const int midPoint = 4400;        // Средняя частота, где звучат сигналы
const int beepDelay = 33;         // Задержка между писками
const int signalDelay = 50;      // Задержка между частотами сигнала
const int pauseDelay = 170;       // Пауза между сериями 3-звучных сигналов
const int signalPauseDelay = 10; // Настраиваемая задержка между основным циклом и сигналами

bool increasing = true;           // Флаг направления изменения частоты
int currentFrequency = startFrequency;

void playSignal(const int freqs[], int size) {
  for (int i = 0; i < size; i++) {
    tone(piezoPin, freqs[i]);
    delay(signalDelay);
    noTone(piezoPin);
    delay(signalDelay);
  }
}

void playBeep(int freq) {
  for (int i = 0; i < 3; i++) {
    tone(piezoPin, freq);
    delay(beepDelay);
    noTone(piezoPin);
    delay(beepDelay);
  }
}

void setup() {
  pinMode(piezoPin, OUTPUT); // Устанавливаем пин как выход
}

void loop() {
  // Проверяем направление изменения частоты
  if (increasing) {
    if (currentFrequency == midPoint) {
      delay(signalPauseDelay);        // Настраиваемая задержка перед сигналами
      playSignal(lowerSignalFreqs, 5); // Воспроизводим начальный сигнал
    }
    playBeep(currentFrequency); // 3-звучный сигнал
    currentFrequency += stepFrequency;

    if (currentFrequency > upperLimit) {
      currentFrequency = upperLimit;
      increasing = false; // Меняем направление
    }
  } else {
    playBeep(currentFrequency); // 3-звучный сигнал
    currentFrequency -= stepFrequency;

    if (currentFrequency < startFrequency) {
      currentFrequency = startFrequency;
      increasing = true; // Меняем направление
    }
  }
  delay(pauseDelay); // Пауза между циклами
}

