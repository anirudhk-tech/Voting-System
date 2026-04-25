// Station A - LED bar graph voter
// buttons: 8, 9   LEDs: 5 4 3 2

const int BTN_A = 8;
const int BTN_B = 9;
const int LEDS[4] = {5, 4, 3, 2};
const char STATION_ID = 'A';

boolean active = true;
int prevBtnA = HIGH;
int prevBtnB = HIGH;

int scoreA = 0;
int scoreB = 0;
char currentMode = 'L';
boolean showResults = true;

void setBar(int n) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(LEDS[i], (i < n) ? HIGH : LOW);
  }
}

void sendVote(char v) {
  for (int i = 0; i < 8; i++) {
    Serial.print("V");
    Serial.print(STATION_ID);
    Serial.print(":");
    Serial.println(v);
    delay(30);
  }
}

void parseMsg(const String& msg) {
  if (msg.length() == 0) return;

  if (msg.charAt(0) == 'R') {
    active = true;
    scoreA = 0;
    scoreB = 0;
    showResults = true;
    return;
  }

  if (msg.charAt(0) == 'T') {
    int p1 = msg.indexOf(':');
    int p2 = msg.indexOf(',');
    int p3 = msg.indexOf(',', p2 + 1);
    if (p1 < 0 || p2 < 0 || p3 < 0) return;
    scoreA = msg.substring(p1 + 1, p2).toInt();
    scoreB = msg.substring(p2 + 1, p3).toInt();
    currentMode = msg.charAt(p3 + 1);
    showResults = true;
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);

  for (int i = 0; i < 4; i++) {
    pinMode(LEDS[i], OUTPUT);
  }
  setBar(0);
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    parseMsg(line);
  }

  int btnA = digitalRead(BTN_A);
  int btnB = digitalRead(BTN_B);

  if (active) {
    if (btnA == LOW && prevBtnA == HIGH) {
      active = false;
      if (currentMode == 'S') showResults = false;
      sendVote('A');
    }
    if (btnB == LOW && prevBtnB == HIGH) {
      active = false;
      if (currentMode == 'S') showResults = false;
      sendVote('B');
    }
  }

  prevBtnA = btnA;
  prevBtnB = btnB;

  if (showResults) {
    int bars = max(scoreA, scoreB);
    if (bars > 4) bars = 4;
    setBar(bars);
  } else {
    boolean lit = (millis() / 400) % 2;
    for (int i = 0; i < 4; i++) {
      digitalWrite(LEDS[i], lit ? HIGH : LOW);
    }
  }

  delay(20);
}
