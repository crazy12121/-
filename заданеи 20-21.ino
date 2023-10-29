// Библиотека для работы с текстовым дисплеем
#include <LiquidCrystalRus.h>
 // библиотека для работы с датчиками MQ (Troyka-модуль)
#include <TroykaMQ.h>
// имя для пина, к которому подключен датчик
#define PIN_MQ7         A0
// имя для пина, к которому подключен нагреватель датчика
#define PIN_MQ7_HEATER  13
// создаём объект для работы с датчиком
// и передаём ему номер пина выходного сигнала и нагревателя
MQ7 mq7(PIN_MQ7, PIN_MQ7_HEATER);
// Задаём имя пинов дисплея
constexpr uint8_t PIN_RS = 6;
constexpr uint8_t PIN_EN = 7;
constexpr uint8_t PIN_DB4 = 8;
constexpr uint8_t PIN_DB5 = 9;
constexpr uint8_t PIN_DB6 = 10;
constexpr uint8_t PIN_DB7 = 11;
//пин кнопки
const int buttonPin = 6; // Пин для кнопки
const int buttonPin1 = 1; // Пин для кнопки
// Переменная с значениями датчика газа
int gasLevel = 0;
//Мин для зелёного света
const int greenLedPin = 4;
// Пин для синего света
const int yellowLedPin = 3;
// Пин для красного света
const int redLedPin = 2;
// Пин для бузера
const int buzzerPin = 5;
// Пороговые значения уровня газа
int controlThreshold = 50; // Некритическое превышение порогового уровня газа
int criticalThreshold = 100; // Критическое превышение порогового уровня газа
int soundEnabled = true; // Флаг для отключения/включения звука

LiquidCrystalRus lcd(PIN_RS, PIN_EN, PIN_DB4, PIN_DB5, PIN_DB6, PIN_DB7);
 
void setup() {
  lcd.begin(16, 2);
  lcd.clear();
  

  pinMode(greenLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buttonPin1, INPUT_PULLUP);
  mq7.cycleHeat();
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonInterrupt, FALLING); 
  attachInterrupt(0, btnIsr, FALLING); 
}

void loop() { 
  lcd.setCursor(0, 0);
  lcd.print("Ур. газа: ");
    
  if (!mq7.isCalibrated() && mq7.atHeatCycleEnd()) {
    mq7.calibrate();
    mq7.cycleHeat();
  }

  if (mq7.isCalibrated() && mq7.atHeatCycleEnd()) {
    gasLevel = mq7.readCarbonMonoxide();
    lcd.setCursor(0, 0);
    lcd.print("Ур. газа: ");
    lcd.print(gasLevel);
    lcd.print(" ppm ");
    delay(100);
    mq7.cycleHeat();
  }
  
  if (gasLevel <= controlThreshold) {
    digitalWrite(greenLedPin, HIGH);
    digitalWrite(yellowLedPin, LOW);
    digitalWrite(redLedPin, LOW);
    noTone(buzzerPin);
    lcd.setCursor(0, 1);
    lcd.print("ОК");
  } else if (gasLevel <= criticalThreshold) {
    RGB_color(255, 255, 0);  
    lcd.setCursor(0, 1);
    lcd.print("Контрол");
    if (soundEnabled) {
    tone(buzzerPin, 1000);
    delay(2000);
    noTone(buzzerPin);
    delay(10000);
    }

  } else {
    
    digitalWrite(greenLedPin, LOW);
    digitalWrite(yellowLedPin, LOW);
    digitalWrite(redLedPin, HIGH);
    tone(buzzerPin, 2000);
    lcd.setCursor(0, 1);
    lcd.print("Критический");
    if (soundEnabled) {
    tone(buzzerPin, 1000);
    delay(1000);
    tone(buzzerPin, 0);
    }
  }
  
  delay(1000); 
}

void RGB_color(int red_value, int green_value, int blue_value){
    analogWrite(redLedPin, red_value);
    analogWrite(greenLedPin, green_value);
    analogWrite(yellowLedPin, blue_value);
}

void buttonInterrupt() {
  soundEnabled = !soundEnabled; 
  delay(200); 
}

void btnIsr() {
  criticalThreshold++;
  delay(200); 
}