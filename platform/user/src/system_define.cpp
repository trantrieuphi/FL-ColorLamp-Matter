/**
* @file  system.c
* @brief this file contains frame analysis of communication between MCU and rf 
* and construct these functions which used send data in a frame format
* @author qinlang
* @date 2022.05.06
* @par email:
* @par email:qinlang.chen@tuya.com
* @copyright HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
* @par company
* http://www.tuya.com
*/


#define SYSTEM_GLOBAL
// #include "main.h"
#include "zigbee_define.h"
#include "protocol_define.h"
#include "system_define.h"
// #include "battery.h"
#include "stdlib.h"
#include <cstdint>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <string.h>
#include "fl.h"

#define TX_NUMBER_BUFFER	5
DOWNLOAD_CMD_S download_cmd[] =
{
	//ch1:
  {DPID_SWITCH_LED_1 + 0*16, DP_TYPE_BOOL},
  {DPID_BRIGHT_VALUE_1 + 0*16, DP_TYPE_VALUE},
  {DPID_BRIGHTNESS_MIN_1+ 0*16, DP_TYPE_VALUE},
  {DPID_LED_TYPE_1+ 0*16, DP_TYPE_ENUM},
  {DPID_BRIGHTNESS_MAX_1+ 0*16, DP_TYPE_VALUE},
  {DPID_TRANSACTION_1+ 0*16, DP_TYPE_VALUE}, 
	{DPID_LIGHT_MODE_1 + 0*16, DP_TYPE_ENUM},
	{DPID_DEVICE_STATUS_1 + 0*16, DP_TYPE_VALUE},
	{DPID_CCT_OR_HSL_1 + 0*16, DP_TYPE_RAW},
	{DPID_TERMP_OR_HUE_1 + 0*16, DP_TYPE_VALUE},
	{DPID_SATURATION_1 + 0*16, DP_TYPE_VALUE},
	
	//ch2:
  {DPID_SWITCH_LED_1 + 01*16, DP_TYPE_BOOL},
  {DPID_BRIGHT_VALUE_1 + 01*16, DP_TYPE_VALUE},
  {DPID_BRIGHTNESS_MIN_1+ 01*16, DP_TYPE_VALUE},
  {DPID_LED_TYPE_1+ 01*16, DP_TYPE_ENUM},
  {DPID_BRIGHTNESS_MAX_1+ 01*16, DP_TYPE_VALUE},
  {DPID_TRANSACTION_1+ 01*16, DP_TYPE_VALUE},
	{DPID_LIGHT_MODE_1 + 01*16, DP_TYPE_ENUM},
	{DPID_DEVICE_STATUS_1 + 01*16, DP_TYPE_VALUE},
	{DPID_CCT_OR_HSL_1 + 01*16, DP_TYPE_RAW},
	{DPID_TERMP_OR_HUE_1 + 01*16, DP_TYPE_VALUE},
	{DPID_SATURATION_1 + 01*16, DP_TYPE_VALUE},
	
	//ch3:
  {DPID_SWITCH_LED_1 + 02*16, DP_TYPE_BOOL},
  {DPID_BRIGHT_VALUE_1 + 02*16, DP_TYPE_VALUE},
  {DPID_BRIGHTNESS_MIN_1+ 02*16, DP_TYPE_VALUE},
  {DPID_LED_TYPE_1+ 02*16, DP_TYPE_ENUM},
  {DPID_BRIGHTNESS_MAX_1+ 02*16, DP_TYPE_VALUE},
  {DPID_TRANSACTION_1+ 02*16, DP_TYPE_VALUE},
	{DPID_LIGHT_MODE_1 + 02*16, DP_TYPE_ENUM},
	{DPID_DEVICE_STATUS_1 + 02*16, DP_TYPE_VALUE},
	{DPID_CCT_OR_HSL_1 + 02*16, DP_TYPE_RAW},
	{DPID_TERMP_OR_HUE_1 + 02*16, DP_TYPE_VALUE},
	{DPID_SATURATION_1 + 02*16, DP_TYPE_VALUE},
	
	//ch4:
  {DPID_SWITCH_LED_1 + 03*16, DP_TYPE_BOOL},
  {DPID_BRIGHT_VALUE_1 + 03*16, DP_TYPE_VALUE},
  {DPID_BRIGHTNESS_MIN_1+ 03*16, DP_TYPE_VALUE},
  {DPID_LED_TYPE_1+ 03*16, DP_TYPE_ENUM},
  {DPID_BRIGHTNESS_MAX_1+ 03*16, DP_TYPE_VALUE},
  {DPID_TRANSACTION_1+ 03*16, DP_TYPE_VALUE},
	{DPID_LIGHT_MODE_1 + 03*16, DP_TYPE_ENUM},
	{DPID_DEVICE_STATUS_1 + 03*16, DP_TYPE_VALUE},
	{DPID_CCT_OR_HSL_1 + 03*16, DP_TYPE_RAW},
	{DPID_TERMP_OR_HUE_1 + 03*16, DP_TYPE_VALUE},
	{DPID_SATURATION_1 + 03*16, DP_TYPE_VALUE},
	
	//ch5:
  {DPID_SWITCH_LED_1 + 04*16, DP_TYPE_BOOL},
  {DPID_BRIGHT_VALUE_1 + 04*16, DP_TYPE_VALUE},
  {DPID_BRIGHTNESS_MIN_1+ 04*16, DP_TYPE_VALUE},
  {DPID_LED_TYPE_1+ 04*16, DP_TYPE_ENUM},
  {DPID_BRIGHTNESS_MAX_1+ 04*16, DP_TYPE_VALUE},
  {DPID_TRANSACTION_1+ 04*16, DP_TYPE_VALUE},
	{DPID_LIGHT_MODE_1 + 04*16, DP_TYPE_ENUM},
	{DPID_DEVICE_STATUS_1 + 4*16, DP_TYPE_VALUE},
	{DPID_CCT_OR_HSL_1 + 04*16, DP_TYPE_RAW},
	{DPID_TERMP_OR_HUE_1 + 04*16, DP_TYPE_VALUE},
	{DPID_SATURATION_1 + 04*16, DP_TYPE_VALUE},
	
	
	//ch6:
  {DPID_SWITCH_LED_1 + 05*16, DP_TYPE_BOOL},
  {DPID_BRIGHT_VALUE_1 + 05*16, DP_TYPE_VALUE},
  {DPID_BRIGHTNESS_MIN_1+ 05*16, DP_TYPE_VALUE},
  {DPID_LED_TYPE_1+ 05*16, DP_TYPE_ENUM},
  {DPID_BRIGHTNESS_MAX_1+ 05*16, DP_TYPE_VALUE},
  {DPID_TRANSACTION_1+ 05*16, DP_TYPE_VALUE}, 
	{DPID_LIGHT_MODE_1 + 05*16, DP_TYPE_ENUM},
	{DPID_DEVICE_STATUS_1 + 05*16, DP_TYPE_VALUE},
	{DPID_CCT_OR_HSL_1 + 05*16, DP_TYPE_RAW},
	{DPID_TERMP_OR_HUE_1 + 05*16, DP_TYPE_VALUE},
	{DPID_SATURATION_1 + 05*16, DP_TYPE_VALUE},
	
	//ch6:
  {DPID_SWITCH_LED_1 + 06*16, DP_TYPE_BOOL},
  {DPID_BRIGHT_VALUE_1 + 06*16, DP_TYPE_VALUE},
  {DPID_BRIGHTNESS_MIN_1+ 06*16, DP_TYPE_VALUE},
  {DPID_LED_TYPE_1+ 06*16, DP_TYPE_ENUM},
  {DPID_BRIGHTNESS_MAX_1+ 06*16, DP_TYPE_VALUE},
  {DPID_TRANSACTION_1+ 06*16, DP_TYPE_VALUE}, 
	{DPID_LIGHT_MODE_1 + 06*16, DP_TYPE_ENUM},
	{DPID_DEVICE_STATUS_1 + 06*16, DP_TYPE_VALUE},
	{DPID_CCT_OR_HSL_1 + 06*16, DP_TYPE_RAW},
	{DPID_TERMP_OR_HUE_1 + 06*16, DP_TYPE_VALUE},
	{DPID_SATURATION_1 + 06*16, DP_TYPE_VALUE},
	
	//ch7:
  {DPID_SWITCH_LED_1 + 07*16, DP_TYPE_BOOL},
  {DPID_BRIGHT_VALUE_1 + 07*16, DP_TYPE_VALUE},
  {DPID_BRIGHTNESS_MIN_1+ 07*16, DP_TYPE_VALUE},
  {DPID_LED_TYPE_1+ 07*16, DP_TYPE_ENUM},
  {DPID_BRIGHTNESS_MAX_1+ 07*16, DP_TYPE_VALUE},
  {DPID_TRANSACTION_1+ 07*16, DP_TYPE_VALUE}, 
	{DPID_LIGHT_MODE_1 + 07*16, DP_TYPE_ENUM},
	{DPID_DEVICE_STATUS_1 + 07*16, DP_TYPE_VALUE},
	{DPID_CCT_OR_HSL_1 + 07*16, DP_TYPE_RAW},
	{DPID_TERMP_OR_HUE_1 + 07*16, DP_TYPE_VALUE},
	{DPID_SATURATION_1 + 07*16, DP_TYPE_VALUE},
	
	
	
	
	
	
  {DPID_RESTART_STATUS, DP_TYPE_ENUM},  //trạng thái on/off hoặc nhớ của thiêt bị sau khi khởi động nguon
  
};
unsigned char cmd_total = sizeof(download_cmd) / sizeof(download_cmd[0]);
TX_Buffer_Maneger_Struct txBufferManeger[TX_NUMBER_BUFFER];

