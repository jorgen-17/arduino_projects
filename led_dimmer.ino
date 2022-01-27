void setup() {
  Serial.begin(9600);
  pinMode(3, OUTPUT);
}

void loop() {
  int potentiometerInput = analogRead(A0);
  float inputVoltage = inputToVoltage(potentiometerInput);
  Serial.print("Potentiometer Voltage: ");
  Serial.println(inputVoltage);

  int brightness = voltageToBrightness(inputVoltage);
  Serial.print("Bulb Brightness: ");
  Serial.println(brightness);
  analogWrite(3, brightness);
}

float inputToVoltage (int input) {
  return input * (5.0 / 1023.0);
}

int voltageToBrightness(float voltage) {
  return (voltage / 5) * 255;
}