/*
 * Master Controller - 2-Station Voting (Station B only for now)
 * 
 * Pins: LCD 5,6,7,8,9,10 | Yellow=2 Green=3 Red=4 | BtnReset=11 BtnMode=12
 * Hardware Serial (pin 0/1) talks to Station B
 */

#include <LiquidCrystal.h>

const int LCD_RS = 5;
const int LCD_E  = 6;
const int LCD_D4 = 7;
const int LCD_D5 = 8;
const int LCD_D6 = 9;
const int LCD_D7 = 10;
const int LED_YELLOW = 2;
const int LED_GREEN  = 3;
const int LED_RED    = 4;
const int BTN_RESET  = 11;
const int BTN_MODE   = 12;

LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// Vote tally
int votesA = 0;
int votesB = 0;

// Per-station voted flag (only B for now; A and D added later)
bool stationBVoted = false;

// Mode: 'L' = Live, 'S' = Secret Ballot
char currentMode = 'L';
char pendingMode = 'L';  // takes effect on next reset

// Button edge detection
int lastResetState = HIGH;
int lastModeState  = HIGH;

// LCD refresh
unsigned long lastLcdUpdate = 0;

void broadcastState() {
  // Send tally to all stations
  Serial.print("T:");
  Serial.print(votesA);
  Serial.print(",");
  Serial.print(votesB);
  Serial.print(",");
  Serial.println(currentMode);
}

void broadcastReset() {
  Serial.println("R");
}

void resetRound() {
  votesA = 0;
  votesB = 0;
  stationBVoted = false;
  currentMode = pendingMode;
  broadcastReset();
  broadcastState();
}

void updateLeaderLED() {
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN,  LOW);
  digitalWrite(LED_RED,    LOW);
  if (votesA == 0 && votesB == 0) return;
  if (votesA > votesB) digitalWrite(LED_GREEN, HIGH);
  else if (votesB > votesA) digitalWrite(LED_RED, HIGH);
  else digitalWrite(LED_YELLOW, HIGH);
}

void updateLcd() {
  lcd.setCursor(0, 0);
  lcd.print("A:");
  lcd.print(votesA);
  lcd.print(" B:");
  lcd.print(votesB);
  lcd.print("    ");

  lcd.setCursor(0, 1);
  lcd.print("Mode:");
  lcd.print(currentMode == 'L' ? "LIVE  " : "SECRET");
  lcd.print(" ");
}

void handleVote(char station, char choice) {
  // Station validation (only B for now)
  if (station == 'B') {
    if (stationBVoted) return;  // already voted, ignore
    stationBVoted = true;
  } else {
    return;  // unknown station
  }

  if (choice == 'A') votesA++;
  else if (choice == 'B') votesB++;
  else return;

  // In Live mode, broadcast immediately
  // In Secret mode, wait until all stations voted (only B for now, so same effect)
  if (currentMode == 'L') {
    broadcastState();
  } else {
    // Secret: only broadcast when all stations have voted
    if (stationBVoted) {  // expand this when adding A and D
      broadcastState();
    }
  }
}

void parseIncoming(const String& msg) {
  // Expect format "V<station>:<choice>"
  if (msg.length() < 4 || msg.charAt(0) != 'V') return;
  char station = msg.charAt(1);
  if (msg.charAt(2) != ':') return;
  char choice = msg.charAt(3);
  handleVote(station, choice);
}

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
  // 1. Check for incoming vote messages
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();
    if (msg.length() > 0) parseIncoming(msg);
  }

  // 2. Check buttons
  int resetState = digitalRead(BTN_RESET);
  int modeState  = digitalRead(BTN_MODE);

  if (resetState == LOW && lastResetState == HIGH) {
    resetRound();
  }
  lastResetState = resetState;

  if (modeState == LOW && lastModeState == HIGH) {
    pendingMode = (pendingMode == 'L') ? 'S' : 'L';
  }
  lastModeState = modeState;

  // 3. Update LEDs and LCD periodically
  if (millis() - lastLcdUpdate > 200) {
    lastLcdUpdate = millis();
    updateLeaderLED();
    updateLcd();
  }
}
