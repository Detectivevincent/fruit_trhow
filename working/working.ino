// пример с линейным меню
#define LINES 2   // количество строк дисплея
#define SETTINGS_AMOUNT 3  // количество настроек

// пины энкодера
#define CLK 8
#define DT 7
#define SW 4

#include "GyverEncoder.h"
Encoder enc1(CLK, DT, SW);  // для работы c кнопкой

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // адрес 0x27 или 0x3f

#include <TimerMs.h>


int vals[SETTINGS_AMOUNT];  // массив параметров
int8_t arrowPos = 0;
int8_t screenPos = 0; // номер "экрана"
boolean butt;
// названия параметров
const char *settingsNames[]  = {
  "Tower",
  "CatapultL",
  "CatapultR",
};

void setup() {
  Serial.begin(9600);
  enc1.setType(TYPE2);
  pinMode(A3, INPUT_PULLUP); // пин кнопки
  pinMode(A0, OUTPUT); //пины лампочек(поршней) 
  pinMode(A1, OUTPUT); 
  pinMode(A2, OUTPUT);
  lcd.init();
  lcd.backlight();
  vals[0] = 1000;
  vals[1] = 1000;
  vals[2] = 1000;
  vals[3] = 1000;
  TimerMs tmr(vals[0], 1, 1);
  printGUI();   // выводим интерфейс
}


void loop() {
  enc1.tick();
  butt = !digitalRead(A3); // считывание состояния кнопки
  Serial.println(butt, A3);
  digitalWrite(A0, HIGH);  //  Изначальные значения low = вкл, high = выкл
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);

if (butt == 1){
    delay(vals[0]);
    digitalWrite(A0, LOW); // вкл(вверх)
    delay(vals[1]);
    digitalWrite(A1, LOW); 
    delay(vals[2]);
    digitalWrite(A2, LOW);
    delay(1000);
    butt = 0;
}


  if (enc1.isTurn()) {
    int increment = 0;  // локальная переменная направления
    
    // получаем направление   
    if (enc1.isRight()) increment = 1;
    if (enc1.isLeft()) increment = -1;
    arrowPos += increment;  // двигаем курсор  
    arrowPos = constrain(arrowPos, 0, SETTINGS_AMOUNT - 1); // ограничиваем

    increment = 0;  // обнуляем инкремент
    if (enc1.isRightH()) increment = 100;
    if (enc1.isLeftH()) increment = -100;
    vals[arrowPos] += increment;  // меняем параметры

    printGUI();
  }
}

void printGUI() {
  lcd.clear();  
  screenPos = arrowPos / LINES;   // ищем номер экрана (0..3 - 0, 4..7 - 1)

  for (byte i = 0; i < LINES; i++) {  // для всех строк
    lcd.setCursor(0, i);              // курсор в начало

    // если курсор находится на выбранной строке
    if (arrowPos == LINES * screenPos + i) lcd.write(126);  // рисуем стрелку
    else lcd.write(32);     // рисуем пробел

    // если пункты меню закончились, покидаем цикл for
    if (LINES * screenPos + i == SETTINGS_AMOUNT) break;

    // выводим имя и значение пункта меню
    lcd.print(settingsNames[LINES * screenPos + i]);
    lcd.print(": ");
    lcd.print(vals[LINES * screenPos + i]);
  }
}
