#include <Arduino.h>
#include <LiquidCrystal.h>

enum State { MENU, SET_TIME, SET_DATE };
enum AMPM { AM, PM };
enum DIRECTION { CW, CCW, NONE };

State state = MENU;
AMPM ampm = PM;

int menuCursor = 0;
int timeCursor = 0;
int dateCursor = 0;

int hour = 12;
int minute = 0;
int second = 0;
int year = 2000;
int month = 1;
int day = 1;

int counter = 0;
int currentStateCLK;
int previousStateCLK;
unsigned long lastTimeUpdate = 0;
unsigned long lastButtonPressed = 0;
unsigned long lastDisplay = 0;

bool buttonPressed = false;

#define CLK 5
#define DT 4
#define SW 3

#define RS 7
#define E 8
#define D4 9
#define D5 10
#define D6 11
#define D7 12

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

void setup() {
  pinMode (CLK, INPUT);
  pinMode (DT, INPUT);
  pinMode (SW, INPUT);

  lcd.begin(16, 2);

  Serial.begin(9600);

  previousStateCLK = digitalRead(CLK);
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

int updateValueWithinCircularLimits (int delta, int value, int lowerLimit, int upperLimit) {
  int newValue = value + delta;

  if (delta > 0) {
    return newValue > upperLimit ? lowerLimit : newValue;
  } else if (delta < 0) {
    return newValue < lowerLimit ? upperLimit : newValue;
  }

  return value;
}

void updateTime () {
  Serial.println("updating the time");

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
  if (day >= 32) {
    month++;
    day = 1;
  }
  if (month >= 13) {
    year++;
    month = 1;
  }
}

void displayMenu() {
  menuCursor = updateValueWithinCircularLimits(counter, menuCursor, 0, 1);
  String line1Cursor = menuCursor == 0 ? ">" : "";
  String line2Cursor = menuCursor == 1 ? ">" : "";
  String hourString = hour >= 10 ? String(hour) : "0" + String(hour);
  String minuteString = minute >= 10 ? String(minute) : "0" + String(minute);
  String secondString = second >= 10 ? String(second) : "0" + String(second);
  String monthString = month >= 10 ? String(month) : "0" + String(month);
  String dayString = day >= 10 ? String(day) : "0" + String(day);

  String hourCursor = timeCursor == 0 ? ">" : "";
  String minuteCursor = timeCursor == 1 ? ">" : "";
  String secondCursor = timeCursor == 2 ? ">" : "";
  String ampmCursor = timeCursor == 3 ? ">" : "";

  String monthCursor = dateCursor == 0 ? ">" : "";
  String dayCursor = dateCursor == 1 ? ">" : "";
  String yearCursor = dateCursor == 2 ? ">" : "";

  switch (state) {
    case MENU:
        Serial.println("menu switch case");
        lcd.setCursor(0, 0);
        lcd.print(line1Cursor + "Time:" + hourString + ":" + minuteString + ":" + secondString + ampmToString(ampm));
        lcd.setCursor(0, 1);
        lcd.print(line2Cursor + "Date:" + monthString + "/" + dayString + "/" + String(year));
        if (buttonPressed) {
          if (menuCursor == 0) {
            state = SET_TIME;
          }
          else if (menuCursor == 1) {
            state = SET_DATE;
          }
        }
      break;
    case SET_TIME:
        Serial.println("set time switch case");
        lcd.setCursor(0, 0);
        lcd.print("Please set time:");
        lcd.setCursor(0, 1);
        lcd.print("Time:" +
          hourCursor + hourString + ":" +
          minuteCursor + minuteString + ":" +
          secondCursor + secondString +
          ampmCursor + ampmToString(ampm));

        if (timeCursor == 0) {
          hour = updateValueWithinCircularLimits(counter, hour, 1, 12);
        } else if (timeCursor == 1) {
          minute = updateValueWithinCircularLimits(counter, minute, 0, 60);
        } else if (timeCursor == 2) {
          second = updateValueWithinCircularLimits(counter, second, 0, 60);
        } else if (timeCursor == 3) {
          int newAMPM = updateValueWithinCircularLimits(counter, ampm, 0, 1);
          ampm = newAMPM == 0 ? AM : PM;
        }

        if (buttonPressed) {
          timeCursor++;
          if (timeCursor >= 4) {
            timeCursor = 0;
            state = MENU;
          }
        }
      break;
    case SET_DATE:
        Serial.println("set date switch case");
        lcd.setCursor(0, 0);
        lcd.print("Please set date:");
        lcd.setCursor(0, 1);
        lcd.print("Date:" +
          monthCursor + monthString + "/" +
          dayCursor + dayString + "/" +
          yearCursor + String(year));


        if (dateCursor == 0) {
          month = updateValueWithinCircularLimits(counter, month, 0, 12);
        } else if (dateCursor == 1) {
          day = updateValueWithinCircularLimits(counter, day, 0, 31);
        } else if (dateCursor == 2) {
          year = updateValueWithinCircularLimits(counter, year, 2001, 2100);
        }

        if (buttonPressed) {
          dateCursor++;
          if (dateCursor >= 3) {
            dateCursor = 0;
            state = MENU;
          }
        }
      break;
  }

  //updated menu to reflect input
  buttonPressed = false;
  counter = 0;
}

DIRECTION readEncoder() {
  DIRECTION direction = NONE;
  currentStateCLK = digitalRead(CLK);

  if(currentStateCLK != previousStateCLK) {
    if (digitalRead(DT) != currentStateCLK) {
      counter++;
      direction = CW;
    } else {
      counter--;
      direction = CCW;
    }
  }

  previousStateCLK = currentStateCLK;

  return direction;
}

void loop() {
  readEncoder();

  if (millis() - lastTimeUpdate >= 1000 && state != SET_TIME) { //update time only once per second
    updateTime();
    lastTimeUpdate = millis();
  }

  if (millis() - lastDisplay >= 200) { //refresh display every 200ms
    lcd.clear();
    displayMenu();
    lastDisplay = millis();
  }

  if (millis() - lastButtonPressed >= 300) { //refresh button press every 300ms
    buttonPressed = digitalRead(SW) == LOW;
    lastButtonPressed = millis();
  }

  delay(1);
}
