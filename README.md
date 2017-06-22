README
============

Overview
--------
This is an Arduino library for communicating with Modbus server over Ethernet (via TCP protocol).


Features
--------
The following Modbus functions have been implemented:

Discrete Coils/Flags

  * 0x01 - Read Coils
  * 0x02 - Read Discrete Inputs
  * 0x05 - Write Single Coil
  * 0x0F - Write Multiple Coils

Registers

  * 0x03 - Read Holding Registers
  * 0x04 - Read Input Registers
  * 0x06 - Write Single Register
  * 0x10 - Write Multiple Registers
  * 0x16 - Mask Write Register
  * 0x17 - Read Write Multiple Registers




Hardware
--------
This library has been tested with an Arduino [Mega](https://www.arduino.cc/en/Main/ArduinoBoardMega) with following Compatible Ethernet ICs.

1. Wizent W5100 - [Ethernet library](https://www.arduino.cc/en/Reference/Ethernet).
2. ENC28J60 - [UIPEthernet library](https://github.com/UIPEthernet/UIPEthernet).

