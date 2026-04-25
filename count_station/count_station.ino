/*
 * Station D - Component Test
 * Pins: 7-seg a=7,b=6,c=3,d=4,e=5,f=8,g=9,dp=2
 *       Buttons: A=11, B=12
 *       Photoresistor: A0
 */

const int SEG_A = 7;
const int SEG_B = 6;
const int SEG_C = 3;
const int SEG_D = 4;
const int SEG_E = 5;
const int SEG_F = 8;
const int SEG_G = 9;
const int SEG_DP = 2;

const int BTN_A = 11;
const int BTN_B = 12;
const int PHOTO_PIN = A0;

const int segPins[7] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G};

const byte digitPatterns[10][7] = {
  {1,1,1,1,1,1,0}, {0,1,1,0,0,0,0}, {1,1,0,1,1,0,1}, {1,1,1,1,0,0,1},
  {0,1,1,0,0,1,1}, {1,0,1,1,0,1,1}, {1,0,1,1,1,1,1}, {1,1,1,0,0,0,0},
  {1,1,1,1,1,1,1}, {1,1,1,1,0,1,1}
};

const byte patternA[7]   = {1,1,1,0,1,1,1};
const byte patternB[7]   = {0,0,1,1,1,1,1};
const byte patternBlank[7] = {0,0,0,0,0,0,0};

int lastBtnA = HIGH;
int lastBtnB = HIGH;
int currentDigit = 0;
unsigned long lastDigitChange = 0;

unsigned long letterShowUntil = 0;
char letterToShow = 0;

void showPattern(const byte pattern[7]) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(segPins[i], pattern[i] ? HIGH : LOW);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println(F("=== Station D Test ==="));
  for (int i = 0; i < 7; i++) pinMode(segPins[i], OUTPUT);
  pinMode(SEG_DP, OUTPUT);
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
}

void loop() {
  int btnAState = digitalRead(BTN_A);
  int btnBState = digitalRead(BTN_B);
  int lightVal  = analogRead(PHOTO_PIN);

  if (btnAState == LOW && lastBtnA == HIGH) {
    Serial.println(F(">>> Button A"));
    letterToShow = 'A';
    letterShowUntil = millis() + 800;
  }
  lastBtnA = btnAState;

  if (btnBState == LOW && lastBtnB == HIGH) {
    Serial.println(F(">>> Button B"));
    letterToShow = 'b';
    letterShowUntil = millis() + 800;
  }
  lastBtnB = btnBState;

  if (millis() < letterShowUntil) {
    if (letterToShow == 'A') showPattern(patternA);
    else showPattern(patternB);
  } else {
    if (millis() - lastDigitChange > 1000) {
      lastDigitChange = millis();
      currentDigit = (currentDigit + 1) % 10;
      showPattern(digitPatterns[currentDigit]);
    }
  }

  digitalWrite(SEG_DP, lightVal < 400 ? HIGH : LOW);

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    lastPrint = millis();
    Serial.print(F("Light=")); Serial.print(lightVal);
    Serial.print(F(" BtnA=")); Serial.print(btnAState);
    Serial.print(F(" BtnB=")); Serial.println(btnBState);
  }

  delay(20);
}
