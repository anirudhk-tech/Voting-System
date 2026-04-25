/*
 * Station B - Component Test
 * Tests: 2 pushbuttons, green status LED, piezo buzzer, photoresistor
 */

const int BTN_A     = 2;
const int BTN_B     = 3;
const int STATUS_LED = 4;
const int BUZZER    = 5;
const int PHOTO_PIN = A0;

int lastBtnA = HIGH;
int lastBtnB = HIGH;

bool stationActive = true;
unsigned long lockUntil = 0;

void setup() {
  Serial.begin(9600);
  Serial.println(F("=== Station B Test ==="));

  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(STATUS_LED, HIGH);
}

void loop() {
  int btnAState = digitalRead(BTN_A);
  int btnBState = digitalRead(BTN_B);
  int lightVal  = analogRead(PHOTO_PIN);

  if (stationActive) {
    if (btnAState == LOW && lastBtnA == HIGH) {
      Serial.println(F(">>> Button A"));
      tone(BUZZER, 1000, 150);
      stationActive = false;
      digitalWrite(STATUS_LED, LOW);
      lockUntil = millis() + 3000;
    }
    if (btnBState == LOW && lastBtnB == HIGH) {
      Serial.println(F(">>> Button B"));
      tone(BUZZER, 1500, 150);
      stationActive = false;
      digitalWrite(STATUS_LED, LOW);
      lockUntil = millis() + 3000;
    }
  }
  lastBtnA = btnAState;
  lastBtnB = btnBState;

  if (!stationActive && millis() > lockUntil) {
    stationActive = true;
    digitalWrite(STATUS_LED, HIGH);
    Serial.println(F(">>> Reset"));
  }

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    lastPrint = millis();
    Serial.print(F("Light=")); Serial.print(lightVal);
    Serial.print(F(" Active=")); Serial.println(stationActive ? "YES" : "NO");
  }

  delay(20);
}

