#define BTN_A       2
#define BTN_B       3
#define STATUS_LED  4
#define BUZZER      5

char ID = 'B';

int voted = 0;
int prevA = HIGH;
int prevB = HIGH;

int countA = 0;
int countB = 0;
char mode = 'L';

void sendVote(char choice) {
  for (int i = 0; i < 8; i++) {
    Serial.print("V");
    Serial.print(ID);
    Serial.print(":");
    Serial.println(choice);
    delay(30);
  }
}

void handleMsg(String msg) {
  if (msg.length() == 0) return;

  if (msg[0] == 'R') {
    voted = 0;
    digitalWrite(STATUS_LED, HIGH);
    countA = 0;
    countB = 0;
    return;
  }

  if (msg[0] == 'T') {
    int c1 = msg.indexOf(':');
    int c2 = msg.indexOf(',');
    int c3 = msg.indexOf(',', c2 + 1);
    if (c1 < 0 || c2 < 0 || c3 < 0) return;
    countA = msg.substring(c1 + 1, c2).toInt();
    countB = msg.substring(c2 + 1, c3).toInt();
    mode = msg[c3 + 1];
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
    handleMsg(msg);
  }

  int a = digitalRead(BTN_A);
  int b = digitalRead(BTN_B);

  if (!voted) {
    if (a == LOW && prevA == HIGH) {
      tone(BUZZER, 1000, 150);
      voted = 1;
      digitalWrite(STATUS_LED, LOW);
      sendVote('A');
    }
    if (b == LOW && prevB == HIGH) {
      tone(BUZZER, 1500, 150);
      voted = 1;
      digitalWrite(STATUS_LED, LOW);
      sendVote('B');
    }
  }

  prevA = a;
  prevB = b;
  delay(20);
}
