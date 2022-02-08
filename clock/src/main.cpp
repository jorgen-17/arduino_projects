#include <Arduino.h>
#include <LiquidCrystal.h>

enum State { MENU, SET_TIME, SET_DATE };
enum AMPM { AM, PM };

State state = MENU;
AMPM ampm = PM;

int ticks = 0;

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

int inputToIntervals (int input, int numberOfIntervals) {
  return (input  / 1023.0) * numberOfIntervals;
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
    if (hour == 12) {
      switch (ampm)
      {
        case AM:
          ampm = PM;
          break;
        case PM:
          ampm = AM;
          day++;
          break;
      }
    }
  }
  if (hour >= 13) {
    hour = 1;
  }
  if(day >= 32) {
    month++;
    day = 1;
  }
  if(month >= 13) {
    year++;
    month = 1;
  }
}

String ampmToString(AMPM ampm) {
  switch (ampm) {
    case AM:
      return "AM";
      break;
    case PM:
      return "PM";
      break;
  }
}



void loop() {
  int potentiometerInput = analogRead(potentiometerPin);
  int input = inputToIntervals(potentiometerInput, 10);
  int buttonRead = digitalRead(buttonPin);

  int menuCursor = input % 2 == 0 ? 0 : 1;
  String line1Cursor = menuCursor == 0 ? ">" : "";
  String line2Cursor = menuCursor == 1 ? ">" : "";
  String hourString = hour >= 10 ? String(hour) : "0" + String(hour);
  String minuteString = minute >= 10 ? String(minute) : "0" + String(minute);
  String secondString = second >= 10 ? String(second) : "0" + String(second);
  String monthString = month >= 10 ? String(month) : "0" + String(month);
  String dayString = day >= 10 ? String(day) : "0" + String(day);

  switch (state) {
    case MENU:
        lcd.setCursor(0, 0);
        lcd.print(line1Cursor + "Time:" + hourString + ":" + minuteString + ":" + secondString + ampmToString(ampm));
        lcd.setCursor(0, 1);
        lcd.print(line2Cursor + "Date:" + monthString + "/" + dayString + "/" + String(year));
        if(buttonRead) {
          if(menuCursor == 0) {
            state = SET_TIME;
          }
          else if (menuCursor == 1) {
            state = SET_DATE;
          }
        }
      break;
    case SET_TIME:
        lcd.setCursor(0, 0);
        lcd.print("Please set time:");
        lcd.setCursor(0, 1);
        lcd.print("Time:" + hourString + ":" + minuteString + ":" + secondString + ampmToString(ampm));
        if(buttonRead) {
          state = MENU;
        }
      break;
    case SET_DATE:
        lcd.setCursor(0, 0);
        lcd.print("Please set date:");
        lcd.setCursor(0, 1);
        lcd.print("Date:" + monthString + "/" + dayString + "/" + String(year));
        if(buttonRead) {
          state = MENU;
        }
        break;
  }

  delay(100);
  ticks++;
  if(ticks >= 10) { //update time only once per second
    ticks = 0;
    updateTime();
  }
  lcd.clear();
}
