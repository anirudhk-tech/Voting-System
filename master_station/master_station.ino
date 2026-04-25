// Voting system master controller
// Station B: A0(rx)/A1(tx)   Station D: A2(rx)/A3(tx)   Station A: A4(rx)/A5(tx)
// debug output on USB serial

#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

LiquidCrystal lcd(5, 6, 7, 8, 9, 10);

SoftwareSerial chanB(A0, A1);
SoftwareSerial chanD(A2, A3);
SoftwareSerial chanA(A4, A5);

const int LED_Y = 2, LED_G = 3, LED_R = 4;
const int BTN_RST = 11, BTN_MOD = 12;

const int N = 3;
const char IDS[N] = {'B', 'D', 'A'};
SoftwareSerial* ch[N] = {&chanB, &chanD, &chanA};

int vA = 0, vB = 0;
bool voted[N] = {false, false, false};
char mode = 'L';
char nextMode = 'L';

int rstLast = HIGH, modLast = HIGH;

unsigned long dispTimer = 0;
unsigned long switchTimer = 0;
int cur = 0;

const unsigned long DISP_MS = 200;
const unsigned long WIN_MS = 30;

int getIdx(char id) {
  for (int i = 0; i < N; i++) {
    if (IDS[i] == id) return i;
  }
  return -1;
}

bool allVoted() {
  for (int i = 0; i < N; i++) if (!voted[i]) return false;
  return true;
}

void sendAll(const String& msg) {
  for (int i = 0; i < N; i++) ch[i]->println(msg);
  Serial.print("TX: "); Serial.println(msg);
}

void pushState() {
  String s = "T:";
  s += vA; s += ","; s += vB; s += ","; s += mode;
  sendAll(s);
}

void doReset() {
  vA = 0; vB = 0;
  for (int i = 0; i < N; i++) voted[i] = false;
  mode = nextMode;
  sendAll("R");
  pushState();
}

void onVote(char station, char choice) {
  int idx = getIdx(station);
  if (idx < 0 || voted[idx]) return;
  voted[idx] = true;
  if (choice == 'A') vA++;
  else if (choice == 'B') vB++;
  else return;
  if (mode == 'L' || allVoted()) pushState();
}

void readCh() {
  SoftwareSerial* s = ch[cur];
  while (s->available()) {
    String msg = s->readStringUntil('\n');
    msg.trim();
    if (msg.length() < 4) continue;
    Serial.print("RX: "); Serial.println(msg);
    if (msg.charAt(0) == 'V' && msg.charAt(2) == ':')
      onVote(msg.charAt(1), msg.charAt(3));
  }
}

void updateLEDs() {
  digitalWrite(LED_Y, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_R, LOW);
  if (mode == 'S' && !allVoted()) return;
  if (vA == 0 && vB == 0) return;
  if (vA > vB)      digitalWrite(LED_G, HIGH);
  else if (vB > vA) digitalWrite(LED_R, HIGH);
  else              digitalWrite(LED_Y, HIGH);
}

void updateLCD() {
  lcd.setCursor(0, 0);
  if (mode == 'S' && !allVoted()) {
    lcd.print("Votes hidden    ");
  } else {
    lcd.print("A:"); lcd.print(vA);
    lcd.print(" B:"); lcd.print(vB);
    lcd.print("           ");
  }
  lcd.setCursor(0, 1);
  lcd.print("Mode:");
  lcd.print(mode == 'L' ? "LIVE   " : "SECRET ");
  lcd.print(nextMode != mode ? "*" : " ");
  lcd.print("    ");
}

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < N; i++) ch[i]->begin(9600);
  ch[0]->listen();

  pinMode(LED_Y, OUTPUT); pinMode(LED_G, OUTPUT); pinMode(LED_R, OUTPUT);
  pinMode(BTN_RST, INPUT_PULLUP); pinMode(BTN_MOD, INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.print("Voting System");
  lcd.setCursor(0, 1);
  lcd.print("Ready");
  delay(1000);
  lcd.clear();

  Serial.println("=== Master ready ===");
  doReset();
}

void loop() {
  readCh();

  if (millis() - switchTimer > WIN_MS) {
    switchTimer = millis();
    cur = (cur + 1) % N;
    ch[cur]->listen();
  }

  int rst = digitalRead(BTN_RST);
  if (rst == LOW && rstLast == HIGH) doReset();
  rstLast = rst;

  int mod = digitalRead(BTN_MOD);
  if (mod == LOW && modLast == HIGH)
    nextMode = (nextMode == 'L') ? 'S' : 'L';
  modLast = mod;

  if (millis() - dispTimer > DISP_MS) {
    dispTimer = millis();
    updateLEDs();
    updateLCD();
  }
}
