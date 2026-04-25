/*
 * Station A - Component Test
 * Tests: 2 pushbuttons (INPUT_PULLUP), 4 LEDs (bar graph), photoresistor
 *
 * Pins: LEDs=5,4,3,2 | Buttons=8,9 | Photo=A0
 *
 * Behavior:
 *  - LEDs cycle: 1 lit -> 2 lit -> 3 lit -> 4 lit -> all -> none, every 500ms
 *  - Press Button A: all LEDs solid for ~800ms
 *  - Press Button B: alternating flash for ~800ms
 *  - Photoresistor reading prints to Serial
 */

const int LED_1 = 5;
const int LED_2 = 4;
const int LED_3 = 3;
const int LED_4 = 2;

const int BTN_A = 8;
const int BTN_B = 9;

const int PHOTO_PIN = A0;

const int ledPins[4] = {LED_1, LED_2, LED_3, LED_4};

int lastBtnA = HIGH;
int lastBtnB = HIGH;

int currentLevel = 0;
unsigned long lastLevelChange = 0;
const unsigned long LEVEL_INTERVAL = 500;

unsigned long specialShowUntil = 0;
char specialPattern = 0;

void setBar(int level) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], i < level ? HIGH : LOW);
  }
}

void setAlternating(bool flip) {
  for (int i = 0; i < 4; i++) {
    bool on = (i % 2 == 0) ? !flip : flip;
    digitalWrite(ledPins[i], on ? HIGH : LOW);
  }
}

void setAllOn() {
  for (int i = 0; i < 4; i++) digitalWrite(ledPins[i], HIGH);
}

void setup() {
  Serial.begin(9600);
  Serial.println(F("=== Station A Test ==="));

  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
}

void loop() {
  int btnAState = digitalRead(BTN_A);
  int btnBState = digitalRead(BTN_B);
  int lightVal  = analogRead(PHOTO_PIN);

  if (btnAState == LOW && lastBtnA == HIGH) {
    Serial.println(F(">>> Button A pressed"));
    specialPattern = 'A';
    specialShowUntil = millis() + 800;
  }
  lastBtnA = btnAState;

  if (btnBState == LOW && lastBtnB == HIGH) {
    Serial.println(F(">>> Button B pressed"));
    specialPattern = 'B';
    specialShowUntil = millis() + 800;
  }
  lastBtnB = btnBState;

  if (millis() < specialShowUntil) {
    if (specialPattern == 'A') {
      setAllOn();
    } else if (specialPattern == 'B') {
      bool flip = (millis() / 150) % 2;
      setAlternating(flip);
    }
  } else {
    if (millis() - lastLevelChange > LEVEL_INTERVAL) {
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
    Serial.print(F(" BtnB=")); Serial.print(btnBState);
    Serial.print(F(" Level=")); Serial.println(currentLevel);
  }

  delay(20);
}
