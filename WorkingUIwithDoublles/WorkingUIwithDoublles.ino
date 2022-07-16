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

boolean incr;
int incrAmount;
uint32_t tmr = 0;
bool onTime = false;
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

void tower() {
  digitalWrite(A0, LOW);
}

void catapultL() {
  digitalWrite(A1, LOW);
}
  
void catapultR() {
  digitalWrite(A2, LOW);
}

void setup() {
  Serial.begin(9600);
  enc1.setType(TYPE2);
  pinMode(A3, INPUT_PULLUP); // пин кнопки
  pinMode(A0, OUTPUT); //пины лампочек(поршней) 
  pinMode(A1, OUTPUT); 
  pinMode(A2, OUTPUT);
  lcd.init();
  lcd.backlight();
  vals[0] = 1000; // изначальный значения
  vals[1] = 1000;
  vals[2] = 1000;
  printGUI();   // выводим интерфейс
}


void loop() {
  enc1.tick();
  butt = !digitalRead(A3); // считывание состояния кнопки
  digitalWrite(A0, HIGH);  //  Изначальные значения low = вкл, high = выкл
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);

if (butt && !onTime){
  onTime = true;
  tmr = millis();
  butt = 0;
  }
  
if (onTime && millis()-tmr >= vals[0]){
  tower();
  }
if (onTime && millis()-tmr >= vals[1]){
  catapultL();
  }
if (onTime && millis()-tmr >= vals[2]){
  catapultR();
  }

if (vals[0] < 0){
  vals[0] = 0;
  lcd.clear();
  printGUI();
  }
if (vals[1] < 0){
  vals[1] = 0;
  lcd.clear();
  printGUI();
  }
if (vals[2] < 0){
  vals[2] = 0;
  lcd.clear();
  printGUI();
  }
if (enc1.isDouble()){
  incr = !incr;
  } 
if (incr == 1){
  incrAmount = 100;
  }
else{
  incrAmount = 1;
}
  if (enc1.isTurn()) {
    int increment = 0;  // локальная переменная направления  
    // получаем направление   
    if (enc1.isRight()) increment = 1;
    if (enc1.isLeft()) increment = -1;
    arrowPos += increment;  // двигаем курсор  
    arrowPos = constrain(arrowPos, 0, SETTINGS_AMOUNT - 1); // ограничиваем

    increment = 0;  // обнуляем инкремент
    if (enc1.isRightH()) increment = incrAmount;
    if (enc1.isLeftH()) increment = -incrAmount;
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
