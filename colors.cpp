/*
 * colors.cpp
 *
 *  Created on: Jun 18, 2018
 *      Author: christoph
 */

#include "colors.h"


uint32_t get_color(uint32_t code){
	uint32_t temp = (uint32_t) ((float)code / 65535.0 * 139.0);
	return color_codes[temp];
}