extern k_msgq queue_data_tx;

#ifdef SUPPORT_MCU_RTC_CHECK
const unsigned char mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};	
#endif

#ifdef CHECK_MCU_TYPE
MCU_TYPE_E mcu_type = MCU_TYPE_DC_POWER;   
#endif


static volatile unsigned short global_seq_num;

#ifdef CHECK_MCU_TYPE
static void response_mcu_type(void);
#endif


#ifdef SET_ZIGBEE_NWK_PARAMETER
nwk_parameter_t  nwk_paremeter;
#endif



#ifdef SET_ZIGBEE_NWK_PARAMETER
void init_nwk_paremeter(void)
{
	#error "please call this fuction in main init"
	nwk_paremeter.fast_poll_period = 0xfffe;
	nwk_paremeter.heart_period = 0xfffe;
	nwk_paremeter.join_nwk_time = 0xfffe;
	nwk_paremeter.rejion_period = 0xfffe;
	nwk_paremeter.poll_period = 0xfffe;
	nwk_paremeter.fast_poll_period = 0xfffe;
	nwk_paremeter.poll_failure_times = 0xfe;
	nwk_paremeter.rejoin_try_times = 0xfe;
	nwk_paremeter.app_trigger_rejoin = 0xfe;
	nwk_paremeter.rf_send_power = 0xfe;
}
#endif

