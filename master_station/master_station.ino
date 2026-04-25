/*
 * Master Controller - Real-Time Voting System
 * Stations B (A0/A1), D (A2/A3), A (A4/A5) via SoftwareSerial.
 * USB Serial (pin 0/1) used for debug output.
 */

#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

const int LCD_RS = 5, LCD_E = 6, LCD_D4 = 7, LCD_D5 = 8, LCD_D6 = 9, LCD_D7 = 10;
const int LED_YELLOW = 2, LED_GREEN = 3, LED_RED = 4;
const int BTN_RESET = 11, BTN_MODE = 12;

SoftwareSerial stationB(A0, A1);  // RX=A0, TX=A1
SoftwareSerial stationD(A2, A3);  // RX=A2, TX=A3
SoftwareSerial stationA(A4, A5);  // RX=A4, TX=A5

const int NUM_STATIONS = 3;
const char STATION_IDS[NUM_STATIONS] = {'B', 'D', 'A'};

SoftwareSerial* channels[NUM_STATIONS] = {&stationB, &stationD, &stationA};

LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

int votesA = 0;
int votesB = 0;
bool stationVoted[NUM_STATIONS] = {false, false, false};

char currentMode = 'L';
char pendingMode = 'L';

int lastResetState = HIGH;
int lastModeState  = HIGH;

unsigned long lastDisplayUpdate = 0;
const unsigned long DISPLAY_INTERVAL = 200;

unsigned long lastListenSwitch = 0;
const unsigned long LISTEN_WINDOW = 100;
int activeChannel = 0;

int stationIndex(char id) {
  for (int i = 0; i < NUM_STATIONS; i++) {
    if (STATION_IDS[i] == id) return i;
  }
  return -1;
}

bool allStationsVoted() {
  for (int i = 0; i < NUM_STATIONS; i++) {
    if (!stationVoted[i]) return false;
  }
  return true;
}

void broadcast(const String& msg) {
  for (int i = 0; i < NUM_STATIONS; i++) {
    channels[i]->println(msg);
  }
  Serial.print("TX: "); Serial.println(msg);
}

void broadcastState() {
  String msg = "T:";
  msg += votesA;
  msg += ",";
  msg += votesB;
  msg += ",";
  msg += currentMode;
  broadcast(msg);
}

void broadcastReset() {
  broadcast("R");
}

void resetRound() {
  votesA = 0;
  votesB = 0;
  for (int i = 0; i < NUM_STATIONS; i++) stationVoted[i] = false;
  currentMode = pendingMode;
  broadcastReset();
  broadcastState();
}

void handleVote(char station, char choice) {
  int idx = stationIndex(station);
  if (idx < 0) return;
  if (stationVoted[idx]) return;

  stationVoted[idx] = true;
  if (choice == 'A') votesA++;
  else if (choice == 'B') votesB++;
  else return;

  if (currentMode == 'L' || allStationsVoted()) {
    broadcastState();
  }
}

void parseIncoming(const String& msg) {
  Serial.print("RX: "); Serial.println(msg);
  if (msg.length() < 4 || msg.charAt(0) != 'V' || msg.charAt(2) != ':') return;
  handleVote(msg.charAt(1), msg.charAt(3));
}

void readActiveChannel() {
  SoftwareSerial* ch = channels[activeChannel];
  while (ch->available()) {
    String msg = ch->readStringUntil('\n');
    msg.trim();
    if (msg.length() > 0) parseIncoming(msg);
  }
}

void switchListenChannel() {
  activeChannel = (activeChannel + 1) % NUM_STATIONS;
  channels[activeChannel]->listen();
}

void updateLeaderLED() {
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN,  LOW);
  digitalWrite(LED_RED,    LOW);

  if (currentMode == 'S' && !allStationsVoted()) return;
  if (votesA == 0 && votesB == 0) return;

  if (votesA > votesB) digitalWrite(LED_GREEN, HIGH);
  else if (votesB > votesA) digitalWrite(LED_RED, HIGH);
  else digitalWrite(LED_YELLOW, HIGH);
}

void updateLcd() {
  lcd.setCursor(0, 0);
  if (currentMode == 'S' && !allStationsVoted()) {
    lcd.print("Votes hidden    ");
  } else {
    lcd.print("A:");
    lcd.print(votesA);
    lcd.print(" B:");
    lcd.print(votesB);
    lcd.print("           ");
  }

  lcd.setCursor(0, 1);
  lcd.print("Mode:");
  lcd.print(currentMode == 'L' ? "LIVE   " : "SECRET ");
  if (pendingMode != currentMode) lcd.print("*");
  else lcd.print(" ");
  lcd.print("    ");
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < NUM_STATIONS; i++) {
    channels[i]->begin(9600);
  }
  channels[0]->listen();
  activeChannel = 0;

  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN,  OUTPUT);
  pinMode(LED_RED,    OUTPUT);
  pinMode(BTN_RESET, INPUT_PULLUP);
  pinMode(BTN_MODE,  INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.print("Voting System");
  lcd.setCursor(0, 1);
  lcd.print("Ready");
  delay(1000);
  lcd.clear();

  Serial.println("=== Master ready ===");

  broadcastReset();
  broadcastState();
}

void loop() {
  readActiveChannel();

  if (millis() - lastListenSwitch > LISTEN_WINDOW) {
    lastListenSwitch = millis();
    switchListenChannel();
  }

  int resetState = digitalRead(BTN_RESET);
  if (resetState == LOW && lastResetState == HIGH) {
    resetRound();
  }
  lastResetState = resetState;

  int modeState = digitalRead(BTN_MODE);
  if (modeState == LOW && lastModeState == HIGH) {
    pendingMode = (pendingMode == 'L') ? 'S' : 'L';
  }
  lastModeState = modeState;

  if (millis() - lastDisplayUpdate > DISPLAY_INTERVAL) {
    lastDisplayUpdate = millis();
    updateLeaderLED();
    updateLcd();
  }
}
