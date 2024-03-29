﻿/*
 * This is a basic command and data handling program for Arduino.
 * It interfaces with the Ball Aerospace COSMOS program
 * 
 * (c) 2018 Kevin Garner
 * kg.dev@protonmail.com or kevin@kev-dev.net
 */

//#pragma pack(1)	// forces no additional padding of data in structs; used to save memory
				// note that this does decrease efficiency in CPU time, so may be removed and only
				// applied when necessary around structs using #pragma pack (push, 1) then #pragma pack(pop)

#include <Arduino.h>
#include <stdint.h>

#include "CommandList.h"

void handleCommand();
void sendTelemetry();
void writeTelemetry(const char* pkt, byte size);

// LED pins, used for testing commands
#define LED_OUTPUT_PIN 7
#define LED_INPUT_PIN 9

// defines default command size in bytes
#define TOTAL_CMD_SIZE_IN_BYTES 4

// baud rate
#define BAUD_RATE 9600

// command data
typedef struct
{
	byte length;
	byte pktID;
	uint16_t commandCount;  //total command count
	uint16_t invalidCommandCount; // total invalid commands
} __attribute__((packed)) Pkt_CommandData;
// attribute packed guarantees that there will be no padding in memory when storing the struct

// example packet - LED state
typedef struct
{
  byte length;  /* length sent to COSMOS so COSMOS knows how much total data to expect in the packet
				 * this should be set to sizeof() each time before sending to COSMOS 
				 */
  byte pktID;   // packet ID. this must be set to the same value that COSMOS has in the config files
  // everything before this line is "stock" data needed for COSMOS - everything after this line is user data
  byte ledState;
} Pkt_LEDState;

// data packets to be sent
Pkt_LEDState ledData;  
Pkt_CommandData cmdData;

void setup() 
{
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  pinMode(LED_INPUT_PIN, INPUT);

  Serial.begin(BAUD_RATE);

// init packet IDs and starting values
  ledData.pktID = 2;

  cmdData.commandCount = 0;
  cmdData.invalidCommandCount = 0;
  cmdData.pktID = 1;
}

void loop() 
{
  handleCommand();
  delay(50);
  sendTelemetry();
  delay(50);
}

void handleCommand()
{
  byte command[TOTAL_CMD_SIZE_IN_BYTES];
  
  // if there is no command, return
  if(Serial.available() <= 0) { return; }

  int count = 0;

  // get all serial data, 1 byte at a time
  // note that commands read this way should be defined as LITTLE_ENDIAN in COSMOS
  while(Serial.available())
  {
    command[count] = Serial.read();
    ++count;
  }

  // increment number of command counts
  if(cmdData.commandCount >= UINT16_MAX) { cmdData.commandCount = 0; }
  else { ++cmdData.commandCount; }

  // take the command array and concatenate the bytes into a number we can actually use
  uint32_t cmd2execute = 0;
  // bitwise shift the bytes in the array onto the command
  cmd2execute = (command[3] << 24) | (command[2] << 16) | (command[1] << 8) | command[0];

// execute command
  if(cmd2execute == CMD_LED_ON)
  {
    digitalWrite(LED_OUTPUT_PIN, HIGH);
  }
  else if(cmd2execute == CMD_LED_OFF)
  {
    digitalWrite(LED_OUTPUT_PIN, LOW);
  }
  else if(cmd2execute == CMD_NO_OP_TEST)
  {
    // do nothing, this is a test command
  }
  else
  { // didn't match any known commands, so increment invalid command counter
	if(cmdData.invalidCommandCount >= UINT16_MAX) { cmdData.invalidCommandCount = 0; }
	else { ++cmdData.invalidCommandCount; }
  }
}

void sendTelemetry()
{
  // set LED state in LED packet
  ledData.ledState = digitalRead(LED_INPUT_PIN);

// start sending data
// first, set .length to sizeof() so COSMOS know how much data to expect
  ledData.length = sizeof(ledData);
  writeTelemetry((const char*) &ledData, sizeof(ledData));
  cmdData.length = sizeof(cmdData);
  writeTelemetry((const char*) &cmdData, sizeof(cmdData));
}

void writeTelemetry(const char* pkt, byte size)
{ // go through each byte in the packet and send it
  // COSMOS should be configured to little endian for any data larger than 1 byte
  // for data of just 1 byte it technically doesn't matter if COSMOS is little or big endian
  for(int c = 0; c < size; ++c)
  {
    Serial.write(pkt[c]);
  }
}

