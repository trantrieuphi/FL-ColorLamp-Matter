/**
* @file  mcu_api.c
* @brief this file contains basic fuctions and DP data report fucntion which is base on DP type
* @author qinlang
* @date 2022.05.06
* @par email:qinlang.chen@tuya.com
* @copyright HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
* @par company
* http://www.tuya.com
*/
// #include "main.h"
#include "zigbee_define.h"
#include "system_define.h"
#include "stdio.h"
#include "stdint.h"
#include <zephyr/drivers/uart.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>



using namespace chip::app;
// LOG_MODULE_DECLARE(app, CONFIG_CHIP_APP_LOG_LEVEL);

// #include "UartService.h"
/**
* @brief hex translate to bcd 
* @param[in] {Value_H} higher bits data 
* @param[in] {Value_L} lower bits data
* @return  bcd type data
*/
extern const struct device *uart1;
K_MSGQ_DEFINE(queue_data_rx, sizeof(fl_data_t), 16, alignof(fl_data_t));

extern uint8_t NUM_LIGHTS;
extern bool IsChangeNumberDevice; 
extern ProductInfo_t product_info;

void send_data(uint8_t *buf, uint16_t len)
{
  for (int i = 0; i < len; i++)
	{
		uart_poll_out(uart1, buf[i]);
	}
}

void mcu_reset(void)
{
    sys_reboot(0);
}
void read_product_info(uint8_t *buf_word, uint8_t size)
{
    const struct device *flash_dev = APP_PARTITION_DEVICE;
    uint32_t offset = MEMORY_DEVICE_INFO;
    uint8_t check = 0;
    check = flash_read(flash_dev, offset, buf_word, size);
    if (check) {
        // print_uart("Flash read failed\n");
        // return 0;
    }
    // send_data(buf_word, size);
}
void write_product_info(uint8_t *buf_word, uint8_t size)
{
    const struct device *flash_dev = APP_PARTITION_DEVICE;
    uint32_t offset = MEMORY_DEVICE_INFO;
    uint8_t check = 0;
    check = flash_erase(flash_dev, offset, FLASH_PAGE_SIZE);
    if (check) {
        // print_uart("Flash erase failed\n");
        return 0;
    } 
    // print_uart("Flash write\n");
    check = flash_write(flash_dev, offset, buf_word, size);
    if (check) {
        // print_uart("Flash write failed\n");
        // return 0;
    }
    // send_data(buf_word, size);
}

unsigned char hex_to_bcd(unsigned char Value_H,unsigned char Value_L)
{
  unsigned char bcd_value;
  
  if((Value_H >= '0') && (Value_H <= '9')){
    Value_H -= '0';
  }
  else if((Value_H >= 'A') && (Value_H <= 'F')){
    Value_H = Value_H - 'A' + 10;
  }
  else if((Value_H >= 'a') && (Value_H <= 'f')){
    Value_H = Value_H - 'a' + 10;
  }

  bcd_value = Value_H & 0x0f;
  bcd_value <<= 4;

  if((Value_L >= '0') && (Value_L <= '9')){
    Value_L -= '0';
  }
  else if((Value_L >= 'A') && (Value_L <= 'F')){
    Value_L = Value_L - 'a' + 10;
  }
  else if((Value_L >= 'a') && (Value_L <= 'f')){
    Value_L = Value_L - 'a' + 10;
  }
  
  bcd_value |= Value_L & 0x0f;

  return bcd_value;
}

/**
* @brief get string len 
* @param[in] {str} higher bits data 
* @return string len 
*/
unsigned long my_strlen(unsigned char *str)  
{
  unsigned long len = 0;
  if(str == NULL){ 
    return 0;
  }
  
  for(len = 0; *str ++ != '\0'; ){
    len ++;
  }
  
  return len;
}

/**
* @brief  assign ch to the first count bytes of the memory address src
* @param[in] {src}    srouce address
* @param[in] {ch}     set value 
* @param[in] {count}  length of set address 
* @return void
*/
void *my_memset(void *src,unsigned char ch,unsigned short count)
{
  unsigned char *tmp = (unsigned char *)src;
  if(src == NULL){
    return NULL;
  }
  
  while(count --){
    *tmp ++ = ch;
  }
  return src;
}

