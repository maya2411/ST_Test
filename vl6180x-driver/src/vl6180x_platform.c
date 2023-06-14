#include "vl6180x_platform.h"

/**
 * DISPLAY public
 */
/***************  DISPLAY PUBLIC *********************/
const char *DISP_NextString;
/***************  DISPLAY PRIVATE *********************/
//static char DISP_CurString[10];
static int DISP_Loop = 0;
uint32_t TimeStarted; /* various display and mode delay starting time */

void DISP_ExecLoopBody(void) {
//    if (DISP_NextString != NULL) {
//        strncpy(DISP_CurString, DISP_NextString, sizeof(DISP_CurString) - 1);
//        DISP_CurString[sizeof(DISP_CurString) - 1] = 0;
//        DISP_NextString = NULL;
//    }
    //XNUCLEO6180XA1_DisplayString(DISP_CurString, DigitDisplay_ms);
    DISP_Loop++;
}



/**
 * @brief       Write data buffer to VL6180x device via i2c
 * @param dev   The device to write to
 * @param buff  The data buffer
 * @param len   The length of the transaction in byte
 * @return      0 on success
 * @ingroup cci_i2c
 */
int  VL6180x_I2CWrite(VL6180xDev_t dev, uint8_t  *buff, uint8_t len)
{
    //TODO
    return 0;
}

/**
 *
 * @brief       Read data buffer from VL6180x device via i2c
 * @param dev   The device to read from
 * @param buff  The data buffer to fill
 * @param len   The length of the transaction in byte
 * @return      0 on success
 * @ingroup  cci_i2c
 */
int VL6180x_I2CRead(VL6180xDev_t dev, uint8_t *buff, uint8_t len)
{
    //TODO
    return 0;
}

