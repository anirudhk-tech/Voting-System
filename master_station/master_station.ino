#include <SoftwareSerial.h>
SoftwareSerial s(A2, A3);  // RX=A2, TX=A3

void setup() {
  Serial.begin(9600);
  delay(1000);
  s.begin(9600);
  s.listen();
  Serial.println("Ready");
}

void loop() {
  s.println("PING");
  delay(100);
  while (s.available()) {
    char c = s.read();
    Serial.print("GOT: ");
    Serial.println(c);
  }
  delay(900);
}
