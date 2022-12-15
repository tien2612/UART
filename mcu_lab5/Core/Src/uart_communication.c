/*
 * uart_communication.c
 *
 *  Created on: Dec 1, 2022
 *      Author: nguye
 */


#include "uart_communication.h"

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
extern ADC_HandleTypeDef hadc1;

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
			ERROR_CODE_G = CMD_NOT_FOUND;
			status_ACK = ERROR_G;
			break;
		case 4: // user done communicate with command !OK#
			if (strcmp((const char *)buffer, userOK) == 0) {
					if (status_ACK != WAIT_ACK) {
						ERROR_CODE_G = REQUEST_DATA_FIRST;
						status_ACK = ERROR_G;
						break;
					}
//					HAL_UART_Transmit(&huart1, (uint8_t*)str,
//							sprintf(str, "%s","\r\nCMD 'OK' DETECTED\r\n"), 1000);

					status_ACK = END_SEND;
					ERROR_CODE_G = NORMAL;
			} else {
				status_ACK = ERROR_G;
				ERROR_CODE_G = CMD_NOT_FOUND;
			}
			break;
		case 5: // user request command !RST#
			if (strcmp((const char *)buffer, userRequest) == 0) {

//					HAL_UART_Transmit(&huart1, (uint8_t*)str,
//						sprintf(str, "%s","\r\nCMD 'RST' DETECTED\r\n"), 1000);
					ADC_value = HAL_ADC_GetValue(&hadc1);

					status_ACK = SEND_ADC;
					setTimer1(10);
					ERROR_CODE_G = NORMAL;
			} else {
				status_ACK = ERROR_G;
				ERROR_CODE_G = CMD_NOT_FOUND;
			}
			break;
		default:
			if (index_buffer < MAX_BUFFER_SIZE)
				ERROR_CODE_G = CMD_NOT_FOUND;
			else
				ERROR_CODE_G = BUFFER_IS_FULL;

			status_ACK = ERROR_G;
			break;

	}

	reset_buffer();
}

void uart_communication_fsm() {
	switch(status_ACK) {
		case SLEEP_MODE:
			System_Go_To_Sleep();
			break;
		case SEND_ADC:
				HAL_UART_Transmit(&huart1, (uint8_t*)str,
					sprintf(str,"!ADC=%lu#\r\n", ADC_value), 1000);

				reset_buffer();
				setTimer1(TIME_OUT_ACK);
				status_ACK = WAIT_ACK;
			break;
		case WAIT_ACK:
			if (timer1_flag == 1) {
				try_times++;
				HAL_UART_Transmit(&huart1, (uint8_t*)str,
					sprintf(str, "!ADC=%lu#\r\n", ADC_value), 1000);
				if (try_times >= MAX_TRY_TIMES) {
					HAL_UART_Transmit(&huart1, (uint8_t*)str,
						sprintf(str, "%s","\r\nREACHED MAXIMUM TRY TIMES\r\n"), 1000);

					status_ACK = END_SEND;
					return;
				} else setTimer1(TIME_OUT_ACK);
				reset_buffer();
			}
			break;
		case END_SEND:
			HAL_UART_Transmit(&huart1, (uint8_t*)str,
				sprintf(str, "%s","\r\nSTOP SENDING DATA\r\n"), 1000);

			try_times = 0;
			reset_buffer();
			status_ACK = SLEEP_MODE;
			break;
		case ERROR_G:
			error_hanlder();
			status_ACK = SLEEP_MODE;
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
			ERROR_CODE_G = NORMAL;
			break;
		case CMD_NOT_FOUND:
			HAL_UART_Transmit(&huart1, (uint8_t*)str,
				sprintf(str, "%s","\r\nERROR: COMMAND NOT FOUND\r\n"), 1000);
			ERROR_CODE_G = NORMAL;
			break;

		case REQUEST_DATA_FIRST:
			HAL_UART_Transmit(&huart1, (uint8_t*)str,
				sprintf(str, "%s","\r\nERROR: DATA MUST BE REQUESTED FIRST\r\n"), 1000);
			ERROR_CODE_G = NORMAL;
			break;
		default:
			break;
	}
}

void System_Go_To_Sleep() {
	HAL_SuspendTick();
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	HAL_ResumeTick();
}