/**
* @brief copy count bytes data from src to dest
* @param[in] {src}    srouce address
* @param[in] {dest}   destination address
* @param[in] {count}  length of copy data
* @return void
*/
void *my_memcpy(void *dest, const void *src, unsigned short count)  
{  
  unsigned char *pdest = (unsigned char *)dest;  
  const unsigned char *psrc  = (const unsigned char *)src;  
  unsigned short i;
  
  if(dest == NULL || src == NULL){ 
    return NULL;
  }
  
  if((pdest <= psrc) || (pdest > psrc + count)){  
    for(i = 0; i < count; i ++){  
      pdest[i] = psrc[i];  
    }  
  }
  else{
    for(i = count; i > 0; i --){  
      pdest[i - 1] = psrc[i - 1];  
    }  
  }  
  
  return dest;  
}

/**
* @brief copy string src to string dest
* @param[in] {src}    srouce address
* @param[in] {dest}   destination address
* @return the tail of destination 
*/
char *my_strcpy(char *dest, const char *src)  
{
  char *p = dest;
  while(*src!='\0'){
    *dest++ = *src++;
  }
  *dest = '\0';
  return p;
}

/**
* @brief compare string s1 with string s2
* @param[in] {s1}  srouce address
* @param[in] {s2}  destination address
* @return compare result
*/
int my_strcmp(char *s1 , char *s2)
{
  while( *s1 && *s2 && *s1 == *s2 ){
    s1++;
    s2++;
  }
  return *s1 - *s2;
}

/**
* @brief int translate to byte
* @param[in] {number} int data 
* @param[out] {value} the result array     
* @return void
*/
void int_to_byte(unsigned long number,unsigned char value[4])
{
  value[0] = number >> 24;
  value[1] = number >> 16;
  value[2] = number >> 8;
  value[3] = number & 0xff;
}

/**
* @brief byte data translate to int 
* @param[in] {value}  the byte array 
* @return result of int data 
*/
unsigned long byte_to_int(const unsigned char value[4])
{
  unsigned long nubmer = 0;

  nubmer = (unsigned long)value[0];
  nubmer <<= 8;
  nubmer |= (unsigned long)value[1];
  nubmer <<= 8;
  nubmer |= (unsigned long)value[2];
  nubmer <<= 8;
  nubmer |= (unsigned long)value[3];
  
  return nubmer;
}

unsigned int byte_to_short(const unsigned char value[2])
{
  unsigned int nubmer = 0;

  nubmer = (unsigned long)value[0];
  nubmer <<= 8;
  nubmer |= (unsigned long)value[1];
  
  return nubmer;
}

/**
* @brief report bool type DP data to rf module 
* @param[in]  {dpid}  dp id
* @param[in]  {value} Data contents of dp 
* @return send result 
*/
unsigned char mcu_dp_bool_update(unsigned char dpid,unsigned char value)
{
  unsigned short length = 0;
  
  length = set_rf_uart_byte(length,dpid);
  length = set_rf_uart_byte(length,DP_TYPE_BOOL);
 
  length = set_rf_uart_byte(length,0);
  length = set_rf_uart_byte(length,1);

  if(value == FALSE){
    length = set_rf_uart_byte(length,FALSE);
  }
  else{
    length = set_rf_uart_byte(length,1);
  }
  
  rf_uart_write_frame(ZIGBEE_DATA_REQ_CMD,length);
  
  return SUCCESS;
}


/**
* @brief report enum type DP data to rf module 
* @param[in]  {dpid}  dp id
* @param[in]  {value} Data contents of dp 
* @return send result 
*/
unsigned char mcu_dp_enum_update(unsigned char dpid,unsigned char value)
{
  unsigned short length = 0;
  
  length = set_rf_uart_byte(length,dpid);
  length = set_rf_uart_byte(length,DP_TYPE_ENUM);
  //
  length = set_rf_uart_byte(length,0);
  length = set_rf_uart_byte(length,1);
  //
  length = set_rf_uart_byte(length,value);
  
  rf_uart_write_frame(DATA_REPORT_CMD,length);
  
  return SUCCESS;
}


