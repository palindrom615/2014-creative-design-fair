public class superSonic{
	public static void main(string args[]){
		for(;;){
			
		}
	}
	final GpioPinDigitalOutput pin = gpio.provisionDigitalOutputPin(RaspiPin.GPIO_01, "MyLED", PinState.HIGH);
	final GpioPinDigitalOutput pin = gpio.provisionDigitalOutputPin(RaspiPin.GPIO_00, "MyLED", PinState.LOW);
	
	public float signalListener() {
  
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
}