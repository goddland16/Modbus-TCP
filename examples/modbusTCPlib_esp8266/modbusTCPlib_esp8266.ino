/*
  This is Modbus test code to demonstrate all the Modbus functions with
  with Ethernet IC WIZNET W5100

  ModbusTCP is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ModbusTCP is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with ModbusTCP.  If not, see <http://www.gnu.org/licenses/>.

  Adopted from ModbusMaster for RTU over RS-485 by Doc Walker
  Modified by Narendra Dehury for TCP.
  Modified by Florian K for ESP8266.
  copyright @ phoenixrobotix.com

*/
#define ESP8266 1

unsigned int param_value_int[7];
#include <Ethernet.h>

IPAddress ModbusDeviceIP(10, 10, 108, 211);  // Put IP Address of PLC here
IPAddress moduleIPAddress(10, 10, 108, 23);  // Assign Anything other than the PLC IP Address

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xE1 };


#include <ModbusTCP.h>

ModbusTCP node(1);                            // Unit Identifier.

void setup()
{

  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);                      // To disable slave select for SD card; depricated.

  Serial.begin(9600);
  delay(1000);
  Ethernet.begin(mac, moduleIPAddress);
  node.setServerIPAddress(ModbusDeviceIP);
  delay(6000);                                // To provide sufficient time to initialize.

}


void loop()
{
  uint8_t result;

  node.setTransactionID(random(100));           // Not necessary; but good to distinguish each frame individually.
  result = node.readHoldingRegisters(1, 12);    // Read Holding Registers

  Serial.println(result, HEX);
  if(result != 0)
  {
    Serial.println("TimeOut");

    delay(6000);
  }

  int len = node.getResponseBufferLength();
  Serial.println("Response Length: " + String(len));// See the length of data packet received.
  for (byte j = 0; j < len; j++)
  {
    Serial.print(node.getResponseBuffer(j));       // Inspect the data.
    Serial.print(" ");
  }
  Serial.println();
  node.clearResponseBuffer();
  delay(100);




  node.writeSingleRegister(5, 3);                  // Write single register
  Serial.println(result, HEX);
  delay(500);



  for (byte i = 0; i < 5; i++)
  {
    node.setTransmitBuffer(i, (i+100));
  }
  node.writeMultipleRegisters(2, 5);               // Write multiple register
  Serial.println(result, HEX);
  delay(500);



  node.writeSingleCoil(20, 1);                     // Write Single coil
  delay(500);



  node.setTransmitBuffer(0, 0b1010101001010101);

  node.writeMultipleCoils(20, 16);                 // Write multiple coils
  delay(500);
  result = node.readCoils(20, 18);
  len = node.getResponseBufferLength();
  Serial.println("Response Length: " + String(len));
  Serial.println(node.getResponseBuffer(0), BIN);


  node.clearResponseBuffer();
  delay(5000);
}
