void setup() {
  Serial.begin(9600);  // hardware serial, pins 0/1
}

void loop() {
  Serial.println("HELLO_FROM_B");
  delay(1000);
}
