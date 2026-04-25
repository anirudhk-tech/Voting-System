/*
 * Master Controller - Real-Time Voting System
 *
 * Currently configured for Station B only via hardware serial (pins 0/1).
 * Designed to extend to Stations A and D via SoftwareSerial.
 *
 * Pins:
 *   LCD: RS=5, E=6, D4=7, D5=8, D6=9, D7=10
 *   LEDs: Yellow=2, Green=3, Red=4
 *   Buttons: Reset=11, Mode=12
 */

#include <LiquidCrystal.h>

// ===== PINS =====
const int LCD_RS = 5, LCD_E = 6, LCD_D4 = 7, LCD_D5 = 8, LCD_D6 = 9, LCD_D7 = 10;
const int LED_YELLOW = 2, LED_GREEN = 3, LED_RED = 4;
const int BTN_RESET = 11, BTN_MODE = 12;

// ===== STATION CONFIG (extend here when adding stations) =====
const int NUM_STATIONS = 1;
const char STATION_IDS[NUM_STATIONS] = {'B'};

// ===== STATE =====
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

int votesA = 0;
int votesB = 0;
bool stationVoted[NUM_STATIONS] = {false};

char currentMode = 'L';   // 'L' = Live, 'S' = Secret Ballot
char pendingMode = 'L';   // takes effect on next reset

int lastResetState = HIGH;
int lastModeState  = HIGH;

unsigned long lastDisplayUpdate = 0;
const unsigned long DISPLAY_INTERVAL = 200;

// ===== HELPERS =====
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

// ===== BROADCASTS =====
// In future: send via every active serial channel.
// For now: just the hardware Serial port (Station B).
void broadcast(const String& msg) {
  Serial.println(msg);
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

// ===== ROUND CONTROL =====
void resetRound() {
  votesA = 0;
  votesB = 0;
  for (int i = 0; i < NUM_STATIONS; i++) stationVoted[i] = false;
  currentMode = pendingMode;
  broadcastReset();
  broadcastState();
}

// ===== VOTE HANDLING =====
void handleVote(char station, char choice) {
  int idx = stationIndex(station);
  if (idx < 0) return;             // unknown station
  if (stationVoted[idx]) return;   // already voted

  stationVoted[idx] = true;
  if (choice == 'A') votesA++;
  else if (choice == 'B') votesB++;
  else return;

  // Live: broadcast every vote
  // Secret: only broadcast when all stations voted
  if (currentMode == 'L' || allStationsVoted()) {
    broadcastState();
  }
}

void parseIncoming(const String& msg) {
  // Expect "V<station>:<choice>"
  if (msg.length() < 4 || msg.charAt(0) != 'V' || msg.charAt(2) != ':') return;
  handleVote(msg.charAt(1), msg.charAt(3));
}

// ===== DISPLAY =====
void updateLeaderLED() {
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN,  LOW);
  digitalWrite(LED_RED,    LOW);

  // In Secret mode, don't reveal leader until all voted
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

// ===== SETUP / LOOP =====
void setup() {
  Serial.begin(9600);

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

  broadcastReset();
  broadcastState();
}

void loop() {
  // Receive from stations
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();
    if (msg.length() > 0) parseIncoming(msg);
  }

  // Buttons
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

  // Display refresh
  if (millis() - lastDisplayUpdate > DISPLAY_INTERVAL) {
    lastDisplayUpdate = millis();
    updateLeaderLED();
    updateLcd();
  }
}
