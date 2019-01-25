#define LEDPin 12

char UID;

void setup() {
  Serial.begin(115200);

  pinMode(LEDPin, OUTPUT);
}

void loop() {

  Serial.write((byte)0x02);
  Serial.write((byte)0x00);
  Serial.write((byte)0xf0);

  delay(100);
  //small time interval for response
  if(Serial.available()){
    UID = Serial.read();
    //temporarily store the first byte of response packet to test
    digitalWrite(LEDPin, HIGH);
    Serial.print("Data: ");
    Serial.println(UID, HEX);
  }
  digitalWrite(LEDPin, LOW);
  Serial.flush();

}






