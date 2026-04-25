/*
 * Master - Hardware Serial Test
 * Echoes anything received from Station B by blinking the onboard LED (pin 13).
 * Also sends "PING" to Station B every 2 seconds.
 */

const int LED = 13;  // built-in LED

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
}

void loop() {
  // Blink LED briefly when data is received
  if (Serial.available()) {
    char c = Serial.read();
    digitalWrite(LED, HIGH);
    delay(50);
    digitalWrite(LED, LOW);
  }

  // Send a ping every 2 seconds
  static unsigned long last = 0;
  if (millis() - last > 2000) {
    last = millis();
    Serial.println("PING");
  }
}
