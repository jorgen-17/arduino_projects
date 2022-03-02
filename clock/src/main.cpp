#include <Arduino.h>
#include <LiquidCrystal.h>

enum State { MENU, SET_TIME, SET_DATE };
enum AMPM { AM, PM };

State state = MENU;
AMPM ampm = PM;

int ticks = 0;
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
unsigned long lastButtonPress = 0;

enum DIRECTION { CW, CCW, NONE };

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

int updateValueWithinLimitsInclusive (DIRECTION direction, int value, int lowerLimit, int upperLimit) {
  if (direction == CW) {
    value++;
    return value > upperLimit ? upperLimit : value;
  } else if (direction == CCW) {
    value--;
    return value < lowerLimit ? lowerLimit : value;
  }
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
  if (day >= 32) {
    month++;
    day = 1;
  }
  if (month >= 13) {
    year++;
    month = 1;
  }
}

DIRECTION readEncoder() {
  DIRECTION direction = NONE;
  currentStateCLK = digitalRead(CLK);

  if(currentStateCLK != previousStateCLK) {
    if (digitalRead(DT) != currentStateCLK) {
      counter--;
      direction = CCW;
    } else {
      counter++;
      direction = CW;
    }
  }

  Serial.print("Direction: ");
  Serial.print(direction);
  Serial.print(" -- Value: ");
  Serial.println(counter);

  previousStateCLK = currentStateCLK;

  return direction;
}

bool buttonClicked() {
  if (digitalRead(SW) == LOW) {
    //if 50ms have passed since last LOW pulse, it means that the
    //button has been pressed, released and pressed again
    if (millis() - lastButtonPress > 50) {
      Serial.println("Button pressed!");
      return true;
    }

    // Remember last button press event
    lastButtonPress = millis();

    return false;
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
  DIRECTION rotationDirection = readEncoder();
  bool buttonRead = buttonClicked();

  menuCursor = updateValueWithinLimitsInclusive(rotationDirection, menuCursor, 0, 1);
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
        if (buttonRead) {
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
        Serial.print("buttonRead inside set time: ");
        Serial.println(buttonRead);

        if (timeCursor == 0) {
          hour = updateValueWithinLimitsInclusive(rotationDirection, hour, 0, 12);
        } else if (timeCursor == 1) {
          minute = updateValueWithinLimitsInclusive(rotationDirection, minute, 0, 60);
        } else if (timeCursor == 2) {
          second = updateValueWithinLimitsInclusive(rotationDirection, second, 0, 60);
        } else if (timeCursor == 3) {
          int newAMPM = updateValueWithinLimitsInclusive(rotationDirection, ampm, 0, 1);
          ampm = newAMPM == 0 ? AM : PM;
        }

        if (buttonRead) {
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
        Serial.print("buttonRead inside set date: ");
        Serial.println(buttonRead);

        if (dateCursor == 0) {
          month = updateValueWithinLimitsInclusive(rotationDirection, month, 0, 12);
        } else if (dateCursor == 1) {
          day = updateValueWithinLimitsInclusive(rotationDirection, day, 0, 31);
        } else if (dateCursor == 2) {
          year = updateValueWithinLimitsInclusive(rotationDirection, year, 2001, 2100);
        }

        if (buttonRead) {
          dateCursor++;
          if (dateCursor >= 3) {
            dateCursor = 0;
            state = MENU;
          }
        }
      break;
  }

  Serial.println("");
  Serial.println("");
  Serial.println("");

  delay(100);
  ticks++;
  if (ticks >= 10 && state != SET_TIME) { //update time only once per second
    ticks = 0;
    updateTime();
  }
  lcd.clear();
}
