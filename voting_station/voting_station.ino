/*
 * Station B - Hardware Serial Test
 * Sends "HELLO_FROM_B" once per second.
 * Beeps the buzzer when it receives anything from master.
 */

const int BUZZER = 5;

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER, OUTPUT);
}

void loop() {
  // Send heartbeat every second
  static unsigned long last = 0;
  if (millis() - last > 1000) {
    last = millis();
    Serial.println("HELLO_FROM_B");
  }

  // Beep when receiving from master
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') return;  // skip newlines
    tone(BUZZER, 1500, 100);
    delay(120);
  }
}
