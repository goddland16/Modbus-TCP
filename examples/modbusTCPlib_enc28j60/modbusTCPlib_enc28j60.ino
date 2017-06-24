/*
  This is Modbus test code to demonstrate all the Modbus functions with 
  with Ethernet IC ENC28J60
  
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
  copyright @ phoenixrobotix.com
  
*/
#define ENC28J60     = 1

unsigned int param_value_int[7];
#include <UIPEthernet.h>
#include <avr/wdt.h>

IPAddress ModbusDeviceIP(10, 10, 108, 211);
IPAddress moduleIPAddress(10, 10, 108, 23);  // Anything other than the device IP Address
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xE1 };


#include <ModbusTCP.h>

ModbusTCP node(1);

void setup()
{
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  
  Serial.begin(9600);
  //wdt_enable(WDTO_4S);
  //WDTCSR |= _BV(WDIE);
  wdt_disable(); 
  delay(1000);
  Ethernet.begin(mac, moduleIPAddress);
  node.setServerIPAddress(ModbusDeviceIP);
  Serial.println("Setup");
  delay(6000);

}


void loop()
{
  uint8_t result;



//  node.setTransactionID(random(100));
  result = node.readHoldingRegisters(1, 12);
  Serial.print("hola: ");
  Serial.println(result, HEX);
  //if(result == node.MBServerConnectionTimeOut)
  if(result != 0)
  {
    node.MBconnectionFlag = 0;
    Serial.println("TimeOut");
    if(result == 5) {

      //Ethernet.begin(mac, moduleIPAddress);
      wdt_enable(WDTO_8S);
      WDTCSR |= _BV(WDIE);
    }
    
    delay(6000);
  }
  int len = node.getResponseBufferLength();
  Serial.println("Response Length: " + String(len));
  for (byte j = 0; j < len; j++)
  {
    Serial.print(node.getResponseBuffer(j));
    Serial.print(" ");
  }
  Serial.println();
  node.clearResponseBuffer();
  delay(100);

  


//  node.writeSingleRegister(5, 3);
//  Serial.println(result, HEX);
//  delay(500);



//  for (byte i = 0; i < 5; i++)
//  {
//    node.setTransmitBuffer(i, (i+100));
//  }
//  node.writeMultipleRegisters(2, 5);
//  Serial.println(result, HEX);
//  delay(500);



//  node.writeSingleCoil(20, 1);
//  delay(500);



  node.setTransmitBuffer(0, 0b1010101001010101);
//
  node.writeMultipleCoils(20, 16);
  delay(500);
  result = node.readCoils(20, 18);
  len = node.getResponseBufferLength();
  Serial.println("Response Length: " + String(len));
  Serial.println(node.getResponseBuffer(0), BIN);
  delay(200);






  result = node.readHoldingRegisters(2, 8);
  len = node.getResponseBufferLength();
  Serial.println("Response Length: " + String(len));
  for (byte j = 0; j < len; j++)
  {
    param_value_int[j] = node.getResponseBuffer(j);
    Serial.print(param_value_int[j]);
    Serial.print(" ");
  }
  Serial.println();
  node.clearResponseBuffer();
  delay(10000);
}