/**
* @brief get frame seq_num
* @param[in] {void}
* @return seq_num  
*/
static unsigned short seq_num_get(void)
{
	global_seq_num ++;
	if(global_seq_num > 0xfff0){
		global_seq_num = 1;
	}
	return global_seq_num;
}

/**
* @brief padding a byte in send buf base on dest
* @param[in] {dest} the location of padding byte 
* @param[in] {byte} padding byte 
* @return  sum of frame after this operation 
*/
unsigned short set_rf_uart_byte(unsigned short dest, unsigned char byte)
{
  unsigned char *obj = (unsigned char *)rf_uart_tx_buf + DATA_START + dest;
	
  *obj = byte;
  dest += 1;
  
  return dest;
}

/**
* @brief padding buf in send buf base on dest
* @param[in] {dest} the location of padding 
* @param[in] {src}  the head address of padding buf
* @param[in] {len}  the length of padding buf
* @return  sum of frame after this operation 
*/
unsigned short set_rf_uart_buffer(unsigned short dest, unsigned char *src, unsigned short len)
{
  unsigned char *obj = (unsigned char *)rf_uart_tx_buf + DATA_START + dest;
  
  my_memcpy(obj,src,len);
  
  dest += len;
  return dest;
}

/**
* @brief send len bytes data
* @param[in] {in} the head address of send data
* @param[in] {len}  the length of send data
* @return  void
*/

void rf_uart_write_data(unsigned char *in, unsigned short len)
{
	static unsigned int txSequency =0;
	// char i;
	
  if((NULL == in) || (0 == len)){
    return;
  }
  
	if(++txSequency==0){
		txSequency = 1;
	}
	// store in to queue_data_tx
	fl_data_t data;
	memcpy((uint8_t*)&data, in, len);
	k_msgq_put(&queue_data_tx,in,K_NO_WAIT);

	
}

