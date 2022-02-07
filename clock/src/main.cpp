#include <Arduino.h>
#include <LiquidCrystal.h>

enum State { MENU, SET_TIME, SET_DATE };

State state = MENU;

int hour = 12;
int minute = 0;
int second = 0;
int year = 1900;
int month = 1;
int day = 1;

int buttonPin = A0;
int potentiometerPin = A1;

int rs = 7;
int e = 8;
int d4 = 9;
int d5 = 10;
int d6 = 11;
int d7 = 12;
LiquidCrystal lcd(rs,e,d4,d5,d6,d7);

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(potentiometerPin, INPUT);

  lcd.begin(16, 2);

  Serial.begin(9600);
}

float inputToVoltage (int input) {
  return input * (5.0 / 1023.0);
}

void updateTime () {
  second++;
  if (second >= 60) {
    second = 0;
    minute++;
  }
  if (minute >= 60) {
    minute = 0;
    hour++;
  }
  if (hour >= 24) {
    hour = 0;
  }
}

void loop() {
  // int potentiometerInput = analogRead(potentiometerPin);
  // float inputVoltage = inputToVoltage(potentiometerInput);
  // Serial.print("Potentiometer Voltage: ");
  // Serial.println(inputVoltage);

  String hourString = hour >= 10 ? String(hour) : "0" + String(hour);
  String minuteString = minute >= 10 ? String(minute) : "0" + String(minute);
  String secondString = second >= 10 ? String(second) : "0" + String(second);
  String monthString = month >= 10 ? String(month) : "0" + String(month);
  String dayString = day >= 10 ? String(day) : "0" + String(day);

  switch (state) {
    case MENU:
        lcd.setCursor(0, 0);
        lcd.print(">Time:" + hourString + ":" + minuteString + ":" + secondString);
        lcd.setCursor(0, 1);
        lcd.print("Date:" + monthString + "/" + dayString + "/" + String(year));
      break;
    case SET_TIME:
      // todo set time menu
      break;
    case SET_DATE:
      // todo set date menu
      break;
  }

  delay(1000);
  updateTime();
  lcd.clear();
}
