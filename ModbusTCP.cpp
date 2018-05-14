/**
@file
Arduino library for communicating with Modbus server over Ethernet in TCP.
*/
/*

  ModbusTCP.h - Arduino library for communicating with Modbus server
  over Ethernet (via TCP protocol).
  
  This file is part of ModbusTCP.
  
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

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "ModbusTCP.h"



/* _____PUBLIC FUNCTIONS_____________________________________________________ */

/**
Constructor.
*/
ModbusTCP::ModbusTCP(void)
{
  _u8MBUnitID = 1;
  _u16MBTransactionID = 1;
}


/**
Constructor.

Creates class object using Modbus Unit ID. This is for unique identification
 of individual client by the server. 

@overload void ModbusTCP::ModbusTCP(uint8_t u8MBUnitID)
@param u8MBUnitID Modbus Unit ID (1..255)
*/
ModbusTCP::ModbusTCP(uint8_t u8MBUnitID)
{
  _u8MBUnitID = u8MBUnitID;
}




void ModbusTCP::setUnitId(uint8_t u8MBUnitID)
{
  _u8MBUnitID = u8MBUnitID;
}


void ModbusTCP::setServerIPAddress(IPAddress ipAddr)
{
  serverIP = ipAddr;
}


void ModbusTCP::setTransactionID(uint16_t transactionID)
{
  _u16MBTransactionID = transactionID;
}


/**
Set idle time callback function (cooperative multitasking).

This function gets called in the idle time between transmission of data
and response from Unit. Do not call functions that read from the serial
buffer that is used by ModbusTCP. Use of i2c/TWI, 1-Wire, other
serial ports, etc. is permitted within callback function.

@see ModbusTCP::ModbusMasterTransaction()
*/
void ModbusTCP::idle(void (*idle)())
{
  _idle = idle;
}

/**
Retrieve data from response buffer.

@see ModbusMaster::clearResponseBuffer()
@param u8Index index of response buffer array (0x00..0x3F)
@return value in position u8Index of response buffer (0x0000..0xFFFF)
@ingroup buffer
*/

uint8_t ModbusTCP::getResponseBufferLength()
{
  return _u8ResponseBufferLength;
}



/**
Retrieve data from response buffer.

@see ModbusTCP::clearResponseBuffer()
@param u8Index index of response buffer array (0x00..0x3F)
@return value in position u8Index of response buffer (0x0000..0xFFFF)
@ingroup buffer
*/
uint16_t ModbusTCP::getResponseBuffer(uint8_t u8Index)
{
  if (u8Index < MaxBufferSize)
  {
    return _u16TxRxBuffer[u8Index];
  }
  else
  {
    return 0xFFFF;
  }
}


/**
Clear Modbus response buffer.

@see ModbusTCP::getResponseBuffer(uint8_t u8Index)
@ingroup buffer
*/
void ModbusTCP::clearResponseBuffer()
{
  uint8_t i;
  
  for (i = 0; i < MaxBufferSize; i++)
  {
    _u16TxRxBuffer[i] = 0;
  }
}

/**
Place data in transmit buffer.

@see ModbusTCP::clearTransmitBuffer()
@param u8Index index of transmit buffer array (0x00..0x3F)
@param u16Value value to place in position u8Index of transmit buffer (0x0000..0xFFFF)
@return 0 on success; exception number on failure
@ingroup buffer
*/
uint8_t ModbusTCP::setTransmitBuffer(uint8_t u8Index, uint16_t u16Value)
{
  if (u8Index < MaxBufferSize)
  {
    _u16TxRxBuffer[u8Index] = u16Value;
    return MBSuccess;
  }
  else
  {
    return MBIllegalDataAddress;
  }
}


/**
Clear Modbus transmit buffer.

@see ModbusTCP::setTransmitBuffer(uint8_t u8Index, uint16_t u16Value)
@ingroup buffer
*/
void ModbusTCP::clearTransmitBuffer()
{
  uint8_t i;
  
  for (i = 0; i < MaxBufferSize; i++)
  {
    _u16TxRxBuffer[i] = 0;
  }
}