void rf_uart_tx_service(void){
	// static uint32_t fnTick=0;
	// static unsigned int cnTx=1;
	// unsigned char i;
		
	// // if(fnTick <= HAL_GetTick()){
	// // 	fnTick = HAL_GetTick() + 10;	//with baudra  56000  <=>  56 byte/10ms, we just need send maximum 50bytes:
	// // }
	// // else{
	// // 	return ;
	// // }
	
	// //check data ready for tx:
	// for(i=0;i< TX_NUMBER_BUFFER;  i++){
	// 	if(txBufferManeger[i].txSequency!=0 && txBufferManeger[i].txSequency  == cnTx){
	// 		//Set DMA to transmit:
	// 		if(HAL_OK == HAL_UART_Transmit_DMA(&huart1,(uint8_t*)txBufferManeger[i].txBuffer,txBufferManeger[i].length)){
	// 			txBufferManeger[i].txSequency = 0;
	// 			if(++cnTx==0){
	// 				cnTx =1;
	// 			}
	// 			break;
	// 		}
	// 		else{
	// 			deviceInfo.cnErrorUartTx  +=1;
	// 		}
	// 	}
	// }
	
}

/**
* @brief calculate the sum of the array
* @param[in] {pack} the head address of array
* @param[in] {pack_len}  the length of  array
* @return  sum
*/
unsigned char get_check_sum(unsigned char *pack, unsigned short pack_len)
{
  unsigned short i;
  unsigned char check_sum = 0;
  
  for(i = 0; i < pack_len; i ++){
    check_sum += *pack ++;
  }
  
  return check_sum;
}

/**
* @brief send a frame data 
* @param[in] {fr_cmd} frame cmd id
* @param[in] {len}    len of frame data 
* @return  void
*/
void rf_uart_write_frame(unsigned char fr_cmd, unsigned short len)
{
  unsigned char check_sum = 0;	
  unsigned short  seq;
	
  rf_uart_tx_buf[HEAD_FIRST] = FIRST_FRAME_HEAD;
  rf_uart_tx_buf[HEAD_SECOND] = SECOND_FRAME_HEAD;
  rf_uart_tx_buf[PROTOCOL_VERSION] = SERIAL_PROTOCOL_VER;
	
 
  seq = seq_num_get();
  rf_uart_tx_buf[SEQ_HIGH] = (seq << 8);
  rf_uart_tx_buf[SEQ_LOW] = (seq & 0xff); 

	
  rf_uart_tx_buf[FRAME_TYPE] = fr_cmd;
  rf_uart_tx_buf[LENGTH_HIGH] = len >> 8;
  rf_uart_tx_buf[LENGTH_LOW] = len & 0xff;
  
  len += PROTOCOL_HEAD;
	
  check_sum = get_check_sum((unsigned char *)rf_uart_tx_buf, len-1);
  rf_uart_tx_buf[len -1] = check_sum;
  
  rf_uart_write_data((unsigned char *)rf_uart_tx_buf, len);
}

/**
* @brief send product info and mcu version 
* @param[in] {void} 
* @return  void
*/
static void product_info_update(void)
{
  unsigned short length = 0;
	char strProductCh[5];
	char strIBeaconRepeat[5]; 
	
	// sprintf(strProductCh,"%d",NUMBER_CHANNEL_IN_DEVICE);
	// sprintf(strIBeaconRepeat,"%d",IBEACON_MESSAGE_REPEART_NUMBER);
	
  length = set_rf_uart_buffer(length, (unsigned char *)"{\"p\":\"", my_strlen((unsigned char *)"{\"p\":\""));
  length = set_rf_uart_buffer(length,(unsigned char *)PRODUCT_KEY,my_strlen((unsigned char *)PRODUCT_KEY));
  length = set_rf_uart_buffer(length, (unsigned char *)"\",\"v\":\"", my_strlen((unsigned char *)"\",\"v\":\""));
  length = set_rf_uart_buffer(length,(unsigned char *)MCU_VER,my_strlen((unsigned char *)MCU_VER));
	length = set_rf_uart_buffer(length, (unsigned char *)"\",\"c\":\"", my_strlen((unsigned char *)"\",\"c\":\""));
  length = set_rf_uart_buffer(length,(unsigned char *)strProductCh,my_strlen((unsigned char *)strProductCh));
	length = set_rf_uart_buffer(length, (unsigned char *)"\",\"r\":\"", my_strlen((unsigned char *)"\",\"r\":\""));
  length = set_rf_uart_buffer(length,(unsigned char *)strIBeaconRepeat,my_strlen((unsigned char *)strIBeaconRepeat));
	
  length = set_rf_uart_buffer(length, (unsigned char *)"\"}", my_strlen((unsigned char *)"\"}"));
	
	
  rf_uart_write_frame(PRODUCT_INFO_CMD, length);
}