/**
* @brief report bitmap type DP data to rf module 
* @param[in]  {dpid}  dp id
* @param[in]  {value} Data contents of dp 
* @return send result 
*/
unsigned char mcu_dp_bitmap_update(unsigned char dpid,unsigned long value)
{
  unsigned short length = 0;

  length = set_rf_uart_byte(length,dpid);
  length = set_rf_uart_byte(length,DP_TYPE_BITMAP);
  //
  length = set_rf_uart_byte(length,0);
  
  if((value | 0xff) == 0xff){
    length = set_rf_uart_byte(length,1);
    length = set_rf_uart_byte(length,value);
  }
  else if((value | 0xffff) == 0xffff){
    length = set_rf_uart_byte(length,2);
    length = set_rf_uart_byte(length,value >> 8);
    length = set_rf_uart_byte(length,value & 0xff);
  }
  else{
    length = set_rf_uart_byte(length,4);
    length = set_rf_uart_byte(length,value >> 24);
    length = set_rf_uart_byte(length,value >> 16);
    length = set_rf_uart_byte(length,value >> 8);
    length = set_rf_uart_byte(length,value & 0xff);
  }    
  
  rf_uart_write_frame(ZIGBEE_DATA_REQ_CMD,length);

  return SUCCESS;
}


/**
* @brief report fault type DP data to rf module 
* @param[in]  {dpid}  dp id
* @param[in]  {value} Data contents of dp 
* @return send result 
*/
unsigned char mcu_dp_fault_update(unsigned char dpid,unsigned long value)
{
  unsigned short length = 0;

  length = set_rf_uart_byte(length,dpid);
  length = set_rf_uart_byte(length,DP_TYPE_BITMAP);
  //
  length = set_rf_uart_byte(length,0);
  
  if((value | 0xff) == 0xff){
    length = set_rf_uart_byte(length,1);
    length = set_rf_uart_byte(length,value);
  }
  else if((value | 0xffff) == 0xffff){
    length = set_rf_uart_byte(length,2);
    length = set_rf_uart_byte(length,value >> 8);
    length = set_rf_uart_byte(length,value & 0xff);
  }
  else{
    length = set_rf_uart_byte(length,4);
    length = set_rf_uart_byte(length,value >> 24);
    length = set_rf_uart_byte(length,value >> 16);
    length = set_rf_uart_byte(length,value >> 8);
    length = set_rf_uart_byte(length,value & 0xff);
  }    
  
  rf_uart_write_frame(DATA_REPORT_CMD,length);

  return SUCCESS;
}

/**
* @brief report raw type DP data to rf module 
* @param[in]  {dpid}  dp id
* @param[in]  {value} Data contents of dp 
* @param[in]  {len} length of Data contents  
* @return send result 
*/
unsigned char mcu_dp_raw_update(unsigned char dpid,const unsigned char value[],unsigned short len)
{
  unsigned short length = 0;
  
  length = set_rf_uart_byte(length,dpid);
  length = set_rf_uart_byte(length,DP_TYPE_RAW);
  //
  length = set_rf_uart_byte(length,len / 0x100);
  length = set_rf_uart_byte(length,len % 0x100);
  //
  length = set_rf_uart_buffer(length,(unsigned char *)value,len);
  
  rf_uart_write_frame(DATA_REPORT_CMD,length);
  
  return SUCCESS;
}