/**
Modbus function 0x01 Read Coils.

This function code is used to read from 1 to 2000 contiguous status of 
coils in a remote device. The request specifies the starting address, 
i.e. the address of the first coil specified, and the number of coils. 
Coils are addressed starting at zero.

The coils in the response buffer are packed as one coil per bit of the 
data field. Status is indicated as 1=ON and 0=OFF. The LSB of the first 
data word contains the output addressed in the query. The other coils 
follow toward the high order end of this word and from low order to high 
order in subsequent words.

If the returned quantity is not a multiple of sixteen, the remaining 
bits in the final data word will be padded with zeros (toward the high 
order end of the word).

@param u16ReadAddress address of first coil (0x0000..0xFFFF)
@param u16BitQty quantity of coils to read (1..2000, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup discrete
*/
uint8_t ModbusTCP::readCoils(uint16_t u16ReadAddress, uint16_t u16BitQty)
{
  _u16ReadAddress = u16ReadAddress;
  _u16ReadQty = u16BitQty;
  return ModbusMasterTransaction(MBReadCoils);
}


/**
Modbus function 0x02 Read Discrete Inputs.

This function code is used to read from 1 to 2000 contiguous status of 
discrete inputs in a remote device. The request specifies the starting 
address, i.e. the address of the first input specified, and the number 
of inputs. Discrete inputs are addressed starting at zero.

The discrete inputs in the response buffer are packed as one input per 
bit of the data field. Status is indicated as 1=ON; 0=OFF. The LSB of 
the first data word contains the input addressed in the query. The other 
inputs follow toward the high order end of this word, and from low order 
to high order in subsequent words.

If the returned quantity is not a multiple of sixteen, the remaining 
bits in the final data word will be padded with zeros (toward the high 
order end of the word).

@param u16ReadAddress address of first discrete input (0x0000..0xFFFF)
@param u16BitQty quantity of discrete inputs to read (1..2000, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup discrete
*/
uint8_t ModbusTCP::readDiscreteInputs(uint16_t u16ReadAddress,
  uint16_t u16BitQty)
{
  _u16ReadAddress = u16ReadAddress;
  _u16ReadQty = u16BitQty;
  return ModbusMasterTransaction(MBReadDiscreteInputs);
}


/**
Modbus function 0x03 Read Holding Registers.

This function code is used to read the contents of a contiguous block of 
holding registers in a remote device. The request specifies the starting 
register address and the number of registers. Registers are addressed 
starting at zero.

The register data in the response buffer is packed as one word per 
register.

@param u16ReadAddress address of the first holding register (0x0000..0xFFFF)
@param u16ReadQty quantity of holding registers to read (1..125, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t ModbusTCP::readHoldingRegisters(uint16_t u16ReadAddress,
  uint16_t u16ReadQty)
{
  _u16ReadAddress = u16ReadAddress;
  _u16ReadQty = u16ReadQty;
  return ModbusMasterTransaction(MBReadHoldingRegisters);
}


/**
Modbus function 0x04 Read Input Registers.

This function code is used to read from 1 to 125 contiguous input 
registers in a remote device. The request specifies the starting 
register address and the number of registers. Registers are addressed 
starting at zero.

The register data in the response buffer is packed as one word per 
register.

@param u16ReadAddress address of the first input register (0x0000..0xFFFF)
@param u16ReadQty quantity of input registers to read (1..125, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t ModbusTCP::readInputRegisters(uint16_t u16ReadAddress,
  uint8_t u16ReadQty)
{
  _u16ReadAddress = u16ReadAddress;
  _u16ReadQty = u16ReadQty;
  return ModbusMasterTransaction(MBReadInputRegisters);
}


/**
Modbus function 0x05 Write Single Coil.

This function code is used to write a single output to either ON or OFF 
in a remote device. The requested ON/OFF state is specified by a 
constant in the state field. A non-zero value requests the output to be 
ON and a value of 0 requests it to be OFF. The request specifies the 
address of the coil to be forced. Coils are addressed starting at zero.

@param u16WriteAddress address of the coil (0x0000..0xFFFF)
@param u8State 0=OFF, non-zero=ON (0x00..0xFF)
@return 0 on success; exception number on failure
@ingroup discrete
*/
uint8_t ModbusTCP::writeSingleCoil(uint16_t u16WriteAddress, uint8_t u8State)
{
  _u16WriteAddress = u16WriteAddress;
  _u16WriteQty = (u8State ? 0xFF00 : 0x0000);
  return ModbusMasterTransaction(MBWriteSingleCoil);
}