/**
* @brief get the serial number of dpid in dp array  
* @param[in] {dpid} dp id 
* @return  serial number of dp 
*/
unsigned char get_dowmload_dpid_index(unsigned char dpid)
{
  unsigned char index;
  unsigned char total = get_download_cmd_total();
  
  for(index = 0; index < total; index ++){
    if(download_cmd[index].dp_id == dpid){
      break;
    }
  }
  return index;
}

/**
* @brief handle received dp data from rf module
* @param[in] {value} dp data
* @return  result of handle
*/

  
static unsigned char rf_data_point_handle(const unsigned char value[])
{

  unsigned char dp_id,index;
  unsigned char dp_type;
  unsigned char ret;
  unsigned short dp_len;
	
  dp_id = value[0];
  dp_type = value[1];
  dp_len = value[2] * 0x100;
  dp_len += value[3];
  

  index = get_dowmload_dpid_index(dp_id);

  if(dp_type != download_cmd[index].dp_type){
    return FALSE;
  }
  else{
    ret = dp_download_handle(dp_id,value + 4,dp_len);
  }
  
  return ret;
}

static unsigned char rf_data_respond_handle(const unsigned char value[])
{

  unsigned char dp_id,index;
  unsigned char dp_type;
  unsigned char ret;
  unsigned short dp_len;
	
  dp_id = value[0];
  dp_type = value[1];
  dp_len = value[2] * 0x100;
  dp_len += value[3];
  

  index = get_dowmload_dpid_index(dp_id);

  if(dp_type != download_cmd[index].dp_type){
    return FALSE;
  }
  else{
    ret = dp_respond_handle(dp_id,value + 4,dp_len);
  }
  
  return ret;
}


uint8_t handler_status_device(fl_data_t *data)
{
//	printf("handler_status_device\n");

	  uint8_t dp_id, index;
	  uint8_t dp_type;
	  uint8_t ret;
	  unsigned short dp_len;

	  dp_id = data->data[0];
	  dp_type = data->data[1];
	  dp_len = data->data[2] * 0x100;
	  dp_len += data->data[3];


	  index = get_dowmload_dpid_index(dp_id);
	//   idx_flag = get_index_flag(index);

	//   if(res[idx_flag].flag == 1){
	// 	  res[idx_flag].flag = 0;

	//   }

	  if(dp_type != download_cmd[index].dp_type){
	    return FALSE;
	  }
	  else{
	    ret = dp_handle_device(dp_id,data->data + 4,dp_len);
	  }

//	  if(ret)
//	  {
//		  status_rp();
////		  res
//	  }

	  return ret;


}


