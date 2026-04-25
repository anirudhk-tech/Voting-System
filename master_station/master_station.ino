const int LED_YELLOW = 2;
const int LED_GREEN  = 3;
const int LED_RED    = 4;

void setup() {
  Serial.begin(9600);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN,  OUTPUT);
  pinMode(LED_RED,    OUTPUT);
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    // Light all three LEDs briefly on receive
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_GREEN,  HIGH);
    digitalWrite(LED_RED,    HIGH);
    delay(100);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_GREEN,  LOW);
    digitalWrite(LED_RED,    LOW);
  }

  static unsigned long last = 0;
  if (millis() - last > 2000) {
    last = millis();
    Serial.println("PING");
  }
}
