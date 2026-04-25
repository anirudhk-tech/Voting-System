const int BTN_A = 2;
const int BTN_B = 3;
const int STATUS_LED = 4;
const int BUZZER = 5;
const char STATION_ID = 'B';

bool stationActive = true;
int lastBtnA = HIGH;
int lastBtnB = HIGH;

void sendVote(char choice) {
  Serial.print("V");
  Serial.print(STATION_ID);
  Serial.print(":");
  Serial.println(choice);
}

void parseIncoming(const String& msg) {
  if (msg.length() == 0) return;
  if (msg.charAt(0) == 'R') {
    stationActive = true;
    digitalWrite(STATUS_LED, HIGH);
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
      sendVote('A');
      tone(BUZZER, 1000, 150);
      stationActive = false;
      digitalWrite(STATUS_LED, LOW);
    }
    if (btnBState == LOW && lastBtnB == HIGH) {
      sendVote('B');
      tone(BUZZER, 1500, 150);
      stationActive = false;
      digitalWrite(STATUS_LED, LOW);
    }
  }
  lastBtnA = btnAState;
  lastBtnB = btnBState;

  delay(20);
}
