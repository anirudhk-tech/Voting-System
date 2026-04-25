const int LED = 13;

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    digitalWrite(LED, HIGH);
    delay(50);
    digitalWrite(LED, LOW);
  }

  static unsigned long last = 0;
  if (millis() - last > 2000) {
    last = millis();
    Serial.println("PING");
  }
}
