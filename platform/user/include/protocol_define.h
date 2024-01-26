/**
* @file  protocol.h
* @brief declaration of fuction in  protocol.c
* @author qinlang
* @date 2022.05.06
* @par email:
* @par email:qinlang.chen@tuya.com
* @copyright HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
* @par company
* http://www.tuya.com
*/
#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <cstdint>
///< product INFORMATION





///< mcu version 
#define MCU_VER "0.0.1"     // MAX 3.3.15   BIT 7~0  XX.XX.XXXX                  
 
#define ZIGBEE_UART_QUEUE_LMT             500             // using to save data received from uart
#define ZIGBEE_UART_RECV_BUF_LMT          128             //
#define ZIGBEE_UART_SEND_BUF_LMT          128             //


typedef enum
{
    MCU_TYPE_DC_POWER = 1,
    MCU_TYPE_LOWER_POWER,
    MCU_TYPE_SCENE
}MCU_TYPE_E;

/**
 * if mcu need to support the time function, this macro definition should be opened
 * and need complete mcu_write_rtctime function 
 * 
 */
 
//#define    SUPPORT_MCU_RTC_CHECK             //start time calibration

/**
 * if mcu need to support OTA, this macro definition should be opened
 */
//#define    SUPPORT_MCU_OTA                  //support mcu ota


/**
 * if mcu need to support mcu type checking, this macro definition should be opened
 * 
 */
//#define    CHECK_MCU_TYPE               //support mcu type check 


/**
 * if mcu need to support rf network parameter setting, this macro definition should be opened
 * 
 */
//#define  SET_ZIGBEE_NWK_PARAMETER        //support rf nwk parameter setting 


/**
 * if mcu need to send a broadcast data, this macro definition should be opened
 * 
 */
//#define  BROADCAST_DATA_SEND           //support broadcast data sending



/**
 * DP data list,this code will be generate by cloud platforms
 */

//Switch 1(Issue and report)
#define DPID_SWITCH_LED_1 1
//Bright Value 1(Issue and report)
#define DPID_BRIGHT_VALUE_1 2
//Min Brightness 1(Issue and report)
#define DPID_BRIGHTNESS_MIN_1 3
//Light Type 1(Issue and report)
#define DPID_LED_TYPE_1 4
//Max Brightness 1(Issue and report)
#define DPID_BRIGHTNESS_MAX_1 5
//Timer 1(Issue and report)
#define DPID_TRANSACTION_1 6
//light  Mode 1(Issue and report)
#define DPID_LIGHT_MODE_1 7

//device status 1(Issue and report)
#define DPID_DEVICE_STATUS_1 8

//CCT_OR_HSL parameter(Issue and report)
#define DPID_CCT_OR_HSL_1 9

// Lightness parameter(Issue and report)
#define DPID_LIGHTNESS_1 10

//TERMP_OR_HUE parameter(Issue and report)
#define DPID_TERMP_OR_HUE_1 11

//Saturation parameter(Issue and report)
#define DPID_SATURATION_1 12


//Restart Status(Issue and report)
#define DPID_RESTART_STATUS 200

#define NUMBER_CHANNEL_IN_DEVICE 8 

enum{
  SET_SWITCH_STATE = 0,
  SET_DIM_VALUE_EXPECT,
	SET_TERMPERA_COLOR,
	SET_HUE_PARA,
	SET_SATURATION,
	SET_DIM_DIM_MIN,
	SET_DIM_MAX,
	SET_LAMP_TYPE,
	SET_LIGHT_MODE,
	SET_TRANSACTION_TIME,
	SET_RESTART_STATUS,
};

typedef struct DRIVER_TYPEDEFF
{
	//Lamp propety: 
	uint16_t 	dimSwitchState;
	float 		dimValueExpect; //Brightneed. 
	float 		hue;						//hue RGB para
	float			saturation;			//saturation RGB para;
	float 		temperaColor;
	
	uint16_t 	dimLevelMin;
	uint8_t 	lamType;				//to create dimValueByProfile
	uint16_t 	dimLevelMax;
	unsigned int transaction;
	uint8_t 	lightMode;		//0: Independent, 1: Mixed  DPID_LIGHT_MODE
	uint8_t 	restartStauts;
	
	//wireless button
	char btUpBatLongPressingActive;
	char btDownBatLongPressingActive;
	char btUpAndDownBatLongPressingActive;
	//wireless encoder
	int encoderValue;
	
	char  automationHasBeenStated;
	unsigned int  delayOffTime ;	//unit: second
	float dimValueCurrent;
	char blinkingNumber;

}driver_info;

