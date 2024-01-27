/*
 * user_service.c
 *
 *  Created on: Sep 17, 2023
 *      Author: hoang
 */

#include "libuser.h"
#include "user_service.h"
// #include "handler_message.h"

K_MSGQ_DEFINE(queue_data_tx, sizeof(fl_data_t), 16, alignof(fl_data_t));
extern	k_msgq		queue_data_rx;

extern  const struct device *uart1;
extern ProductInfo_t product_info;

// extern uint8_t NUM_LIGHTS;
// uint8_t NUM_LIGHTS_PRE = NUM_LIGHTS;

extern char rx_buf[MSG_SIZE];
extern uint8_t rx_buf_pos;


void parking_message(void)
{
	if(k_msgq_num_used_get(&queue_data_tx) == 0){
		return -1;
	}
	fl_data_t data;
	k_msgq_get(&queue_data_tx, &data, K_NO_WAIT);
	
	uart_send((uint8_t*)&data, data.datalength[1] + 9);
	return 0;
}

void service_handler (void)
{
	rf_uart_service();
}