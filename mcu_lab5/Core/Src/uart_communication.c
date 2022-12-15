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
uint8_t buffer_flag = 0;

uint8_t try_times = 0;

uint8_t ERROR_CODE_G = NORMAL;

char str[50];

extern UART_HandleTypeDef huart1;
void reset_buffer() {
	for (int i = 0; i < MAX_BUFFER_SIZE; i++) {
		buffer[i] = 0;
	}
	index_buffer = 0;
}

void command_parser_fsm() {
	switch(index_buffer) {
		case 0:
		case 1:
		case 2:
		case 3:
			ERROR_CODE_G = CMD_NOT_EXIST;
			break;
		case 4: // user done communicate with command !OK#
			if (strcmp((const char *)buffer, userOK) == 0) {
					HAL_UART_Transmit(&huart1, (uint8_t*)str,
							sprintf(str, "%s","cmd 'OK' detected\r\n"), 1000);

					status_ACK = END_SEND;
					ERROR_CODE_G = NORMAL;
			}
			break;
		case 5: // user request command !RST#
			if (strcmp((const char *)buffer, userRequest) == 0) {
					HAL_UART_Transmit(&huart1, (uint8_t*)str,
						sprintf(str, "%s","cmd 'RST' detected\r\n"), 1000);
					ADC_value = HAL_ADC_GetValue(&hadc1);

					status_ACK = SEND_ADC;
					setTimer1(10);
					ERROR_CODE_G = NORMAL;
			}

			break;
		default:
			if (index_buffer < MAX_BUFFER_SIZE)
				ERROR_CODE_G = CMD_NOT_EXIST;
			else ERROR_CODE_G = BUFFER_IS_FULL;
			break;

	}
	error_hanlder();
	reset_buffer();
}

void uart_communication_fsm() {
	switch(status_ACK) {
		case IDE_MODE:
			break;
		case SEND_ADC:
				status_ACK = WAIT_ACK;
				HAL_UART_Transmit(&huart1, (uint8_t*)str,
					sprintf(str,"!ADC=%lu#\r\n", ADC_value), 1000);
//				printf("!ADC=%lu#\r\n", ADC_value);
				reset_buffer();
				setTimer1(TIME_OUT_ACK);
			break;
		case WAIT_ACK:
			if (timer1_flag == 1) {
				if (try_times >= MAX_TRY_TIMES) {
					startWaitingUser = false;
					HAL_UART_Transmit(&huart1, (uint8_t*)str,
						sprintf(str, "%s","MAX TRY TIMES REACHED\r\n"), 1000);
					status_ACK = END_SEND;
					break;
				} else {
					try_times++;
					HAL_UART_Transmit(&huart1, (uint8_t*)str,
						sprintf(str, "!ADC=%lu#\r\n", ADC_value), 1000);
				}

				setTimer1(TIME_OUT_ACK);
				reset_buffer();
			}
			break;
		case END_SEND:
			status_ACK = IDE_MODE;
			HAL_UART_Transmit(&huart1, (uint8_t*)str,
				sprintf(str, "%s","USER RESPONDED OR REACHED MAX TIMES TRY\r\n"), 1000);

			try_times = 0;
			reset_buffer();
			break;
	}

}

void error_hanlder() {
	switch (ERROR_CODE_G) {
		case NORMAL:
			break;
		case BUFFER_IS_FULL:
			HAL_UART_Transmit(&huart1, (uint8_t*)str,
				sprintf(str, "%s","\r\nERROR: BUFFER IS FULL\r\n"), 1000);

			break;
		case CMD_NOT_EXIST:
			HAL_UART_Transmit(&huart1, (uint8_t*)str,
				sprintf(str, "%s","\r\nERROR: COMMAND NOT EXIST\r\n"), 1000);

			break;
		default:
			break;
	}
}
