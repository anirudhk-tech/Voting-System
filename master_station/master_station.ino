/*
 * Master Controller - Component Test
 * Tests: 2 pushbuttons, 3 LEDs, 16x2 LCD
 */

#include <LiquidCrystal.h>

const int LCD_RS = 5;
const int LCD_E  = 6;
const int LCD_D4 = 7;
const int LCD_D5 = 8;
const int LCD_D6 = 9;
const int LCD_D7 = 10;

const int BTN_RESET = 11;
const int BTN_MODE  = 12;

const int LED_YELLOW = 2;
const int LED_GREEN  = 3;
const int LED_RED    = 4;

LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

int lastResetState = HIGH;
int lastModeState  = HIGH;
bool modeSecret    = false;

unsigned long lastLedChange = 0;
int ledPhase = 0;
const unsigned long LED_INTERVAL = 800;

void setup() {
  Serial.begin(9600);
  Serial.println(F("=== Master Test ==="));

  pinMode(BTN_RESET, INPUT_PULLUP);
  pinMode(BTN_MODE,  INPUT_PULLUP);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN,  OUTPUT);
  pinMode(LED_RED,    OUTPUT);

  lcd.begin(16, 2);
  lcd.print("Master Test");
  lcd.setCursor(0, 1);
  lcd.print("Booting...");
  delay(1500);
  lcd.clear();
}

void loop() {
  if (millis() - lastLedChange > LED_INTERVAL) {
    lastLedChange = millis();
    ledPhase = (ledPhase + 1) % 5;

    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_GREEN,  LOW);
    digitalWrite(LED_RED,    LOW);

    switch (ledPhase) {
      case 0: digitalWrite(LED_YELLOW, HIGH); break;
      case 1: digitalWrite(LED_GREEN,  HIGH); break;
      case 2: digitalWrite(LED_RED,    HIGH); break;
      case 3:
        digitalWrite(LED_YELLOW, HIGH);
        digitalWrite(LED_GREEN,  HIGH);
        digitalWrite(LED_RED,    HIGH);
        break;
      case 4: break;
    }
  }

  int resetState = digitalRead(BTN_RESET);
  int modeState  = digitalRead(BTN_MODE);

  if (resetState == LOW && lastResetState == HIGH) {
    lcd.clear();
    lcd.print("RESET PRESSED");
    delay(400);
    lcd.clear();
  }
  lastResetState = resetState;

  if (modeState == LOW && lastModeState == HIGH) {
    modeSecret = !modeSecret;
    lcd.clear();
    lcd.print("MODE: ");
    lcd.print(modeSecret ? "SECRET" : "LIVE");
    delay(400);
    lcd.clear();
  }
  lastModeState = modeState;

  lcd.setCursor(0, 0);
  lcd.print("Master Test");
  lcd.setCursor(0, 1);
  lcd.print("Mode:");
  lcd.print(modeSecret ? "SEC " : "LIVE");
  lcd.print(" L:");
  lcd.print(ledPhase);

  delay(20);
}