/**
Modbus function 0x06 Write Single Register.

This function code is used to write a single holding register in a 
remote device. The request specifies the address of the register to be 
written. Registers are addressed starting at zero.

@param u16WriteAddress address of the holding register (0x0000..0xFFFF)
@param u16WriteValue value to be written to holding register (0x0000..0xFFFF)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t ModbusTCP::writeSingleRegister(uint16_t u16WriteAddress,
  uint16_t u16WriteValue)
{
  _u16WriteAddress = u16WriteAddress;
  _u16WriteQty = 0;
  _u16TxRxBuffer[0] = u16WriteValue;
  return ModbusMasterTransaction(MBWriteSingleRegister);
}


/**
Modbus function 0x0F Write Multiple Coils.

This function code is used to force each coil in a sequence of coils to 
either ON or OFF in a remote device. The request specifies the coil 
references to be forced. Coils are addressed starting at zero.

The requested ON/OFF states are specified by contents of the transmit 
buffer. A logical '1' in a bit position of the buffer requests the 
corresponding output to be ON. A logical '0' requests it to be OFF.

@param u16WriteAddress address of the first coil (0x0000..0xFFFF)
@param u16BitQty quantity of coils to write (1..2000, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup discrete
*/
uint8_t ModbusTCP::writeMultipleCoils(uint16_t u16WriteAddress,
  uint16_t u16BitQty)
{
  _u16WriteAddress = u16WriteAddress;
  _u16WriteQty = u16BitQty;
  return ModbusMasterTransaction(MBWriteMultipleCoils);
}


