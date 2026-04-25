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

LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

int votesA = 0;
int votesB = 0;
bool stationBVoted = false;
int lastResetState = HIGH;

void broadcastState() {
  Serial.print("T:");
  Serial.print(votesA);
  Serial.print(",");
  Serial.println(votesB);
}

void broadcastReset() {
  Serial.println("R");
}

void resetRound() {
  votesA = 0;
  votesB = 0;
  stationBVoted = false;
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
  lcd.print("Votes A:");
  lcd.print(votesA);
  lcd.print(" B:");
  lcd.print(votesB);
  lcd.print("  ");
  lcd.setCursor(0, 1);
  lcd.print("Press to vote   ");
}

void handleVote(char station, char choice) {
  if (station == 'B' && !stationBVoted) {
    stationBVoted = true;
    if (choice == 'A') votesA++;
    else if (choice == 'B') votesB++;
    broadcastState();
  }
}

void parseIncoming(const String& msg) {
  // DEBUG: show full message on LCD line 2 briefly
  lcd.setCursor(0, 1);
  lcd.print("RX:");
  lcd.print(msg);
  lcd.print("        ");
  
  if (msg.length() < 4 || msg.charAt(0) != 'V' || msg.charAt(2) != ':') return;
  handleVote(msg.charAt(1), msg.charAt(3));
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN,  OUTPUT);
  pinMode(LED_RED,    OUTPUT);
  pinMode(BTN_RESET, INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.print("Voting Ready");
  delay(1000);
  lcd.clear();

  broadcastReset();
}

void loop() {
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();
    if (msg.length() > 0) parseIncoming(msg);
  }

  int resetState = digitalRead(BTN_RESET);
  if (resetState == LOW && lastResetState == HIGH) {
    resetRound();
  }
  lastResetState = resetState;

  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 200) {
    lastUpdate = millis();
    updateLeaderLED();
    updateLcd();
  }
}
