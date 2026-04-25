/*
 * Station D - Adaptive Voter (7-segment numeric display)
 * Pins: 7-seg a=8,b=5,c=4,d=3,e=2,f=7,g=9,dp=6
 *       Buttons: A=11, B=12
 *       Photoresistor: A0
 * Hardware Serial (pin 0/1) talks to master.
 */

const int SEG_A = 8;
const int SEG_B = 5;
const int SEG_C = 4;
const int SEG_D = 3;
const int SEG_E = 2;
const int SEG_F = 7;
const int SEG_G = 9;
const int SEG_DP = 6;

const int BTN_A = 11;
const int BTN_B = 12;
const int PHOTO_PIN = A0;

const int LIGHT_THRESHOLD = 400;

const char STATION_ID = 'D';

const int segPins[7] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G};

const byte digitPatterns[10][7] = {
  {1,1,1,1,1,1,0},
  {0,1,1,0,0,0,0},
  {1,1,0,1,1,0,1},
  {1,1,1,1,0,0,1},
  {0,1,1,0,0,1,1},
  {1,0,1,1,0,1,1},
  {1,0,1,1,1,1,1},
  {1,1,1,0,0,0,0},
  {1,1,1,1,1,1,1},
  {1,1,1,1,0,1,1}
};

const byte patternBlank[7] = {0,0,0,0,0,0,0};
const byte patternDash[7]  = {0,0,0,0,0,0,1};

bool stationActive = true;
int lastBtnA = HIGH;
int lastBtnB = HIGH;

int displayedA = 0;
int displayedB = 0;
char displayedMode = 'L';
bool resultsRevealed = true;

unsigned long lastFlashToggle = 0;
bool flashState = false;

void showPattern(const byte pattern[7]) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(segPins[i], pattern[i] ? HIGH : LOW);
  }
}

void showDigit(int d) {
  if (d < 0) { showPattern(patternBlank); return; }
  if (d > 9) d = 9;
  showPattern(digitPatterns[d]);
}

void sendVote(char choice) {
  for (int i = 0; i < 3; i++) {
    Serial.print("V");
    Serial.print(STATION_ID);
    Serial.print(":");
    Serial.println(choice);
    delay(30);
  }
}

void parseIncoming(const String& msg) {
  if (msg.length() == 0) return;

  if (msg.charAt(0) == 'R') {
    stationActive = true;
    displayedA = 0;
    displayedB = 0;
    resultsRevealed = true;
    return;
  }

  if (msg.charAt(0) == 'T') {
    int firstColon  = msg.indexOf(':');
    int firstComma  = msg.indexOf(',');
    int secondComma = msg.indexOf(',', firstComma + 1);
    if (firstColon < 0 || firstComma < 0 || secondComma < 0) return;

    displayedA = msg.substring(firstColon + 1, firstComma).toInt();
    displayedB = msg.substring(firstComma + 1, secondComma).toInt();
    displayedMode = msg.charAt(secondComma + 1);
    resultsRevealed = true;
  }
}

void updateDisplay() {
  int lightVal = analogRead(PHOTO_PIN);
  bool isDark = (lightVal < LIGHT_THRESHOLD);
  digitalWrite(SEG_DP, isDark ? HIGH : LOW);

  if (!stationActive && !resultsRevealed) {
    if (millis() - lastFlashToggle > 400) {
      lastFlashToggle = millis();
      flashState = !flashState;
    }
    showPattern(flashState ? patternDash : patternBlank);
    return;
  }

  int displayValue = max(displayedA, displayedB);
  showDigit(displayValue);
}

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 7; i++) {
    pinMode(segPins[i], OUTPUT);
  }
  pinMode(SEG_DP, OUTPUT);

  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);

  showDigit(0);
}

void loop() {
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();
    parseIncoming(msg);
  }

  int btnAState = digitalRead(BTN_A);
  int btnBState = digitalRead(BTN_B);

  if (stationActive) {
    if (btnAState == LOW && lastBtnA == HIGH) {
      stationActive = false;
      if (displayedMode == 'S') resultsRevealed = false;
      sendVote('A');
    }
    if (btnBState == LOW && lastBtnB == HIGH) {
      stationActive = false;
      if (displayedMode == 'S') resultsRevealed = false;
      sendVote('B');
    }
  }
  lastBtnA = btnAState;
  lastBtnB = btnBState;

  updateDisplay();

  delay(20);
}