/**
Modbus function 0x10 Write Multiple Registers.

This function code is used to write a block of contiguous registers (1 
to 123 registers) in a remote device.

The requested written values are specified in the transmit buffer. Data 
is packed as one word per register.

@param u16WriteAddress address of the holding register (0x0000..0xFFFF)
@param u16WriteQty quantity of holding registers to write (1..123, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t ModbusTCP::writeMultipleRegisters(uint16_t u16WriteAddress,
  uint16_t u16WriteQty)
{
  _u16WriteAddress = u16WriteAddress;
  _u16WriteQty = u16WriteQty;
  return ModbusMasterTransaction(MBWriteMultipleRegisters);
}


/**
Modbus function 0x16 Mask Write Register.

This function code is used to modify the contents of a specified holding 
register using a combination of an AND mask, an OR mask, and the 
register's current contents. The function can be used to set or clear 
individual bits in the register.

The request specifies the holding register to be written, the data to be 
used as the AND mask, and the data to be used as the OR mask. Registers 
are addressed starting at zero.

The function's algorithm is:

Result = (Current Contents && And_Mask) || (Or_Mask && (~And_Mask))

@param u16WriteAddress address of the holding register (0x0000..0xFFFF)
@param u16AndMask AND mask (0x0000..0xFFFF)
@param u16OrMask OR mask (0x0000..0xFFFF)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t ModbusTCP::maskWriteRegister(uint16_t u16WriteAddress,
  uint16_t u16AndMask, uint16_t u16OrMask)
{
  _u16WriteAddress = u16WriteAddress;
  _u16TxRxBuffer[0] = u16AndMask;
  _u16TxRxBuffer[1] = u16OrMask;
  return ModbusMasterTransaction(MBMaskWriteRegister);
}


/**
Modbus function 0x17 Read Write Multiple Registers.

This function code performs a combination of one read operation and one 
write operation in a single MODBUS transaction. The write operation is 
performed before the read. Holding registers are addressed starting at 
zero.

The request specifies the starting address and number of holding 
registers to be read as well as the starting address, and the number of 
holding registers. The data to be written is specified in the transmit 
buffer.

@param u16ReadAddress address of the first holding register (0x0000..0xFFFF)
@param u16ReadQty quantity of holding registers to read (1..125, enforced by remote device)
@param u16WriteAddress address of the first holding register (0x0000..0xFFFF)
@param u16WriteQty quantity of holding registers to write (1..121, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t ModbusTCP::readWriteMultipleRegisters(uint16_t u16ReadAddress,
  uint16_t u16ReadQty, uint16_t u16WriteAddress, uint16_t u16WriteQty)
{
  _u16ReadAddress = u16ReadAddress;
  _u16ReadQty = u16ReadQty;
  _u16WriteAddress = u16WriteAddress;
  _u16WriteQty = u16WriteQty;
  return ModbusMasterTransaction(MBReadWriteMultipleRegisters);
}


/* _____PRIVATE FUNCTIONS____________________________________________________ */
/**
Modbus transaction engine.
Sequence:
  - assemble Modbus Request Application Data Unit (ADU),
    based on particular function called
  - transmit request over selected serial port
  - wait for/retrieve response
  - evaluate/disassemble response
  - return status (success/exception)

@param u8MBFunction Modbus function (0x01..0xFF)
@return 0 on success; exception number on failure
*/
uint8_t ModbusTCP::ModbusMasterTransaction(uint8_t u8MBFunction)
{
  uint8_t u8ModbusADU[256];
  uint8_t u8ModbusADUSize = 0;
  uint8_t i, u8Qty;
  uint16_t packetLength;
  uint32_t u32StartTime;
  uint16_t u16BytesLeft = 6;
  uint8_t u8MBStatus = MBSuccess;
  _u8ResponseBufferLength = 0;
  
  // assemble Modbus Request Application Data Unit
  u8ModbusADU[u8ModbusADUSize++] = highByte(_u16MBTransactionID);
  u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16MBTransactionID);
  u8ModbusADU[u8ModbusADUSize++] = highByte(_u16MBProtocolID);
  u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16MBProtocolID);
  u8ModbusADUSize += 2;
  packetLength = u8ModbusADUSize;
  u8ModbusADU[u8ModbusADUSize++] = _u8MBUnitID;
  u8ModbusADU[u8ModbusADUSize++] = u8MBFunction;

  switch(u8MBFunction)
  {
    case MBReadCoils:
    case MBReadDiscreteInputs:
    case MBReadInputRegisters:
    case MBReadHoldingRegisters:
    case MBReadWriteMultipleRegisters:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16ReadAddress);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16ReadAddress);
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16ReadQty);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16ReadQty);
      break;
  }
  
  switch(u8MBFunction)
  {
    case MBWriteSingleCoil:
    case MBMaskWriteRegister:
    case MBWriteMultipleCoils:
    case MBWriteSingleRegister:
    case MBWriteMultipleRegisters:
    case MBReadWriteMultipleRegisters:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16WriteAddress);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteAddress);
      break;
  }
  
  switch(u8MBFunction)
  {
    case MBWriteSingleCoil:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16WriteQty);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteQty);
      break;
      
    case MBWriteSingleRegister:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TxRxBuffer[0]);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TxRxBuffer[0]);
      break;
      
    case MBWriteMultipleCoils:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16WriteQty);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteQty);
      u8Qty = (_u16WriteQty % 8) ? ((_u16WriteQty >> 3) + 1) : (_u16WriteQty >> 3);
      u8ModbusADU[u8ModbusADUSize++] = u8Qty;
      for (i = 0; i < u8Qty; i++)
      {
        switch(i % 2)
        {
          case 0: // i is even
            u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TxRxBuffer[i >> 1]);
            break;
            
          case 1: // i is odd
            u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TxRxBuffer[i >> 1]);
            break;
        }
      }
      break;
      
    case MBWriteMultipleRegisters:
    case MBReadWriteMultipleRegisters:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16WriteQty);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteQty);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteQty << 1);
      
      for (i = 0; i < lowByte(_u16WriteQty); i++)
      {
        u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TxRxBuffer[i]);
        u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TxRxBuffer[i]);
      }
      break;
      
    case MBMaskWriteRegister:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TxRxBuffer[0]);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TxRxBuffer[0]);
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TxRxBuffer[1]);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TxRxBuffer[1]);
      break;
  }
  
  u8ModbusADU[u8ModbusADUSize] = 0;

  packetLength = u8ModbusADUSize - packetLength;
  u8ModbusADU[4] = packetLength << 8;
  u8ModbusADU[5] = packetLength;

  Serial.println(F("Check time for connection."));
  uint32_t MBconnectionTimer = millis();