/**
* @brief report string type DP data to rf module 
* @param[in]  {dpid}  dp id
* @param[in]  {value} Data contents of dp 
* @param[in]  {len} length of Data contents  
* @return send result 
*/
unsigned char mcu_dp_string_update(unsigned char dpid,const unsigned char value[],unsigned short len)
{
  unsigned short length = 0;
  
  length = set_rf_uart_byte(length,dpid);
  length = set_rf_uart_byte(length,DP_TYPE_STRING);
  //
  length = set_rf_uart_byte(length,len / 0x100);
  length = set_rf_uart_byte(length,len % 0x100);
  //
  length = set_rf_uart_buffer(length,(unsigned char *)value,len);
  
  rf_uart_write_frame(DATA_REPORT_CMD,length);
  
  return SUCCESS;
}
char *strstr_custom( char *haystack, char *needle) {
    while (*haystack) {
        char *h = haystack;
       char *n = needle;

        while (*n && *h == *n) {
            h++;
            n++;
        }

        if (*n == '\0') {
            return (char *)haystack;
        }

        haystack++;
    }

    return NULL;
}
void get_product_info(fl_data_t *data)
{


		// printf("get_product_info\n");

		char prd_id[] = "\"p\"";
		char version[] = "\"v\"";
		char channel[] = "\"c\"";

		char *ptrp = strstr_custom( (char*)data->data, prd_id);
		if(ptrp != NULL){
			uint8_t i = 0;
			// printf("product_info: ");
			while( ptrp[i +5] != '\"'){
				// printf("%c", ptrp[i+5]);
				i++;
			}
			// printf("\n");
      memset(product_info.product_id, 0, 32);
      memcpy(product_info.product_id,ptrp+5,i);
			// memset(device.produc_info, 0, 32);
			// memcpy(device.produc_info,ptrp+5,i);
		}

		char *ptrv = strstr_custom( (char*)data->data, version);
		if(ptrv != NULL){
			u8 i = 0;
			// printf("version: ");
			while( ptrv[i +5] != '\"'){
				// printf("%c", ptrv[i+5]);
				i++;
			}
			// printf("\n");
      memset(product_info.version, 0, 16);
      memcpy(product_info.version,ptrv+5,i);
			// memset(device.version, 0, 16);
			// memcpy(device.produc_info,ptrv+5,i);
		}

		char *ptrc = strstr_custom( (char*)data->data, channel);
		if(ptrp != NULL){
      product_info.num_channels = ptrc[5] - 0x30;
		}
    
		if(product_info.num_channels)
		{
      write_product_info((uint8_t*)&product_info, sizeof(ProductInfo_t));
      // mcu_reset();
      chip::Server::GetInstance().ScheduleFactoryReset();
		}

}

/**
* @brief report raw type DP data to rf module 
* @param[in]  {dpid}  dp id
* @param[in]  {value} Data contents of dp 
* @return send result 
*/
unsigned char mcu_dp_value_update(unsigned char dpid,unsigned long value)
{
  unsigned short length = 0;
  
  length = set_rf_uart_byte(length,dpid);
  length = set_rf_uart_byte(length,DP_TYPE_VALUE);
  //
  length = set_rf_uart_byte(length,0);
  length = set_rf_uart_byte(length,4);
  //
  length = set_rf_uart_byte(length,value >> 24);
  length = set_rf_uart_byte(length,value >> 16);
  length = set_rf_uart_byte(length,value >> 8);
  length = set_rf_uart_byte(length,value & 0xff);
  
  rf_uart_write_frame(ZIGBEE_DATA_REQ_CMD,length);
  
  return SUCCESS;
}

unsigned char mcu_dp_hsl_value_update(unsigned char dpid, uint16_t hue, uint16_t saturation, uint16_t level)
{
  unsigned short length = 0;
  
  length = set_rf_uart_byte(length,dpid);
  length = set_rf_uart_byte(length,DP_TYPE_RAW);
  //
  length = set_rf_uart_byte(length,0);
  length = set_rf_uart_byte(length,6);
  // 6 bytes
  length = set_rf_uart_byte(length, hue >> 8);
  length = set_rf_uart_byte(length, hue & 0xff);
  length = set_rf_uart_byte(length, saturation >> 8);
  length = set_rf_uart_byte(length, saturation & 0xff);
  length = set_rf_uart_byte(length, level >> 8);
  length = set_rf_uart_byte(length, level & 0xff);
  
  rf_uart_write_frame(ZIGBEE_DATA_REQ_CMD,length);
  
  return SUCCESS;
}

unsigned char mcu_dp_hsl_value_hue_update(unsigned char dpid, uint16_t hue)
{
  unsigned short length = 0;
  
  length = set_rf_uart_byte(length,dpid);
  length = set_rf_uart_byte(length,DP_TYPE_VALUE);
  //
  length = set_rf_uart_byte(length,0);
  length = set_rf_uart_byte(length,2);
  // 2 bytes
  length = set_rf_uart_byte(length, hue >> 8);
  length = set_rf_uart_byte(length, hue & 0xff);
  
  rf_uart_write_frame(ZIGBEE_DATA_REQ_CMD,length);
  
  return SUCCESS;

}