typedef struct AUTOMATION_TYPEDEF
{

	
	unsigned char  level ;	//unit: second
	unsigned int   dimValueSet;


}automation_control;


typedef struct DEVICE_INFOR
{
	char networkStatus;
	char iBeaconMessageComming;
	char tableSave;
	unsigned int expiredTimeParingMode; //
	unsigned int expiredTimeAppAddingMode; //
	unsigned int cnErrorUartRx;
	unsigned int cnErrorUartTx;
	unsigned int cnErrorSaveFlash;
	
	//wireless config button
	char btCfgPressingActive;
	
	driver_info driver[NUMBER_CHANNEL_IN_DEVICE];
}device_info;


typedef struct {
  unsigned char dp_id;                     
  unsigned char dp_type;           
} DOWNLOAD_CMD_S;

/**
* @brief encapsulates a generic send function, developer should use their own function to completing this fuction 
* @param[in] {value} send signle data 
* @return  void
*/
void uart_transmit_output(unsigned char value);

/**
* @brief Upload all dp information of the system, and realize the synchronization of APP and muc data
* @param[in] {void}
* @return  void
*/
void all_data_update(void);

/**
* @brief mcu check local RTC time 
* @param[in] {time} timestamp
* @return  void
*/
void mcu_write_rtctime(unsigned char time[]);

/**
* @brief Zigbee functional test feedback
* @param[in] {void} 
* @return  void
*/
void rf_test_result(void);

/**
* @brief this function will handle uart received frame data  
* @param[in] {dpid}   dp id
* @param[in] {value}  dp data 
* @param[in] {length} lenght of dp data 
* @return  handle result 
*/
unsigned char dp_download_handle(unsigned char dpid,const unsigned char value[], unsigned short length);
unsigned char dp_handle_device(unsigned char dpid,const unsigned char value[], unsigned short length);
unsigned char dp_respond_handle(unsigned char dpid,const unsigned char value[], unsigned short length);
/**
* @brief get received cmd total number
* @param[in] {void}   
* @return  received cmd total number
*/
unsigned char get_download_cmd_total(void);

/**
* @brief received rf net_work state handle 
* @param[in] {rf_work_state}  rf current network state
* @return  void 
*/
void rf_work_state_event(unsigned char rf_work_state);
/**
* @brief received reset rf response 
* @param[in] {state} response state 
* @return  void 
*/
void mcu_reset_rf_event(unsigned char state);

/**
* @brief check mcu version response
* @param[in] {void}
* @return  void 
*/
void response_mcu_ota_version_event(void);

void rf_notify_factory_new_hanlde(void);
#ifdef SUPPORT_MCU_OTA 
/**
* @brief mcu ota update notify response
* @param[in] {offset} offset of file 
* @return  void 
*/
void response_mcu_ota_notify_event(unsigned char offset);
/**
* @brief received mcu ota data request response
* @param[in] {fw_offset}  offset of file 
* @param[in] {data}  received data  
* @return  void 
*/
void reveived_mcu_ota_data_handle(unsigned int fw_offset,char *data);

/**
* @brief mcu send ota data request 
* @param[in] {offset} offset of file 
* @return  void 
*/
void mcu_ota_fw_request_event(unsigned char offset);

/**
* @brief mcu ota data result notify
* @param[in] {offset} offset of file 
* @return  void 
*/
void mcu_ota_result_event(unsigned char offset);


/**
* @brief mcu ota data handle 
* @param[in] {fw_offset} frame offset 
* @param[in] {data} received data
* @return  void 
*/
void ota_fw_data_handle(unsigned int fw_offset,char *data,unsigned char data_len);
#endif


#ifdef  BEACON_TEST
/**
* @brief beacon test notify,which used in testing
* @param[in] {void} 
* @return  void 
*/
void mcu_received_beacon_test_handle(void);
#endif

#ifdef  CHECK_ZIGBEE_NETWORK
/**
* @brief check rf nwkstate,before start join nwk, check nwk if state is not 0x02,can start jion,
* else delay 5s 
*
* @param[in] {void} 
* @return  void 
*/
void mcu_check_rf_nwk_state(void);
#endif

#ifdef READ_DP_DATA_NOTIFY
/**
* @brief when gateway repower or relink clould, or rf module join sucess, repower, this commod will notify 
* mcu, to sys dp data, mcu itself decide whether report.
*
* @param[in] {void} 
* @return  void 
*/
void read_dp_data_notify_hanlde(void);
#endif

#ifdef __cplusplus
}
#endif
#endif