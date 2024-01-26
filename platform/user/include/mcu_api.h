/**
* @file   mcu_api.h
* @brief  declaration of fuction in mcu_api.c
* @author qinlang
* @date 2022.05.06
* @par email:qinlang.chen@tuya.com
* @copyright HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
* @par company
* http://www.tuya.com
*/

#ifndef __MCU_API_H_
#define __MCU_API_H_
#include <app/server/Server.h>
#ifdef __cplusplus
extern "C"
{
#endif

#define APP_PARTITION	storage_partition
#define APP_PARTITION_OFFSET	FIXED_PARTITION_OFFSET(APP_PARTITION)
#define APP_PARTITION_DEVICE	FIXED_PARTITION_DEVICE(APP_PARTITION)

#define FLASH_PAGE_SIZE   4096
#define MEMORY_DEVICE_INFO  0xf4000

#ifdef MCU_API_GLOBAL
  #define MCU_API_EXTERN
#else
  #define MCU_API_EXTERN   extern
#endif
void read_product_info(uint8_t *buf_word, uint8_t size);
void write_product_info(uint8_t *buf_word, uint8_t size);

/**
* @brief hex translate to bcd 
* @param[in] {Value_H} higher bits data 
* @param[in] {Value_L} lower bits data
* @return  bcd type data
*/
unsigned char hex_to_bcd(unsigned char Value_H,unsigned char Value_L);

/**
* @brief get string len 
* @param[in] {str} higher bits data 
* @return string len 
*/
unsigned long my_strlen(unsigned char *str);

/**
* @brief  assign ch to the first count bytes of the memory address src
* @param[in] {src}    srouce address
* @param[in] {ch}     set value 
* @param[in] {count}  length of set address 
* @return void
*/
void *my_memset(void *src,unsigned char ch,unsigned short count);

/**
* @brief copy count bytes data from src to dest
* @param[in] {src}    srouce address
* @param[in] {dest}   destination address
* @param[in] {count}  length of copy data
* @return void
*/
void *my_memcpy(void *dest, const void *src, unsigned short count);

/**
* @brief copy string src to string dest
* @param[in] {src}    srouce address
* @param[in] {dest}   destination address
* @return the tail of destination 
*/
char *my_strcpy(char *dest, const char *src);

/**
* @brief compare string s1 with string s2
* @param[in] {s1}  srouce address
* @param[in] {s2}  destination address
* @return compare result
*/
int my_strcmp(char *s1 , char *s2);

/**
* @brief int translate to byte
* @param[in] {number} int data 
* @param[out] {value} the result array     
* @return void
*/
void int_to_byte(unsigned long number,unsigned char value[4]);

/**
* @brief byte data translate to int 
* @param[in] {value}  the byte array 
* @return result of int data 
*/
unsigned long byte_to_int(const unsigned char value[4]);

/**
* @brief report bool type DP data to rf module 
* @param[in]  {dpid}  dp id
* @param[in]  {value} Data contents of dp 
* @return send result 
*/
unsigned char mcu_dp_bool_update(unsigned char dpid,unsigned char value);

unsigned char mcu_dp_report_network(unsigned char dpid,unsigned long value);
/**
* @brief report enum type DP data to rf module 
* @param[in]  {dpid}  dp id
* @param[in]  {value} Data contents of dp 
* @return send result 
*/
unsigned char mcu_dp_enum_update(unsigned char dpid,unsigned char value);


/**
* @brief report bitmap type DP data to rf module 
* @param[in]  {dpid}  dp id
* @param[in]  {value} Data contents of dp 
* @return send result 
*/
unsigned char mcu_dp_bitmap_update(unsigned char dpid,unsigned long value);

/**
* @brief report fault type DP data to rf module 
* @param[in]  {dpid}  dp id
* @param[in]  {value} Data contents of dp 
* @return send result 
*/
unsigned char mcu_dp_fault_update(unsigned char dpid,unsigned long value);

/**
* @brief report raw type DP data to rf module 
* @param[in]  {dpid}  dp id
* @param[in]  {value} Data contents of dp 
* @param[in]  {len} length of Data contents  
* @return send result 
*/
unsigned char mcu_dp_raw_update(unsigned char dpid,const unsigned char value[],unsigned short len);

/**
* @brief report string type DP data to rf module 
* @param[in]  {dpid}  dp id
* @param[in]  {value} Data contents of dp 
* @param[in]  {len} length of Data contents  
* @return send result 
*/
unsigned char mcu_dp_string_update(unsigned char dpid,const unsigned char value[],unsigned short len);

/**
* @brief report raw type DP data to rf module 
* @param[in]  {dpid}  dp id
* @param[in]  {value} Data contents of dp 
* @return send result 
*/
unsigned char mcu_dp_value_update(unsigned char dpid,unsigned long value);
unsigned char mcu_dp_hsl_value_update(unsigned char dpid, uint16_t hue, uint16_t saturation, uint16_t level);
unsigned char mcu_dp_hsl_value_hue_update(unsigned char dpid, uint16_t hue);
unsigned char mcu_dp_hsl_value_saturation_update(unsigned char dpid, uint16_t saturation);
unsigned char mcu_dp_hsl_value_level_update(unsigned char dpid, uint16_t level);
/**
* @brief mcu get bool type value from rf translate 
* @param[in]  {value}  data of dp
* @param[in]  {len} Data length
* @return  true or false
*/
unsigned char mcu_get_dp_download_bool(const unsigned char value[],unsigned short len);

/**
* @brief mcu get enum type value from rf translate 
* @param[in]  {value}  data of dp
* @param[in]  {len} Data length
* @return  enum data
*/
unsigned char mcu_get_dp_download_enum(const unsigned char value[],unsigned short len);

/**
* @brief mcu get value type value from rf translate 
* @param[in]  {value}  data of dp
* @param[in]  {len} Data length
* @return  value data
*/
unsigned long mcu_get_dp_download_value(const unsigned char value[],unsigned short len);

unsigned int mcu_get_dp_download_value_2byte(const unsigned char value[],unsigned short len);

/**
* @brief mcu reset rf module 
* @param[in]  {void} 
* @return void
*/
void mcu_reset_rf(void);

/**
* @brief mcu start rf module 
* @param[in]  {void} 
* @return void
*/
void mcu_network_get_status(void);


/**
* @brief mcu start rf module test 
* @param[in]  {channel} test channel usually 11
* @return void
*/
void mcu_start_rf_test(unsigned char channel);

/**
* @brief copy receive data from uart receive interrupt
* @param[in]  {value} Data received from interrupt
* @return void 
*/
void uart_receive_input(unsigned char value);

/**
* @brief init paramter and set rx_buf with zero
* @param[in]  {void}
* @return void 
*/
void rf_protocol_init(void);

/**
 * @brief  check rx buffer is empty or not
 * @param  Null
 * @return the queue state
 */
unsigned char with_data_rxbuff(void);

/**
* @brief read byte from rf_queue_buf array
* @param[in]  {void}
* @return read byte value
*/
unsigned char Queue_Read_Byte(void);

/**
* @brief uart receive data handle, call this function at mian loop 
* @param[in]  {void}
* @return void 
*/
void rf_uart_service(void);

void get_product_info(fl_data_t *data);

uint8_t status_rp(void);
#ifdef __cplusplus
}
#endif
#endif
