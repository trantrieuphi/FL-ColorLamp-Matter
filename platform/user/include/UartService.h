#include <system/SystemError.h>
#include <cstdint>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>

#include <platform/CHIPDeviceLayer.h>

#define MSG_SIZE 256

///< Byte order of frames
#define         HEAD_FIRST                      0
#define         HEAD_SECOND                     1
#define         PROTOCOL_VERSION                2
#define         SEQ_HIGH                        3
#define         SEQ_LOW                         4
#define         FRAME_TYPE                      5
#define         LENGTH_HIGH                     6
#define         LENGTH_LOW                      7
#define         DATA_START                      8

///< define cmd of frame
#define         MATTER_FACTORY_NEW_CMD          0
#define         PRODUCT_INFO_CMD                1                               //pid information
#define         MATTER_STATE_CMD                2                               //rf module send network state to mcu
#define         MATTER_CFG_CMD                  3                               //mcu config rf state
#define         MATTER_DATA_REQ_CMD             4                               //rf module shend DP data to mcu
#define         DATA_DATA_RES_CMD               5                               //mcu response rf DP send
#define         DATA_REPORT_CMD                 6                               //mcu report DP data to rf
#define         MATTER_RF_TEST_CMD              8
#define         QUERY_KEY_INFO_CMD              9
#define         CALL_SCENE_CMD                  10
#define 		RF_IBEACON_MESSAGE				0xff

#define			MCU_OTA_VERSION_CMD				    0x0B							//rf request mcu version
#define			MCU_OTA_NOTIFY_CMD				    0x0C							//mcu ota notify
#define			MCU_OTA_DATA_REQUEST_CMD			0x0D							//MCU OTA data request
#define			MCU_OTA_RESULT_CMD					0x0E							//MCU OTA result report

#define         CHECK_MATTER_NETWORK_CMD            0x20
#define         TIME_GET_CMD                        0x24
#define         CHECK_MCU_TYPE_CMD                  0x25
#define         SET_MATTER_NEK_PARAMETER_CMD        0x26
#define         SEND_BROADCAST_DATA_CMD             0x27
#define         READ_MCU_DP_DATA_CMD                0x28
#define         SEND_BEACON_NOTIFY_CMD              0x29





///< frme data define
#define SERIAL_PROTOCOL_VER                 0x02                                            // the version of frame
#define PROTOCOL_HEAD                       0x09                                           //the lcation of frame length except for tail
#define FIRST_FRAME_HEAD                    0x55                                            // first byte of frame
#define SECOND_FRAME_HEAD                   0xaa                                            // second byte of frame

///< define communication state
#define MATTER_NOT_JOIN							      0x00	//rf module not jion network
#define MATTER_JOIN_GATEWAY								0x01	//rf module had jioned network
#define MATTER_JOIN_ERROR									0x02	//rf module network state error
#define MATTER_JOINING							    	0x03	//rf module jioning

#define REPORT_DATA_SUCESS	                            0x10	//translate sucess
#define REPORT_DATA_FAILURE	                            0x20	//translate failure
#define REPORT_DATA_TIMEOUT	                            0x40	//translate timeout
#define REPORT_DATA_BUSY                                0x80	//translate busy

#define RESET_MATTER_OK			0x00	//rest rf success
#define RESET_MATTER_ERROR  	0x01	//reset rf error

// typedef struct res_flag{
// 	data_buffer_t data;
// 	uint8_t flag;
// } res_flag_t;

// void service_handle (void);
// int32_t parking_message(void);
// int32_t check_ACK(void);

// DEFINE uart B91 telink
int uart_init();
void print_uart(char *buf);
void debug_uart(char *buf, int len);
void printUartHexToHexString(const uint8_t * buf, uint16_t bufLen);
void serial_cb(const struct device *dev, void *user_data);
void uart_send(uint8_t *buf, uint16_t len);
void process_status_data();

