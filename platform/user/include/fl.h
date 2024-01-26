/*
 * fl.h
 *
 *  Created on: Sep 14, 2023
 *      Author: hoang
 */

#ifndef APPS_USER_FL_H_
#define APPS_USER_FL_H_

#include "types.h"
#include <cstdint>


#define FL_DATA_LENGTH_MAX 	64
#define MAX_CHANEL_DEVICE	8
// #define NUMBER_CHANNEL_IN_DEVICE 8

struct ProductInfo_t
{
    u8 product_id[32];
	u8 version[16];
	u8 num_channels;
};

typedef struct fl_data {
	u8 header[2];
	u8 version;
	u8 sequence[2];
	u8 cmdword;
	u8 datalength[2];
	u8 data[64]; // byte cuoi la: check sum,

}fl_data_t;


#endif /* APPS_USER_FL_H_ */