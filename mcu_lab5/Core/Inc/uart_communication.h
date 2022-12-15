/*
 * uart_communication.h
 *
 *  Created on: Dec 1, 2022
 *      Author: nguye
 */
#include "main.h"
#include "string.h"
#include "stdio.h"
#include "software_timer.h"
#include "stdbool.h"
#include "global.h"

#ifndef INC_UART_COMMUNICATION_H_
#define INC_UART_COMMUNICATION_H_

#define IDE_MODE					1
#define	SEND_ADC 					2
#define WAIT_ACK					3
#define END_SEND 					4
#define TIME_OUT_ACK				3000 // wait for user to respone within 3s, if not send data again
#define MAX_TRY_TIMES				5

#define NORMAL						9
#define BUFFER_IS_FULL				10
#define CMD_NOT_EXIST				11

#define MAX_BUFFER_SIZE 30
extern uint8_t temp;
extern uint8_t buffer [MAX_BUFFER_SIZE];
extern uint8_t buffer_flag;

extern uint8_t ERROR_CODE_G;

extern int status_ACK;
extern const char userRequest[];
extern const char userOK[];

void command_parser_fsm();

void uart_communication_fsm();

void error_hanlder();

#endif /* INC_UART_COMMUNICATION_H_ */
