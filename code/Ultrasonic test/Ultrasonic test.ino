#define trigPin 8

float distArray[2];
unsigned long timeMicro;
unsigned long tempMicro1;
int i = 0;
int count = 0;

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  attachInterrupt(0, signalListener, RISING);
  //An interrupt set to catch the signal sent by hypersonic sensor module
}

void loop() {
  digitalWrite(trigPin, HIGH);
  timeMicro = micros();
  tempMicro1 = timeMicro;
  count = 0;
  delayMicroseconds(150);
  digitalWrite(trigPin, LOW);
  delay(350);
}

void signalListener() {
  
  if (i == 0 && count==0) {
    distArray[0] = (float)340 * (micros() - tempMicro1) / 2000000;
    i++;
    Serial.print("Distance: ");
    Serial.print(distArray[0]);
    Serial.println("m");
  }

  else if(count==0){
    distArray[0] = distArray[1];
    distArray[1] = (float)340 * (micros() - tempMicro1) / 2000000;
    Serial.print("Distance: ");
    Serial.print(distArray[1]);
    Serial.println("m");
  }

  if (i != 0 && count==0) {
    Serial.print("Velocity: ");
    Serial.print(speed());
    Serial.println("m/s");
  }
  
  count++;
}

float speed() {
  float answer;

  answer = (distArray[0] - distArray[1]) / 2;

  return answer;
}

