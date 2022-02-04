#include <Arduino.h>
#include <LiquidCrystal.h>

enum State { RECORD_TIME, RECORD_DATE, DISPLAY_DATETIME, MENU };

State state = RECORD_TIME;

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

void loop() {

  // int potentiometerInput = analogRead(potentiometerPin);
  // float inputVoltage = inputToVoltage(potentiometerInput);
  // Serial.print("Potentiometer Voltage: ");
  // Serial.println(inputVoltage);

  lcd.setCursor(0, 0);
  lcd.print("Hello World!!!");

  delay(500);
  lcd.clear();
}
