// 7-segment display station (common cathode)
// segments: a=7  b=6  c=3  d=4  e=5  f=8  g=9  dp=2
// buttons:  A=11  B=12
// light sensor: A0

#define BTN_A        11
#define BTN_B        12
#define PHOTO_PIN    A0
#define SEG_DP       2
#define LIGHT_THRESH 400

const int segs[7] = {7, 6, 3, 4, 5, 8, 9};  // a b c d e f g
const char STATION_ID = 'D';

const byte digits[10][7] = {
  {1,1,1,1,1,1,0},  // 0
  {0,1,1,0,0,0,0},  // 1
  {1,1,0,1,1,0,1},  // 2
  {1,1,1,1,0,0,1},  // 3
  {0,1,1,0,0,1,1},  // 4
  {1,0,1,1,0,1,1},  // 5
  {1,0,1,1,1,1,1},  // 6
  {1,1,1,0,0,0,0},  // 7
  {1,1,1,1,1,1,1},  // 8
  {1,1,1,1,0,1,1}   // 9
};

const byte BLANK[7] = {0,0,0,0,0,0,0};
const byte DASH[7]  = {0,0,0,0,0,0,1};

bool voted = false;
int lastA = HIGH;
int lastB = HIGH;

int tA = 0;
int tB = 0;
char voteMode = 'L';
bool revealed = true;

unsigned long flashTimer = 0;
bool flashOn = false;

void writeSegs(const byte pat[7]) {
  for (int i = 0; i < 7; i++)
    digitalWrite(segs[i], pat[i] ? HIGH : LOW);
}

void showNum(int n) {
  if (n < 0) { writeSegs(BLANK); return; }
  if (n > 9) n = 9;
  writeSegs(digits[n]);
}

void sendVote(char choice) {
  for (int i = 0; i < 8; i++) {
    Serial.print("V");
    Serial.print(STATION_ID);
    Serial.print(":");
    Serial.println(choice);
    delay(30);
  }
}

void parseMsg(const String& msg)
{
  if (msg.length() == 0) return;

  if (msg.charAt(0) == 'R') {
    voted = false;
    tA = 0; tB = 0;
    revealed = true;
    return;
  }

  if (msg.charAt(0) == 'T') {
    int i1 = msg.indexOf(':');
    int i2 = msg.indexOf(',');
    int i3 = msg.indexOf(',', i2 + 1);
    if (i1 < 0 || i2 < 0 || i3 < 0) return;
    tA = msg.substring(i1 + 1, i2).toInt();
    tB = msg.substring(i2 + 1, i3).toInt();
    voteMode = msg.charAt(i3 + 1);
    revealed = true;
  }
}

void refreshDisplay()
{
  int light = analogRead(PHOTO_PIN);
  digitalWrite(SEG_DP, (light < LIGHT_THRESH) ? HIGH : LOW);

  if (voted && !revealed) {
    if (millis() - flashTimer > 400) {
      flashTimer = millis();
      flashOn = !flashOn;
    }
    writeSegs(flashOn ? DASH : BLANK);
    return;
  }

  showNum(max(tA, tB));
}

void setup()
{
  Serial.begin(9600);

  for (int i = 0; i < 7; i++) {
    pinMode(segs[i], OUTPUT);
  }
  pinMode(SEG_DP, OUTPUT);
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);

  showNum(0);
}

void loop()
{
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();
    parseMsg(msg);
  }

  int a = digitalRead(BTN_A);
  int b = digitalRead(BTN_B);

  if (!voted) {
    if (a == LOW && lastA == HIGH) {
      voted = true;
      if (voteMode == 'S') revealed = false;
      sendVote('A');
    }
    if (b == LOW && lastB == HIGH) {
      voted = true;
      if (voteMode == 'S') revealed = false;
      sendVote('B');
    }
  }

  lastA = a;
  lastB = b;

  refreshDisplay();
  delay(20);
}
