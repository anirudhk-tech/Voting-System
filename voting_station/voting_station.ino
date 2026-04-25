const int BUZZER = 5;

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER, OUTPUT);
}

void loop() {
  static unsigned long last = 0;
  if (millis() - last > 1000) {
    last = millis();
    Serial.println("HELLO_FROM_B");
  }

  if (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') return;
    tone(BUZZER, 1500, 100);
    delay(120);
  }
}
