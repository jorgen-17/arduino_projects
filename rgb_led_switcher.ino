enum Color { RED, GREEN, BLUE };

int potentiometerPin = A0;
int buttonInputPin = A1;
int buttonOutputPin = 4;
int redPin = 3;
int greenPin = 6;
int bluePin = 5;
int colorIndex = 0;
Color colors[] { RED, GREEN, BLUE };
int colorsLength = sizeof(colors)/sizeof(colors[0]);
int brightness = 255;

void setup() {
  pinMode(potentiometerPin, INPUT);
  pinMode(buttonInputPin, INPUT);

  pinMode(buttonOutputPin, OUTPUT);
  digitalWrite(buttonOutputPin, HIGH);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  int potentiometerInput = analogRead(potentiometerPin);
  Serial.print("potentiometer: ");
  Serial.println(potentiometerInput);
  brightness = inputToOutput(potentiometerInput);
  Serial.print("Brightness: ");
  Serial.println(brightness);
  
  int buttonRead = digitalRead(buttonInputPin);
  if (buttonRead){
    if(colorIndex >= (colorsLength - 1)){
      colorIndex = 0;
    } else {
      colorIndex++;
    }
  }
  Serial.print("colorIndex after: ");
  Serial.println(colorIndex);
  
  switch (colors[colorIndex]) {
    case RED:
      analogWrite(redPin, brightness);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 0);
      break;
    case GREEN:
      analogWrite(redPin, 0);
      analogWrite(greenPin, brightness);
      analogWrite(bluePin, 0);
      break;
    case BLUE:
      analogWrite(redPin, 0);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, brightness);
      break;
  }
  
  delay(100);
}

int inputToOutput(int input) {
  return (int) ((input / 1023.0) * 255.0);
}
