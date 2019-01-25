// START SNIPPET: serial-snippet


/*
 * #%L
 * **********************************************************************
 * ORGANIZATION  :  Pi4J
 * PROJECT       :  Pi4J :: Java Examples
 * FILENAME      :  SerialExample.java  
 * 
 * This file is part of the Pi4J project. More information about 
 * this project can be found here:  http://www.pi4j.com/
 * **********************************************************************
 * %%
 * Copyright (C) 2012 - 2013 Pi4J
 * %%
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * #L%
 */


import java.util.Date;

import com.pi4j.io.serial.Serial;
import com.pi4j.io.serial.SerialDataEvent;
import com.pi4j.io.serial.SerialDataListener;
import com.pi4j.io.serial.SerialFactory;
import com.pi4j.io.serial.SerialPortException;

/**
 * This example code demonstrates how to perform serial communications using the Raspberry Pi.
 * 
 * @author Robert Savage
 */
public class SerialExample {

	char UID;
    
    public static void main(String args[]) throws InterruptedException {
        
        // !! ATTENTION !!
        // By default, the serial port is configured as a console port 
        // for interacting with the Linux OS shell.  If you want to use 
        // the serial port in a software program, you must disable the 
        // OS from using this port.  Please see this blog article by  
        // Clayton Smith for step-by-step instructions on how to disable 
        // the OS console for this port:
        // http://www.irrational.net/2012/04/19/using-the-raspberry-pis-serial-port/
                
        System.out.println("<--Pi4J--> Serial Communication Example ... started.");
        System.out.println(" ... connect using settings: 38400, N, 8, 1.");
        System.out.println(" ... data received on serial port should be displayed below.");
        
		return;
		}
		public char readUID(){
        // create an instance of the serial communications class
        final Serial serial = SerialFactory.createInstance();

        // create and register the serial data listener
        serial.addListener(new SerialDataListener() {
            @Override
            public void dataReceived(SerialDataEvent event) {
                // print out the data received to the console
                System.out.print(event.getData());
            }            
        });
                
        try {
            // open the default serial port provided on the GPIO header
            serial.open(Serial.DEFAULT_COM_PORT, 115200);
            
            // continuous loop to keep the program running until the user terminates the program
            for (;;) {
                try {    
                    // write a individual bytes to the serial transmit buffer
                    serial.write((byte) 0x02);
                    serial.write((byte) 0x00);
					serial.write((byte) 0xf0);
					Thread.sleep(100);
					}
                catch(IllegalStateException ex){
                    ex.printStackTrace();                    
                }
                UID=serial.read();
                // wait 1 second before continuing
                
            }
            
        }
        catch(SerialPortException ex) {
            System.out.println(" ==>> SERIAL SETUP FAILED : " + ex.getMessage());
            return UID;
        }
    }
	public void writeUID(char length, char UID, char blockNUMBER, char data1, char data2, char data3, char data4){
        // create an instance of the serial communications class
        final Serial serial = SerialFactory.createInstance();

        // create and register the serial data listener
        serial.addListener(new SerialDataListener() {
            @Override
            public void dataReceived(SerialDataEvent event) {
                // print out the data received to the console
                System.out.print(event.getData());
            }            
        });
                
        try {
            // open the default serial port provided on the GPIO header
            serial.open(Serial.DEFAULT_COM_PORT, 115200);
            
            // continuous loop to keep the program running until the user terminates the program
            for (;;) {
                try {    //명령: Length(조회와 마찬가지) + Flag(TI社 태그는 0x62) + 
				//0x21(명령코드) + UID(8byte) + Block Number + Data1 + Data2 
				//+ Data3 + Data4
                    // write a individual bytes to the serial transmit buffer
                    serial.write(length);
                    serial.write((byte) 0x62);
					serial.write((byte) 0x21);
					serial.write(UID);
					serial.write(blockNUMBER);
					serial.write(data1);
					serial.write(data2);
					serial.write(data3);
					serial.write(data4);
					Thread.sleep(100);
					}
                catch(IllegalStateException ex){
                    ex.printStackTrace();                    
                }
                // wait 1 second before continuing
                
            }
            
        }
        catch(SerialPortException ex) {
            System.out.println(" ==>> SERIAL SETUP FAILED : " + ex.getMessage());
            return;
        }
	}
}

// END SNIPPET: serial-snippet
