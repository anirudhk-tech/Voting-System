/*
 * Station A - Standard Voter (LED bar graph)
 * Pins: BtnA=8, BtnB=9, LEDs=5,4,3,2, Photo=A0
 * Hardware Serial (pin 0/1) talks to master.
 */

const int BTN_A = 8;
const int BTN_B = 9;
const int LED_PINS[4] = {5, 4, 3, 2};
const int PHOTO_PIN = A0;

const char STATION_ID = 'A';

bool stationActive = true;
int lastBtnA = HIGH;
int lastBtnB = HIGH;

int displayedA = 0;
int displayedB = 0;
char displayedMode = 'L';
bool resultsRevealed = true;

void setBar(int level) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(LED_PINS[i], i < level ? HIGH : LOW);
  }
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

void setup() {
  Serial.begin(9600);
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  for (int i = 0; i < 4; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
  setBar(0);
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

  if (resultsRevealed) {
    int level = max(displayedA, displayedB);
    if (level > 4) level = 4;
    setBar(level);
  } else {
    bool on = (millis() / 400) % 2;
    for (int i = 0; i < 4; i++) digitalWrite(LED_PINS[i], on ? HIGH : LOW);
  }

  delay(20);
}
