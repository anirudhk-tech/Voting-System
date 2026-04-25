/*
 * Station B - Audio-Visual Voter Test Sketch
 * Tests: 2 pushbuttons (INPUT_PULLUP), green status LED, piezo buzzer, photoresistor
 */

// ===== PIN ASSIGNMENTS =====
const int BTN_A     = 2;
const int BTN_B     = 3;
const int STATUS_LED = 4;
const int BUZZER    = 5;
const int PHOTO_PIN = A0;
// ============================

int lastBtnA = HIGH;
int lastBtnB = HIGH;

bool stationActive = true;
unsigned long lockUntil = 0;

void playBeepA() {
  tone(BUZZER, 1000, 150);  // 1000Hz for 150ms
}

void playBeepB() {
  tone(BUZZER, 1500, 150);  // 1500Hz for 150ms
}

void setup() {
  Serial.begin(9600);
  Serial.println(F("=== Station B Test ==="));

  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(STATUS_LED, HIGH);  // start active
}

void loop() {
  int btnAState = digitalRead(BTN_A);
  int btnBState = digitalRead(BTN_B);
  int lightVal  = analogRead(PHOTO_PIN);

  if (stationActive) {
    if (btnAState == LOW && lastBtnA == HIGH) {
      Serial.println(F(">>> Button A pressed - vote A"));
      playBeepA();
      stationActive = false;
      digitalWrite(STATUS_LED, LOW);
      lockUntil = millis() + 3000;
    }
    if (btnBState == LOW && lastBtnB == HIGH) {
      Serial.println(F(">>> Button B pressed - vote B"));
      playBeepB();
      stationActive = false;
      digitalWrite(STATUS_LED, LOW);
      lockUntil = millis() + 3000;
    }
  }
  lastBtnA = btnAState;
  lastBtnB = btnBState;

  // Auto-unlock after 3s
  if (!stationActive && millis() > lockUntil) {
    stationActive = true;
    digitalWrite(STATUS_LED, HIGH);
    Serial.println(F(">>> Station reset - active again"));
  }

  // Serial diagnostics
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    lastPrint = millis();
    Serial.print(F("Light=")); Serial.print(lightVal);
    Serial.print(F(" BtnA=")); Serial.print(btnAState);
    Serial.print(F(" BtnB=")); Serial.print(btnBState);
    Serial.print(F(" Active=")); Serial.println(stationActive ? "YES" : "NO");
  }

  delay(20);
}
