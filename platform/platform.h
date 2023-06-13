/*******************************************************************************
* Copyright (c) 2020, STMicroelectronics - All Rights Reserved
*
* This file is part of the VL53L5CX Ultra Lite Driver and is dual licensed,
* either 'STMicroelectronics Proprietary license'
* or 'BSD 3-clause "New" or "Revised" License' , at your option.
*
********************************************************************************
*
* 'STMicroelectronics Proprietary license'
*
********************************************************************************
*
* License terms: STMicroelectronics Proprietary in accordance with licensing
* terms at www.st.com/sla0081
*
* STMicroelectronics confidential
* Reproduction and Communication of this document is strictly prohibited unless
* specifically authorized in writing by STMicroelectronics.
*
*
********************************************************************************
*
* Alternatively, the VL53L5CX Ultra Lite Driver may be distributed under the
* terms of 'BSD 3-clause "New" or "Revised" License', in which case the
* following provisions apply instead of the ones mentioned above :
*
********************************************************************************
*
* License terms: BSD 3-clause "New" or "Revised" License.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors
* may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*
*******************************************************************************/

#ifndef PLATFORM_H_
#define PLATFORM_H_
#pragma once

#include <stdint.h>
#include <string.h>

/**
 * @brief Structure VL53L5CX_Platform needs to be filled by the customer,
 * depending on his platform. At least, it contains the VL53L5CX I2C address.
 * Some additional fields can be added, as descriptors, or platform
 * dependencies. Anything added into this structure is visible into the platform
 * layer.
 */

typedef struct
{
	/* To be filled with customer's platform. At least an I2C address/descriptor
	 * needs to be added */
	/* Example for most standard platform : I2C address of sensor */    
    uint16_t  			address;


	/* For Linux implementation, file descriptor */
    int fd;

} VL53L5CX_Platform;

/*
 * @brief The macro below is used to define the number of target per zone sent
 * through I2C. This value can be changed by user, in order to tune I2C
 * transaction, and also the total memory size (a lower number of target per
 * zone means a lower RAM). The value must be between 1 and 4.
 */

#define 	VL53L5CX_NB_TARGET_PER_ZONE		1U

/* Results can be tuned in order to reduce I2C access and RAM footprints.
 * The 'platform.h' file contains macros used to disable output. If user declare
 * one of these macros, the results will not be sent through I2C, and the array will not
 * be created into the VL53L5CX_ResultsData structure.
 * For the minimum size, ST recommends 1 targets per zone, and only keep distance_mm,
 * target_status, and nb_target_detected.
 */

#ifdef LAW_RAM

#define VL53L5CX_DISABLE_AMBIENT_PER_SPAD
#define VL53L5CX_DISABLE_NB_SPADS_ENABLED
#define VL53L5CX_DISABLE_SIGNAL_PER_SPAD
#define VL53L5CX_DISABLE_RANGE_SIGMA_MM
#define VL53L5CX_DISABLE_REFLECTANCE_PERCENT
#define VL53L5CX_DISABLE_MOTION_INDICATOR

#endif
/*
 * @brief The macro below can be used to avoid data conversion into the driver.
 * By default there is a conversion between firmware and user data. Using this macro
 * allows to use the firmware format instead of user format. The firmware format allows
 * an increased precision.
 */

// #define 	VL53L5CX_USE_RAW_FORMAT

/*
 * @brief All macro below are used to configure the sensor output. User can
 * define some macros if he wants to disable selected output, in order to reduce
 * I2C access.
 */

// #define VL53L5CX_DISABLE_AMBIENT_PER_SPAD
// #define VL53L5CX_DISABLE_NB_SPADS_ENABLED
// #define VL53L5CX_DISABLE_NB_TARGET_DETECTED
// #define VL53L5CX_DISABLE_SIGNAL_PER_SPAD
// #define VL53L5CX_DISABLE_RANGE_SIGMA_MM
// #define VL53L5CX_DISABLE_DISTANCE_MM
// #define VL53L5CX_DISABLE_REFLECTANCE_PERCENT
// #define VL53L5CX_DISABLE_TARGET_STATUS
// #define VL53L5CX_DISABLE_MOTION_INDICATOR


 /**
 * @brief Mandatory function used to read one single byte.
 * @param (VL53L5CX_Platform*) p_platform : Pointer of VL53L5CX platform
 * structure.
 * @param (uint16_t) Address : I2C location of value to read.
 * @param (uint8_t) *p_values : Pointer of value to read.
 * @return (uint8_t) status : 0 if OK
 */

