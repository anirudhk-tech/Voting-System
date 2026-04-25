/*
 * Adaptive Station - Wiring Test with Low-Light Indicator
 * Display shows "8" by default.
 * Press Button A -> display blank.
 * Press Button B -> display shows "-".
 * Decimal point lights up when ambient light is below threshold.
 */

// ===== PIN ASSIGNMENTS =====
const int SEG_A = 8;
const int SEG_B = 5;
const int SEG_C = 4;
const int SEG_D = 3;
const int SEG_E = 6;
const int SEG_F = 7;
const int SEG_G = 9;
const int SEG_DP = 2;   // decimal point (NEW - wire Arduino 10 -> 220Ω -> display dp pin)

const int BTN_A = 11;
const int BTN_B = 12;
const int PHOTO_PIN = A0;

const int LIGHT_THRESHOLD = 800;  // below this = "dark" — tune as needed
// ============================

const int segPins[7] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G};

const byte patternAllOn[7]  = {1,1,1,1,1,1,1};  // "8"
const byte patternAllOff[7] = {0,0,0,0,0,0,0};  // blank
const byte patternDash[7]   = {0,0,0,0,0,0,1};  // "-"

void showPattern(const byte pattern[7]) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(segPins[i], pattern[i] ? HIGH : LOW);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println(F("=== Wiring Test w/ Low-Light Indicator ==="));

  for (int i = 0; i < 7; i++) {
    pinMode(segPins[i], OUTPUT);
  }
  pinMode(SEG_DP, OUTPUT);

  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);

  showPattern(patternAllOn);
}

void loop() {
  int btnAState = digitalRead(BTN_A);
  int btnBState = digitalRead(BTN_B);
  int lightVal  = analogRead(PHOTO_PIN);

  // Pattern selection
  if (btnAState == LOW) {
    showPattern(patternAllOff);
  } else if (btnBState == LOW) {
    showPattern(patternDash);
  } else {
    showPattern(patternAllOn);
  }

  // Decimal point = low-light indicator
  bool isDark = (lightVal < LIGHT_THRESHOLD);
  digitalWrite(SEG_DP, isDark ? HIGH : LOW);

  // Serial diagnostics
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    lastPrint = millis();
    Serial.print(F("Light=")); Serial.print(lightVal);
    Serial.print(F(" Dark=")); Serial.print(isDark ? "YES" : "NO ");
    Serial.print(F(" BtnA=")); Serial.print(btnAState);
    Serial.print(F(" BtnB=")); Serial.println(btnBState);
  }

  delay(20);
}
