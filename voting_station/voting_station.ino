/*
 * Station B - Audio-Visual Voter
 * Pins: BtnA=2, BtnB=3, StatusLED=4, Buzzer=5, Photo=A0
 * Hardware Serial (pin 0/1) talks to master.
 */

const int BTN_A = 2;
const int BTN_B = 3;
const int STATUS_LED = 4;
const int BUZZER = 5;
const int PHOTO_PIN = A0;

const char STATION_ID = 'B';

bool stationActive = true;
int lastBtnA = HIGH;
int lastBtnB = HIGH;

int displayedA = 0;
int displayedB = 0;
char displayedMode = 'L';

void sendVote(char choice) {
  // Burst-send to survive master's round-robin listen window
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
    digitalWrite(STATUS_LED, HIGH);
    displayedA = 0;
    displayedB = 0;
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
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(STATUS_LED, HIGH);
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
      tone(BUZZER, 1000, 150);
      stationActive = false;
      digitalWrite(STATUS_LED, LOW);
      sendVote('A');
    }
    if (btnBState == LOW && lastBtnB == HIGH) {
      tone(BUZZER, 1500, 150);
      stationActive = false;
      digitalWrite(STATUS_LED, LOW);
      sendVote('B');
    }
  }
  lastBtnA = btnAState;
  lastBtnB = btnBState;

  delay(20);
}