unsigned char mcu_dp_hsl_value_saturation_update(unsigned char dpid, uint16_t saturation)
{
  unsigned short length = 0;
  
  length = set_rf_uart_byte(length,dpid);
  length = set_rf_uart_byte(length,DP_TYPE_VALUE);
  //
  length = set_rf_uart_byte(length,0);
  length = set_rf_uart_byte(length,2);
  // 2 bytes
  length = set_rf_uart_byte(length, saturation >> 8);
  length = set_rf_uart_byte(length, saturation & 0xff);
  
  rf_uart_write_frame(ZIGBEE_DATA_REQ_CMD,length);
  
  return SUCCESS;

}

unsigned char mcu_dp_hsl_value_level_update(unsigned char dpid, uint16_t level)
{
  unsigned short length = 0;
  
  length = set_rf_uart_byte(length,dpid);
  length = set_rf_uart_byte(length,DP_TYPE_VALUE);
  //
  length = set_rf_uart_byte(length,0);
  length = set_rf_uart_byte(length,2);
  // 2 bytes
  length = set_rf_uart_byte(length, level >> 8);
  length = set_rf_uart_byte(length, level & 0xff);
  
  rf_uart_write_frame(ZIGBEE_DATA_REQ_CMD,length);
  
  return SUCCESS;

}
unsigned char mcu_dp_report_network(unsigned char dpid,unsigned long value)
{
  unsigned short length = 0;
  
  length = set_rf_uart_byte(length,dpid);
  length = set_rf_uart_byte(length,DP_TYPE_BOOL);
 
  length = set_rf_uart_byte(length,0);
  length = set_rf_uart_byte(length,1);

  if(value == FALSE){
    length = set_rf_uart_byte(length,FALSE);
  }
  else{
    length = set_rf_uart_byte(length,1);
  }
  
  rf_uart_write_frame(ZIGBEE_STATE_CMD,length);
  
  return SUCCESS;
}

/**
* @brief mcu get bool type value from rf translate 
* @param[in]  {value}  data of dp
* @param[in]  {len} Data length
* @return  true or false
*/
unsigned char mcu_get_dp_download_bool(const unsigned char value[],unsigned short len)
{
  return(value[0]);
}

/**
* @brief mcu get enum type value from rf translate 
* @param[in]  {value}  data of dp
* @param[in]  {len} Data length
* @return  enum data
*/
unsigned char mcu_get_dp_download_enum(const unsigned char value[],unsigned short len)
{
  return(value[0]);
}

/**
* @brief mcu get value type value from rf translate 
* @param[in]  {value}  data of dp
* @param[in]  {len} Data length
* @return  value data
*/
unsigned long mcu_get_dp_download_value(const unsigned char value[],unsigned short len)
{
  return(byte_to_int(value));
}


/**
* @brief mcu get value 2byte type value from rf translate 
* @param[in]  {value}  data of dp
* @param[in]  {len} Data length
* @return  value data
*/
unsigned int mcu_get_dp_download_value_2byte(const unsigned char value[],unsigned short len)
{
  return( byte_to_short(value));
}

/**
* @brief mcu start rf module test 
* @param[in]  {channel} test channel usually 11
* @return void
*/
void mcu_start_rf_test(unsigned char channel)
{
	rf_uart_tx_buf[DATA_START] = channel;	
  rf_uart_write_frame(ZIGBEE_RF_TEST_CMD,1);
}

/**
* @brief mcu reset rf module 
* @param[in]  {void} 
* @return void
*/
void mcu_reset_rf(void) //Ask Join to Network 
{
  unsigned short length = 0;

  length = set_rf_uart_byte(length,0);
  rf_uart_write_frame(ZIGBEE_CFG_CMD, length);
}

/**
* @brief mcu start rf module 
* this commond must be call after reveived checking proudect info, or after get rf network info
* @param[in]  {void} 
* @return void
*/
void mcu_network_get_status(void)  //Request Network Status
{
	
  unsigned short length = 0;

  length = set_rf_uart_byte(length,1);
  rf_uart_write_frame(ZIGBEE_CFG_CMD, length);
}