/**
* @brief handle received frame from rf module baseon frame cmd id
* @param[in] {void}
* @return  result of handle
*/
int data_handle(unsigned short offset)
{
	unsigned char cmd_type = 0;
	unsigned short total_len = 0, seq_num = 0;
	unsigned short dp_len;  
	unsigned char ret;
 	unsigned short i;
	
  	cmd_type  = rf_uart_rx_buf[offset + FRAME_TYPE];

	total_len  = rf_uart_rx_buf[offset + LENGTH_HIGH] * 0x100;
	total_len += rf_uart_rx_buf[offset + LENGTH_LOW];
		
	seq_num = rf_uart_rx_buf[offset + SEQ_HIGH] << 8;
  	seq_num += rf_uart_rx_buf[offset + SEQ_LOW];
	fl_data_t data;
	memcpy((uint8_t*)&data, (uint8_t*)&rf_uart_rx_buf[offset], total_len + 8);
	switch(cmd_type)
	{
	  	case ZIGBEE_FACTORY_NEW_CMD:{
		    rf_notify_factory_new_hanlde(); // response
		}
		break;

		case PRODUCT_INFO_CMD:{                                		
			get_product_info(&data);
		}
		break;
		 
		case ZIGBEE_STATE_CMD:{    
			// unsigned char current_state = rf_uart_rx_buf[offset + DATA_START];                         	
			// rf_work_state_event(current_state);
		}
		break;
		
		case ZIGBEE_CFG_CMD:{																		
			mcu_reset_rf_event(rf_uart_rx_buf[offset + DATA_START]);
		}
		break;

		case ZIGBEE_DATA_REQ_CMD:{                               
			for(i = 0;i < total_len;){
				dp_len = rf_uart_rx_buf[offset + DATA_START + i + 2] * 0x100;
				dp_len += rf_uart_rx_buf[offset + DATA_START + i + 3];
				ret = rf_data_point_handle((unsigned char *)rf_uart_rx_buf + offset + DATA_START + i);
					
				if(SUCCESS == ret){
						
				}
				else{
						
				}      
				i += (dp_len + 4);	
			}	
		}		
		break;
		case DATA_DATA_RES_CMD:{
			for(i = 0;i < total_len;){
				dp_len = rf_uart_rx_buf[offset + DATA_START + i + 2] * 0x100;
				dp_len += rf_uart_rx_buf[offset + DATA_START + i + 3];
				ret = rf_data_respond_handle((unsigned char *)rf_uart_rx_buf + offset + DATA_START + i);
					
				if(SUCCESS == ret){
						
				}
				else{
						
				}      
				i += (dp_len + 4);	
			}	
		}	
		case DATA_REPORT_CMD:{
			handler_status_device(&data);
		}	
		break;	

		case QUERY_KEY_INFO_CMD:{

		}
		break;

		case CALL_SCENE_CMD:{

		}
		break;

		case ZIGBEE_RF_TEST_CMD:{
           
		}
		break;
		case RF_IBEACON_MESSAGE: //iBeacon message get here:
			// rf_data_ibeacon_handle((unsigned char *)rf_uart_rx_buf +  DATA_START ); //xu ly iBeacon here:
			break;
		
		case MCU_OTA_VERSION_CMD:{
			response_mcu_ota_version_event();				
		}
		break;
#ifdef SUPPORT_MCU_OTA		
		case MCU_OTA_NOTIFY_CMD:{
			response_mcu_ota_notify_event(offset);
		}
		break;
		
		case MCU_OTA_DATA_REQUEST_CMD:{
			mcu_ota_fw_request_event(offset);
        }
		break;
		
		case MCU_OTA_RESULT_CMD:{
			mcu_ota_result_event(offset);
		}
		break;
#endif
		case CHECK_MCU_TYPE_CMD:
		{
#ifdef CHECK_MCU_TYPE
			response_mcu_type();
#endif
		}
		break;

		case TIME_GET_CMD:{
#ifdef SUPPORT_MCU_RTC_CHECK
			mcu_write_rtctime((unsigned char *)(rf_uart_rx_buf + offset + DATA_START));
#endif
		}
		break;
		
#ifdef  BEACON_TEST
		case SEND_BEACON_NOTIFY_CMD:{
			mcu_received_beacon_test_handle();
		}
		break;
#endif

		case READ_MCU_DP_DATA_CMD:{ // gw read DP
			all_data_update();
		}
		break;

		default:
			return 0;
  }
	return 1;
}

/**
* @brief mcu send a cmd which used to making rf module reset
* @param[in] {void}
* @return  void
*/
void mcu_exit_rf(void)
{
	unsigned short length = 0;
	length = set_rf_uart_byte(length,0x00);
	rf_uart_write_frame(ZIGBEE_CFG_CMD, length);
}

/**
* @brief mcu send this commond starting rf module join network
* this commond must be call after reveived checking proudect info, or after get rf network info
* @param[in] {void}
* @return  void
*/
void mcu_join_rf(void)
{
	unsigned short length = 0;
	length = set_rf_uart_byte(length,0x01);
    rf_uart_write_frame(ZIGBEE_CFG_CMD, length);
}

/**
* @brief mcu send a cmd which used to getting rf network state 
* @param[in] {void}
* @return  void
*/
void mcu_get_rf_state(void)
{
  rf_uart_write_frame(ZIGBEE_STATE_CMD, 0);
}


#ifdef CHECK_MCU_TYPE
/**
* @brief response rf check mcu type cmd 
* @param[in] {void}
* @return  void
*/
static void response_mcu_type(void)
{
  unsigned short length = 0;
  length = set_rf_uart_byte(length,mcu_type);
  rf_uart_write_frame(CHECK_MCU_TYPE_CMD, length);
}   
#endif



#ifdef SUPPORT_MCU_RTC_CHECK
/**
* @brief mcu send a cmd which used to getting timestamp
* @param[in] {void}
* @return void
*/
void mcu_get_system_time(void)
{
  rf_uart_write_frame(TIME_GET_CMD,0);
}