#if WIZNET_W5100  
  if(!ModbusClient.connected()) {         // fOR w5100
#elif ENC28J60
  if(!MBconnectionFlag) {                 // For ENC28J60
#elif ESP8266
  if (!ModbusClient.connected()) {        // for esp8266
#endif
    Serial.print(F("Trying to connect..."));
    MBconnectionFlag = 0;
    while(MBconnectionFlag != 1)
    {
      if((millis() - MBconnectionTimer) > 3000)
      {
        ModbusClient.stop();
        return MBServerConnectionTimeOut;
      }      
      MBconnectionFlag = ModbusClient.connect(serverIP, 502);
      Serial.println("MBconnectionFlag: " + String(int(MBconnectionFlag)));  // Add further functionality here.
      delay(100);                                                                      // Read client.connect() Further.
    }
    Serial.println(F("Connected to Server!!"));
  }
  else
    Serial.println(F("Already Connected to Server!!"));

#ifdef ESP8266
  ModbusClient.write(&u8ModbusADU[0], u8ModbusADUSize);
#else
  ModbusClient.write(u8ModbusADU, u8ModbusADUSize);
#endif
  u8ModbusADUSize = 0;
  // loop until we run out of time or bytes, or an error occurs
  u32StartTime = millis();
  while (u16BytesLeft && !u8MBStatus)
  {
    if (ModbusClient.available())
    {
      u8ModbusADU[u8ModbusADUSize++] = ModbusClient.read();
      u16BytesLeft--;
    }
    else
    {
      if (_idle)
      {
        _idle();
      }
    }
    
    // evaluate Unit ID, function code once enough bytes have been read
    if (u8ModbusADUSize == 6)
    {
      uint16_t responseTransactionId = u8ModbusADU[0];
      responseTransactionId = responseTransactionId << 8;
      responseTransactionId |= u8ModbusADU[1];

      if (responseTransactionId != _u16MBTransactionID)
      {
        u8MBStatus = MBInvalidTransactionID;
        break;
      }

      if((u8ModbusADU[2] != 0) || (u8ModbusADU[3] != 0))
      {
        u8MBStatus = MBInvalidProtocol;
        break;
      }



      u16BytesLeft = u8ModbusADU[4];
      u16BytesLeft = u16BytesLeft << 8;
      u16BytesLeft |= u8ModbusADU[5];
      

      
    }
    if ((millis() - u32StartTime) > ku16MBResponseTimeout)
    {
      u8MBStatus = MBResponseTimedOut;
    }
  }
#if WIZNET_W5100  
  ModbusClient.stop();
  Serial.println("WIZNET W5100 : Stopping");
#elif ENC28J60
  Serial.println("ENC28J60 : Not Stopping");
#elif ESP8266
  ModbusClient.stop();
  Serial.println("ESP8266 : Stopping");
#endif


  if(u8ModbusADU[6] != _u8MBUnitID)
  {
    u8MBStatus = MBInvalidUnitID;      
  }
  // verify response is for correct Modbus function code (mask exception bit 7)
  if ((u8ModbusADU[7] & 0x7F) != u8MBFunction)
  {
    u8MBStatus = MBInvalidFunction;
  }
  
  // check whether Modbus exception occurred; return Modbus Exception Code
  if (bitRead(u8ModbusADU[7], 7))
  {
    u8MBStatus = u8ModbusADU[8];
  }

  // disassemble ADU into words
  if (!u8MBStatus)
  {
    // evaluate returned Modbus function code
    switch(u8ModbusADU[7])
    {
      case MBReadCoils:
      case MBReadDiscreteInputs:

        _u8ResponseBufferLength = u8ModbusADU[8] >> 1;
        // load bytes into word; response bytes are ordered L, H, L, H, ...
        for (i = 0; i < _u8ResponseBufferLength; i++)
        {
          if (i < MaxBufferSize)
          {
            _u16TxRxBuffer[i] = word(u8ModbusADU[2 * i + 10], u8ModbusADU[2 * i + 9]);
          }                  
        }
        
        // in the event of an odd number of bytes, load last byte into zero-padded word
        if (u8ModbusADU[8] % 2)
        {
          if (i < MaxBufferSize)
          {
            _u16TxRxBuffer[i] = word(0, u8ModbusADU[2 * i + 9]);
          }
          
          _u8ResponseBufferLength = i + 1;
        }
        break;
        
      case MBReadInputRegisters:
      case MBReadHoldingRegisters:
      case MBReadWriteMultipleRegisters:
        // load bytes into word; response bytes are ordered H, L, H, L, ...
        _u8ResponseBufferLength = u8ModbusADU[8] >> 1;
        for (i = 0; i < _u8ResponseBufferLength; i++)
        {
          if (i < MaxBufferSize)
          {
            _u16TxRxBuffer[i] = word(u8ModbusADU[2 * i + 9], u8ModbusADU[2 * i + 10]);
          }
        }
        break;
    }
  }

  return u8MBStatus;
}