/**
* @brief copy receive data from uart receive interrupt
* @param[in]  {value} Data received from interrupt
* @return void 
*/
void uart_receive_input(unsigned char value)
{
 // #error "please call this fuction in the interrupt fuction of serial receive, and delete this line"

    if(1 == queue_out - queue_in) {
        //queue full
    }else if((queue_in > queue_out) && ((queue_in - queue_out) >= sizeof(rf_queue_buf))) {
        //queue full
    }else {
        //queue not full
        if(queue_in >= (unsigned char *)(rf_queue_buf + sizeof(rf_queue_buf))){
            queue_in = (unsigned char *)(rf_queue_buf);
        }
        *queue_in ++ = value;
    }
}


/**
* @brief init paramter and set rx_buf with zero
* @param[in]  {void}
* @return void 
*/
void rf_protocol_init(void)
{
  //#error "please call this fuction in main init"

  queue_in = (unsigned char *)rf_queue_buf;
  queue_out = (unsigned char *)rf_queue_buf;
}

/**
* @brief read byte from rf_queue_buf array
* @param[in]  {void}
* @return read byte value
*/
unsigned char Queue_Read_Byte(void)
{
    unsigned char value;
    
    if(queue_out != queue_in) {
        //have data in queue
        if(queue_out >= (unsigned char *)(rf_queue_buf + sizeof(rf_queue_buf))) {
            queue_out = (unsigned char *)(rf_queue_buf);
        }
        value = *queue_out ++;   
    }
    
    return value;
}

/**
 * @brief  check rx buffer is empty or not
 * @param  Null
 * @return the queue state
 */
unsigned char with_data_rxbuff(void)
{
    if(queue_out != queue_in)
        return 1;
    else
        return 0;
}


/**
* @brief uart receive data handle, call this function at mian loop 
* @param[in]  {void}
* @return void 
*/
void rf_uart_service(void)
{
    static unsigned short rx_in = 0;
    unsigned short offset = 0;
    unsigned short rx_value_len = 0;  
    unsigned short protocol_version = 0, protocol_head_len = PROTOCOL_HEAD;

    while((rx_in < sizeof(rf_uart_rx_buf)) && (with_data_rxbuff() > 0)){
        rf_uart_rx_buf[rx_in++] = Queue_Read_Byte();
    }

    if(rx_in < PROTOCOL_HEAD){
        return;
    }

    while((rx_in - offset) >= PROTOCOL_HEAD){
        if(rf_uart_rx_buf[offset + HEAD_FIRST] != FIRST_FRAME_HEAD){
            offset++;
            continue;
        }

        if(rf_uart_rx_buf[offset + HEAD_SECOND] != SECOND_FRAME_HEAD){
            offset++;
            continue;
        }

        protocol_version = rf_uart_rx_buf[offset + PROTOCOL_VERSION];

       if(protocol_version != SERIAL_PROTOCOL_VER) /*version 2 is more than two byte  sequence number than version 1*/
        {
            offset += 2; 
            continue;
        }

        rx_value_len =  rf_uart_rx_buf[offset + LENGTH_HIGH ] * 0x100;
        rx_value_len += (rf_uart_rx_buf[offset + LENGTH_LOW] + PROTOCOL_HEAD);

        if(rx_value_len > sizeof(rf_uart_rx_buf)){
            offset += 3;
            continue;
        }

        if((rx_in - offset) < rx_value_len){
            break;
        }

        if( get_check_sum((unsigned char *)rf_uart_rx_buf + offset, rx_value_len - 1) != rf_uart_rx_buf[offset + rx_value_len - 1]){
            offset += 3; 
            continue;
        }

        // data_handle(offset);
        // store data to queue rx
        fl_data_t data;
        my_memcpy((char*)&data, (const char *)rf_uart_rx_buf + offset, rx_value_len);
        k_msgq_put(&queue_data_rx, &data, K_NO_WAIT);
        
        // send_data(rf_uart_rx_buf, sizeof(rf_uart_rx_buf));
        offset += rx_value_len;
    }
    rx_in -= offset;

    if(rx_in > 0){
			if((offset == 0)  && (rx_in >= sizeof(rf_uart_rx_buf))){
							offset++;
							rx_in -= offset;
			}
			my_memcpy((char*)rf_uart_rx_buf, (const char *)rf_uart_rx_buf + offset, rx_in);
    }
}

uint8_t status_rp(void)
{
	uint8_t data = 1;
	rf_uart_write_frame(DATA_REPORT_CMD, data);
	return	1;
}