/**
* @brief is a leap year
* @param[in] {pyear}
* @return  leap year return 1 
*/
static unsigned char Is_Leap_Year(unsigned int pyear)
{
	if(0 == (pyear%4)) {
		if(0 == (pyear%100)){
			if(0 == (pyear%400))   
				return 1;
			else    
				return 0;
		}
		else
			return 1;	
	}
	else
		return 0;
}

/**
* @brief get calendar time from timestamp
* @param[in]  {secCount} input timestamp 
* @param[out] {calendar} output calendar 
* @return  result of changing
*/
static unsigned char RTC_Get(unsigned int secCount,_calendar_obj *calendar)
{
	static unsigned int dayCount=0;
	unsigned int tmp=0;	
	unsigned int tmp1=0;	

	tmp=secCount/86400;

	if(dayCount!=tmp){
		dayCount=tmp;
		tmp1=1970;
	
		while(tmp>=365){
			if(Is_Leap_Year(tmp1)){
				if(tmp>=366)    
					tmp-=366;
				else{
					break;
				}
			}
			else
				tmp-=365;
			tmp1++;	
		}
		calendar->w_year=tmp1;
		tmp1=0;
		
		while(tmp>=28){
			if(Is_Leap_Year(calendar->w_year)&&tmp1==1){
				if(tmp>=29)	
					tmp-=29;	
				else
					break;
			}
			else{
				if(tmp>=mon_table[tmp1])
					tmp-=mon_table[tmp1];	
				else
					break;
			}
			tmp1++;
		}
		calendar->w_month=tmp1+1;	
		calendar->w_date=tmp+1;
	}

	tmp=secCount%86400;
	calendar->hour=tmp/3600;
	calendar->min=(tmp%3600)/60;
	calendar->sec=(tmp%3600)%60;

	return 0;
}

/**
* @brief calculta time from frame 
* @param[in] {void}
* @return  void
*/
void rf_timestamp_to_time(void)	
{
	unsigned int time_stamp = byte_to_int(timestamp);
	RTC_Get(time_stamp,&_time);	
}
#endif

#ifdef BROADCAST_DATA_SEND
/**
* @brief mcu can send a broadcast data in rf network
* @param[in] {buf} array of buf which to be sended
* @param[in] {buf_len} send buf length
* @return void
*/
 void mcu_send_broadcast_data(unsigned char buf[], unsigned char buf_len)
{
	 unsigned short length = 0;
     length = set_rf_uart_buffer(length,(unsigned char *)buf, buf_len);
     rf_uart_write_frame(SEND_BROADCAST_DATA_CMD,length);
}
#endif

#ifdef SET_ZIGBEE_NWK_PARAMETER
/**
* @brief mcu can set rf nwk parameter
* @param[in] {parameter_t *} 
* @param
* @return void
*/
 void mcu_set_rf_nwk_parameter(nwk_parameter_t *Pparameter)
{
	unsigned short length = 0;
	#error "please set network parameter in here, when rf received this message, it will start reboot"
	 //Pparameter->app_trigger_rejoin  = 0x00;
     //Pparameter->fast_poll_period = 0x00;

     length = set_rf_uart_byte(length,(Pparameter->heart_period >>8)); 
	 length = set_rf_uart_byte(length,(Pparameter->heart_period)); 

	 length = set_rf_uart_byte(length,(Pparameter->join_nwk_time >>8)); 
	 length = set_rf_uart_byte(length,(Pparameter->join_nwk_time)); 

	 length = set_rf_uart_byte(length,(Pparameter->rejion_period >>8)); 
     length = set_rf_uart_byte(length,(Pparameter->rejion_period)); 

     length = set_rf_uart_byte(length,(Pparameter->poll_period >>8)); 
	 length = set_rf_uart_byte(length,(Pparameter->poll_period));

	 length = set_rf_uart_byte(length,(Pparameter->fast_poll_period >>8)); 
	 length = set_rf_uart_byte(length,(Pparameter->fast_poll_period)); 

	 length = set_rf_uart_byte(length,(Pparameter->poll_failure_times)); 

	 length = set_rf_uart_byte(length,(Pparameter->app_trigger_rejoin)); 

	 length = set_rf_uart_byte(length,(Pparameter->rejoin_try_times)); 

	 length = set_rf_uart_byte(length,(Pparameter->rf_send_power)); 
     rf_uart_write_frame(SET_ZIGBEE_NEK_PARAMETER_CMD,length);
}
#endif

