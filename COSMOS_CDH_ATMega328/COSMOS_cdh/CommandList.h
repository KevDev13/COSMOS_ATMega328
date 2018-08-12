/*
 * CommandList.h
 * 
 * Contains a list of all possible commands and their hex values
 *
 * Created: 8/12/2018 12:20:02 PM
 * Author: Kevin Garner, kg.dev@protonmail.com or kevin@kev-dev.net
 */ 


#ifndef COMMANDLIST_H_
#define COMMANDLIST_H_

#define CMD_NO_OP_TEST 0x00	// test command, does nothing

#define CMD_LED_ON 0x01	// command LED on pin LED_OUTPUT_PIN to ON
#define CMD_LED_OFF 0x02	//command LED on pin LED_OUTPUT_PIN to OFF

#endif /* COMMANDLIST_H_ */