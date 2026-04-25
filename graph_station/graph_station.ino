/*
 * Station A - Component Test
 * Pins: LEDs=5,4,3,2 | Buttons=8,9 | Photo=A0
 */

const int LED_PINS[4] = {5, 4, 3, 2};
const int BTN_A = 8;
const int BTN_B = 9;
const int PHOTO_PIN = A0;

int lastBtnA = HIGH;
int lastBtnB = HIGH;

int currentLevel = 0;
unsigned long lastLevelChange = 0;

unsigned long specialShowUntil = 0;
char specialPattern = 0;

void setBar(int level) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(LED_PINS[i], i < level ? HIGH : LOW);
  }
}

void setAlternating(bool flip) {
  for (int i = 0; i < 4; i++) {
    bool on = (i % 2 == 0) ? !flip : flip;
    digitalWrite(LED_PINS[i], on ? HIGH : LOW);
  }
}

void setAllOn() {
  for (int i = 0; i < 4; i++) digitalWrite(LED_PINS[i], HIGH);
}

void setup() {
  Serial.begin(9600);
  Serial.println(F("=== Station A Test ==="));
  for (int i = 0; i < 4; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
}

void loop() {
  int btnAState = digitalRead(BTN_A);
  int btnBState = digitalRead(BTN_B);
  int lightVal  = analogRead(PHOTO_PIN);

  if (btnAState == LOW && lastBtnA == HIGH) {
    Serial.println(F(">>> Button A"));
    specialPattern = 'A';
    specialShowUntil = millis() + 800;
  }
  lastBtnA = btnAState;

  if (btnBState == LOW && lastBtnB == HIGH) {
    Serial.println(F(">>> Button B"));
    specialPattern = 'B';
    specialShowUntil = millis() + 800;
  }
  lastBtnB = btnBState;

  if (millis() < specialShowUntil) {
    if (specialPattern == 'A') setAllOn();
    else {
      bool flip = (millis() / 150) % 2;
      setAlternating(flip);
    }
  } else {
    if (millis() - lastLevelChange > 500) {
      lastLevelChange = millis();
      currentLevel = (currentLevel + 1) % 6;
    }
    if (currentLevel == 5) setAllOn();
    else setBar(currentLevel);
  }

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    lastPrint = millis();
    Serial.print(F("Light=")); Serial.print(lightVal);
    Serial.print(F(" BtnA=")); Serial.print(btnAState);
    Serial.print(F(" BtnB=")); Serial.println(btnBState);
  }

  delay(20);
}