uint8_t RdByte(
		VL53L5CX_Platform * p_platform,
		uint16_t reg_address,
		uint8_t *p_value);

/**
 * @brief Mandatory function used to write one single byte.
 * @param (VL53L5CX_Platform*) p_platform : Pointer of VL53L5CX platform
 * structure.
 * @param (uint16_t) reg_address : I2C location of value to read.
 * @param (uint8_t) value : Pointer of value to write.
 * @return (uint8_t) status : 0 if OK
 */
uint8_t i2cWrByte(
        int fd,
        uint16_t i2c_addr,
        uint16_t reg_address,
        uint8_t value);

uint8_t WrByte(
		VL53L5CX_Platform * p_platform,
		uint16_t reg_address,
		uint8_t value);

/**
 * @param (VL53L5CX_Platform*) p_platform : Pointer of VL53L5CX platform
 * structure.
 * @param (uint16_t) reg_address : I2C location of values to read.
 * @param (uint8_t) *p_values : Buffer of bytes to read.
 * @param (uint32_t) size : Size of *p_values buffer.
 * @return (uint8_t) status : 0 if OK
 */

uint8_t RdMulti(
		VL53L5CX_Platform * p_platform,
		uint16_t reg_address,
		uint8_t *p_values,
		uint32_t size);

/**
 * @brief Mandatory function used to write multiples bytes.
 * @param (VL53L5CX_Platform*) p_platform : Pointer of VL53L5CX platform
 * structure.
 * @param (uint16_t) reg_address : I2C location of values to write.
 * @param (uint8_t) *p_values : Buffer of bytes to write.
 * @param (uint32_t) size : Size of *p_values buffer.
 * @return (uint8_t) status : 0 if OK
 */

uint8_t WrMulti(
		VL53L5CX_Platform * p_platform,
		uint16_t reg_address,
		uint8_t *p_values,
		uint32_t size);

/**
 * @brief Optional function, only used to perform an hardware reset of the
 * sensor. This function is not used in the API, but it can be used by the host.
 * This function is not mandatory to fill if user don't want to reset the
 * sensor.
 * @param (VL53L5CX_Platform*) p_platform : Pointer of VL53L5CX platform
 * structure.
 * @return (uint8_t) status : 0 if OK
 */

uint8_t Reset_Sensor(
		VL53L5CX_Platform * p_platform);

/**
 * @brief Mandatory function, used to swap a buffer. The buffer size is always a
 * multiple of 4 (4, 8, 12, 16, ...).
 * @param (uint8_t*) buffer : Buffer to swap, generally uint32_t
 * @param (uint16_t) size : Buffer size to swap
 */

void SwapBuffer(
		uint8_t 		*buffer,
		uint16_t 	 	 size);
/**
 * @brief Mandatory function, used to wait during an amount of time. It must be
 * filled as it's used into the API.
 * @param (VL53L5CX_Platform*) p_platform : Pointer of VL53L5CX platform
 * structure.
 * @param (uint32_t) TimeMs : Time to wait in ms.
 * @return (uint8_t) status : 0 if wait is finished.
 */

uint8_t WaitMs(
		VL53L5CX_Platform * p_platform,
		uint32_t TimeMs);

/**
 * @brief I2C communication channel initialization
 * @param (VL53L5CX_Platform) *p_platform : pointer on a I2C device descriptor.
 * @param uint16_t addr - 8 bit address on the I2C bus
 * @param const char* p_i2cbusName - for example "/dev/i2c-0"
 * @return (uint8_t) status : 0 if OK
 */
int32_t vl53l5cx_comms_init(VL53L5CX_Platform * p_platform, uint16_t addr);


/**
 * @brief I2C communication channel deletion
 * @param (int) fd : I2C channel descriptor.
 * @return (uint8_t) status : 0 if OK
 */
int32_t vl53l5cx_comms_close(VL53L5CX_Platform * p_platform);


/**
 * @brief General I2C communication channel initialization
 * @param (int) *fd : pointer on a I2C channel descriptor
 * @param const char* p_i2cbusName - forexample "/dev/i2c-0"
 * @return (uint8_t) status : 0 if OK
 */
int32_t i2c_comms_init(int* fd, const char* p_i2cbusName);

#endif	// _PLATFORM_H_
