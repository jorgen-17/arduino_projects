#include <Arduino.h>

// Rotary Encoder Inputs
#define CLK 5
#define DT 4
#define SW 3

// LED Outputs
#define ledCW 8
#define ledCCW 9

int counter = 0;
int currentStateCLK;
int previousStateCLK;
unsigned long lastButtonPress = 0;

enum DIRECTION { CW, CCW };

DIRECTION direction;

void setup() {
  // Set encoder pins as inputs
  pinMode (CLK, INPUT);
  pinMode (DT, INPUT);
  pinMode (SW, INPUT);

  // Set LED pins as outputs
  pinMode (ledCW, OUTPUT);
  pinMode (ledCCW, OUTPUT);

  // Setup Serial Monitor
  Serial.begin(9600);

  // Read the initial state of CLK
  // Assign to previousStateCLK variable
  previousStateCLK = digitalRead(CLK);
}

void loop() {
  // Read the current state of CLK
  currentStateCLK = digitalRead(CLK);

  // If the previous and the current state of the CLK are different then a pulse has occured
  if (currentStateCLK != previousStateCLK){

    // If the DT state is different than the CLK state then
    // the encoder is rotating counterclockwise
    if (digitalRead(DT) != currentStateCLK) {
      counter--;
      direction = CCW;
      digitalWrite(ledCW, LOW);
      digitalWrite(ledCCW, HIGH);

    } else {
      // Encoder is rotating clockwise
      counter++;
      direction = CW;
      digitalWrite(ledCW, HIGH);
      digitalWrite(ledCCW, LOW);

    }

    Serial.print("Direction: ");
    Serial.print(direction);
    Serial.print(" -- Value: ");
    Serial.println(counter);
  }
  // Update previousStateCLK with the current state
  previousStateCLK = currentStateCLK;
  if (digitalRead(SW) == LOW) {
    //if 50ms have passed since last LOW pulse, it means that the
    //button has been pressed, released and pressed again
    if (millis() - lastButtonPress > 50) {
      Serial.println("Button pressed!");
      digitalWrite(ledCW, HIGH);
      digitalWrite(ledCCW, HIGH);
    }

    // Remember last button press event
    lastButtonPress = millis();
  }
  // Put in a slight delay to help debounce the reading
  delay(1);
}