/**
* @brief mcu version string to char
* @param[in] {void}
* @return  result of version
*/
unsigned char get_current_mcu_fw_ver(void)
{
	unsigned char *fw_ver = (unsigned char*) MCU_VER;	//Current version
	unsigned char current_mcu_fw_ver = 0;
	if(assic_to_hex(fw_ver[5]) == -1){ // 3 number
		current_mcu_fw_ver = (assic_to_hex(fw_ver[0]) & 0x03) << 6;	//high ver
		current_mcu_fw_ver |= (assic_to_hex(fw_ver[2]) & 0x03) << 4;	//mid ver
		current_mcu_fw_ver |= (assic_to_hex(fw_ver[4]) & 0x0F);	//low ver
	}
	else{ // 4 number
		current_mcu_fw_ver = (assic_to_hex(fw_ver[0]) & 0x03) << 6;	//high ver
		current_mcu_fw_ver |= (assic_to_hex(fw_ver[2]) & 0x03) << 4;	//mid ver
		current_mcu_fw_ver |= ((assic_to_hex(fw_ver[4])*10 + assic_to_hex(fw_ver[5])) & 0x0F);	//low ver
	}
	return current_mcu_fw_ver;
}

#ifdef SUPPORT_MCU_OTA
/**
* @brief get mcu pid data saved in current_mcu_pid
* @param[in] {void}
* @return  result of handle
*/
void current_mcu_fw_pid(void)
{
	unsigned char i = 0;
	unsigned char *fw_pid = (unsigned char*) PRODUCT_KEY;
	
	while(i < 8){
		current_mcu_pid[i] = fw_pid[i];
		i++;
	}
}

/**
* @brief mcu ota offset requset 
* @param[in] {packet_offset}  packet offset 
* @return  viod
*/
//when call this function, should set timeout event, if not received rf send response should res
void mcu_ota_fw_request(void)
{
	unsigned char i = 0;
	unsigned short length = 0;

	if(ota_fw_info.mcu_current_offset >= ota_fw_info.mcu_ota_fw_size)   //outside
		return;
	while(i < 8){
		length = set_rf_uart_byte(length,ota_fw_info.mcu_ota_pid[i]); //ota fw pid
		i++;
	}
	length = set_rf_uart_byte(length,ota_fw_info.mcu_ota_ver);		//ota fw version
	i = 0;
	while(i < 4){
		length = set_rf_uart_byte(length , ota_fw_info.mcu_current_offset >> (24 - i * 8));	//pakage offset request
		i++;
	}
	length = set_rf_uart_byte(length ,FW_SINGLE_PACKET_SIZE);	// packet size request
	
	rf_uart_write_frame(MCU_OTA_DATA_REQUEST_CMD,length);
}

/**
* @brief mcu ota result report 
* @param[in] {status} result of mcu ota
* @return  void
*/
void mcu_ota_result_report(unsigned char status)
{
	unsigned short length = 0;
	unsigned char i = 0;
	
	length = set_rf_uart_byte(length,status); //upgrade result status(0x00:ota success;0x01:ota failed)
	while(i < 8){
		length = set_rf_uart_byte(length,ota_fw_info.mcu_ota_pid[i]);	//PID
		i++;
	}
	length = set_rf_uart_byte(length,ota_fw_info.mcu_ota_ver);	//ota fw version
	
	rf_uart_write_frame(MCU_OTA_RESULT_CMD,length);	//response 
}

#endif

/**
* @brief compare str1 and str2
* @param[in] {str1} str1 
* @param[in] {str2} str2
* @param[in] {len}  len
* @return  equal return 0 else return -1
*/
int strcmp_barry(unsigned char *str1,unsigned char *str2,unsigned char len)
{
   for(unsigned char i = 0; i < len; i++){
		if(str1[i] < str2[i]){
			return -1; //str1 < str2
		}
		else if(str1[i] > str2[i]){ 
			return 1; //str1 > str2
		}
	}
	return 0; //str1 == str2
}

/**
* @brief translate assic to hex
* @param[in] {assic_num} assic number
* @return hex data
*/
char assic_to_hex(unsigned char assic_num)
{
	if(assic_num<0x30 || assic_num > 0x39)	//0~9
		return -1;
	else
		return assic_num % 0x30;
}



