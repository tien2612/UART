/*
 * uart_communication.c
 *
 *  Created on: Dec 1, 2022
 *      Author: nguye
 */


#include "uart_communication.h"

extern ADC_HandleTypeDef hadc1;

int status_ACK = 0;
bool startWaitingUser = false;

const char userRequest[] = "!RST#";
const char userOK[] = "!OK#";

uint8_t temp = 0;
uint8_t buffer [MAX_BUFFER_SIZE] = {0};
uint8_t index_buffer = 0;
uint8_t buffer_flag = 0;
uint32_t ADC_value = 0;

uint8_t try_times = 0;

uint8_t ERROR_CODE_G = NORMAL;
void reset_buffer() {
	for (int i = 0; i < MAX_BUFFER_SIZE; i++) {
		buffer[i] = 0;
	}
	index_buffer = 0;
}
void command_parser_fsm() {
	for (int i = 0; i < index_buffer; i++) printf("%d\r\n", buffer[i]);

	switch(index_buffer) {
		case 0:
		case 1:
		case 2:
		case 3:
			ERROR_CODE_G = CMD_NOT_EXIST;
			break;
		case 4: // user done communicate with command !OK#
			if (strcmp((const char *)buffer, userOK) == 0) {
					printf("\r\nOk\r\n");
					status_ACK = END_SEND;
					ERROR_CODE_G = NORMAL;
			}
			break;
		case 5: // user request command !RST#
			if (strcmp((const char *)buffer, userRequest) == 0) {
					printf("\r\nRequesting\r\n");
					status_ACK = SEND_ADC;
					ADC_value = HAL_ADC_GetValue(&hadc1);
					setTimer1(10);
					ERROR_CODE_G = NORMAL;
			}

			break;
		default:
			ERROR_CODE_G = CMD_NOT_EXIST;
			break;

	}
	error_notify();
	reset_buffer();
}

void uart_communication_fsm() {
	switch(status_ACK) {
		case IDE_MODE:
			break;
		case SEND_ADC:
				status_ACK = WAIT_ACK;

				printf("!ADC=%lu#\r\n", ADC_value);
				reset_buffer();
			break;
		case WAIT_ACK:
			if (timer1_flag == 1) {
				if (try_times >= MAX_TRY_TIMES) {
					startWaitingUser = false;
					printf("MAX TRY TIMES REACHED\r\n");
					status_ACK = END_SEND;
					break;
				} else {
					try_times++;
					printf("!ADC=%ld#\r\n", ADC_value);
				}

				setTimer1(TIME_OUT_ACK);
				reset_buffer();
			}
			break;
		case END_SEND:
			status_ACK = IDE_MODE;
			printf("USER RESPONE\r\n");
			try_times = 0;
			reset_buffer();
			break;
	}

	System_Go_To_Sleep();
}

void error_notify() {
	switch (ERROR_CODE_G) {
		case NORMAL:
			break;
		case BUFFER_IS_FULL:
			printf("ERROR: BUFFER IS FULL\r\n");
			break;
		case CMD_NOT_EXIST:
			printf("ERROR: COMMAND NOT EXIST\r\n");
			break;
		default:
			break;
	}
}

void System_Go_To_Sleep() {

}
