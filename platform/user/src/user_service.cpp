/*
 * user_service.c
 *
 *  Created on: Sep 17, 2023
 *      Author: hoang
 */

#include "libuser.h"
#include "user_service.h"
// #include "handler_message.h"

K_MSGQ_DEFINE(queue_data_tx, sizeof(fl_data_t), 10, alignof(fl_data_t));
extern	k_msgq		queue_data_rx;

extern  const struct device *uart1;
extern ProductInfo_t product_info;

// extern uint8_t NUM_LIGHTS;
// uint8_t NUM_LIGHTS_PRE = NUM_LIGHTS;

extern char rx_buf[MSG_SIZE];
extern uint8_t rx_buf_pos;

// res_flag_t res[8];
// unsigned char get_check_sum(unsigned char *pack, unsigned short pack_len)
// {
//   unsigned short i;
//   unsigned char check_sum = 0;
  
//   for(i = 0; i < pack_len; i ++){
//     check_sum += *pack ++;
//   }
  
//   return check_sum;
// }

// bool check_sum(data_buffer_t *data);

// // int T_BDT_chechsum = 0;

// bool check_sum(data_buffer_t *data)
// {
// 	// T_BDT_chechsum++;
// 	uint8_t sum = 0;
// 	u16 length = data->datalength[1];
// //	printf("length:%d %x  %x",length, data->datalength[0], data->datalength[1]);
// 	uint8_t * ptr = (uint8_t*) data;
// 	for(int i = 0; i < length + 8; i++){
// //		printf("%x ",ptr[i]);
// 		sum += ptr[i];
// 	}
// //	printf("sum: %x  data[length]: %x\n", sum, data->data[data->datalength[1]]);
// 	if(sum == data->data[length])
// 		return 1;
// 	return 0;
// }
// int  T_BDT_hdmess = 0;

void handler_message(void)
{
// 	fl_data_t data;
// 	// check if queue is empty
// 	if(k_msgq_num_used_get(&queue_data_rx) == 0){
// 		// print_uart("queue empty\n");
// 		return;
// 	} else {
// 		k_msgq_get(&queue_data_rx, &data, K_NO_WAIT);
// 		uart_send((uint8_t*)&data, data.datalength[1] + 9);
// 	}
// 	// k_msgq_get(&queue_data_rx, &data, K_NO_WAIT);
// 	if(data.header[HEAD_FIRST] != FIRST_FRAME_HEAD || data.header[HEAD_SECOND] != SECOND_FRAME_HEAD ){
// 		print_uart("header_fail\n");
// 		return;
// 	}
// 	if(data.version != SERIAL_PROTOCOL_VER){
// 		print_uart("version_fail\n");
// 		return;
// 	}

// 	if(!check_sum(&data)){
// 		return;
// 	}
// //	printf("data_success\n");
// //	drv_uart_tx_start((uint8_t*)&data, data.datalength[1] +9);
// 	uint8_t cmd_word = data.cmdword;
// //	printf("cmd word: %x\n", cmd_word);
// 	switch(cmd_word)
// 	{
// 	case ZIGBEE_FACTORY_NEW_CMD:
// //		zb_factoryReset();
// 		break;
// 	case PRODUCT_INFO_CMD:
// 		print_uart("get product info\n");
// 		get_product_info(&data);
// 		break;
// 	case ZIGBEE_STATE_CMD:
// //		report_network_satus();
// 		break;
// 	case ZIGBEE_CFG_CMD:
// 		uart_write_frame(ZIGBEE_CFG_CMD, 0, 0);
// 		config_network(&data);
// 		break;
// 	case ZIGBEE_DATA_REQ_CMD:
// 		break;
// 	case DATA_DATA_RES_CMD:
// 		break;
// 	case DATA_REPORT_CMD:
// 		handler_status_device(&data);
// 		break;
// 	case RF_IBEACON_MESSAGE:
// 		break;
// 	default:
// 		break;
// 	}
}


// uint8_t flag_1 = 0;

// s32 check_ACK(void){
// 	uint8_t check =0;
// 	for(uint8_t i = 0; i < 8; i++){
// 		if(res[i].flag == 1){
// 			ring_buffer_queue(&ring_buffer_tx, res[i].data);
// 			// printf("this is send again packet %d missing\n", i);
// 			check++;
// 		}
// 	}
// 	if(check)
// 		return 0;
// 	else
// 	{
// 		flag_1 = 0;
// 		return -1;
// 	}
// }


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