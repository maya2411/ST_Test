#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#include "vl53l5cx_api.h"

#define NUM_OF_SEC  10
#define FREQ_HZ     5
#define RES 4

#define PRX_NUM 4

typedef enum E_I2C_BUS {
    E_I2C_BUS_BACK,
    E_I2C_BUS_FRONT,
    E_I2C_BUS_NUM
}E_PRX_TYPE;

static uint8_t	resolution = RES*RES;
static uint8_t sharpener_percent;
//uint8_t	resolution = VL53L5CX_RESOLUTION_8X8;
static const u_int16_t Default_vl53l5Address = 0x52;// VL53l5 device default I2C address is 0x29 <-- 0x52
static const u_int16_t Default_vl6180Address = 0x52;// VL6180X has a default I2C address of 0x29 <-- 0x52!

int main()
{
    int i2c_fd[E_I2C_BUS_NUM];
    VL53L5CX_Configuration myDev[E_I2C_BUS_NUM][PRX_NUM] ;

    /*********************************/
    /*   VL53L5CX ranging variables  */
    /*********************************/
    uint8_t 				status, isAlive, isReady,i, j, n,index;
    VL53L5CX_ResultsData 	Results;		/* Results data from VL53L5CX */
    unsigned long loop;
    // to store the execution time of code
    double time_spent = 0.0;
    //double times[FREQ_HZ * NUM_OF_SEC];


    // TODO -verify back is i2c0 and front is i2c2
    // assuming rc.local turned on all proximities by gpio 51,52 131,133

    status = i2c_comms_init(&i2c_fd[E_I2C_BUS_BACK],"/dev/i2c-0");
    if (status != 0)
    {
        printf("i2c_comms_init(/dev/i2c-0) ended with error %d\n", status);
        return -1;
    }
    status = i2c_comms_init(&i2c_fd[E_I2C_BUS_FRONT],"/dev/i2c-2");
    if (status != 0)
    {
        printf("i2c_comms_init(/dev/i2c-2) ended with error %d\n", status);
        return -1;
    }

    for (int i = 0; i < E_I2C_BUS_NUM; i++){
        // The gpio expander on 0x20 address on each I2C is turned on.
        // Set all P0 and P1 pins to output by writing 0x0 to registers 0x6 and 0x7
        i2cWrByte(i2c_fd[i], 0x20, 0x6, 0);
        i2cWrByte(i2c_fd[i], 0x20, 0x7, 0);

        for (int index =0; index < PRX_NUM; index++) {
            myDev[i][index].platform.fd = i2c_fd[i];

            // turn on just the one vl53l5cx proximity according to j
            uint8_t value = (0x00 | 0x1 << index);
            i2cWrByte(i2c_fd[i], 0x20, 0x2, value);

            // change i2c addresses for all 4 VL53L5CX proximities on i2c bus, relevant LPn pins need to be set to Low
            // keep the vl6180 proximity at default address
            status = vl53l5cx_comms_init(&myDev[i][j].platform, Default_vl53l5Address);

            if (status != 0)
            {
                printf("vl53l5cx_comms_init() ended with error %d\n", status);
                return -1;
            }
            else
               printf("\nvl53l5cx_comms_init() addr: %x, fd: %d\n ULD API version %s \n", \
                       (myDev[i][j].platform.address>>1), myDev[i][j].platform.fd, VL53L5CX_API_REVISION);

            uint16_t new_addr = (Default_vl53l5Address - index - 1);
            status = vl53l5cx_set_i2c_address(&myDev[i][j], new_addr);
            if(status)
            {
                printf("vl53l5cx_set_i2c_address failed, status %u\n", status);
                return status;
            }
            else
                printf("the i2c address for vl53l5cx number %d is %x\n", i, new_addr);

        }
    }




    /* ----------------------------------------------------------------------------- */


    /*********************************
       Power on sensors and init them
    *********************************/

    /* (Optional) Check if there is a VL53L5CX sensor connected */
//    status = vl53l5cx_is_alive(&myDev, &isAlive);
//    if(!isAlive || status)
//    {
//        printf("VL53L5CX not detected at requested address\n");
//        return status;
//    }

//    /* (Mandatory) Init VL53L5CX sensor */
//    status = vl53l5cx_init(&myDev[i]);
//    if(status)
//    {
//        printf("VL53L5CX ULD Loading failed\n");
//        return status;
//    }

//    /* Set resolution. WARNING : As others settings depend to thisone, it must be the first to use */
//    status = vl53l5cx_set_resolution(&myDev, resolution);
//    if(status)
//    {
//        printf("vl53l5cx_set_resolution failed, status %u\n", status);
//        return status;
//    }
//    else
//        printf("the resolution is %d\n", resolution);

//    /* Set ranging frequency to 10Hz.
//     * Using 4x4, min frequency is 1Hz and max is 60Hz
//     * Using 8x8, min frequency is 1Hz and max is 15Hz
//     */
//    status = vl53l5cx_set_ranging_frequency_hz(&myDev, 10);
//    if(status)
//    {
//        printf("vl53l5cx_set_ranging_frequency_hz failed, status %u\n", status);
//        return status;
//    }

//    status = vl53l5cx_get_sharpener_percent(&myDev,&sharpener_percent);
//    if(status)
//    {
//        printf("vl53l5cx_get_sharpener_percent failed, status %u\n", status);
//        return status;
//    }
//    else
//        printf("vl53l5cx_get_sharpener_percent returned %d\n", sharpener_percent);

//    sharpener_percent = 40;

//    status = vl53l5cx_set_sharpener_percent(&myDev, sharpener_percent);
//    if(status)
//    {
//        printf("vl53l5cx_set_sharpener_percent failed, status %u\n", status);
//        return status;
//    }
//    else
//        printf("set_sharpener_percent is %d \n",sharpener_percent);

//    status = vl53l5cx_set_ranging_mode(&myDev, VL53L5CX_RANGING_MODE_CONTINUOUS);
//    if(status)
//    {
//        printf("vl53l5cx_set_ranging_mode failed, status %u\n", status);
//        return status;
//    }

//    printf("VL53L5CX ULD ready ! (Version : %s)\n",
//            VL53L5CX_API_REVISION);


////    status = vl53l5cx_set_power_mode(&myDev, VL53L5CX_POWER_MODE_SLEEP);
////    if(status)
////    {
////        printf("vl53l5cx_set_power_mode failed, status %u\n", status);
////        return status;
////    }
////    else
////        printf("the power mode is %d\n", VL53L5CX_POWER_MODE_SLEEP);

//    sleep(5);
//    status = vl53l5cx_set_power_mode(&myDev, VL53L5CX_POWER_MODE_WAKEUP);
//    if(status)
//    {
//        printf("vl53l5cx_set_power_mode failed, status %u\n", status);
//        return status;
//    }
//    else
//        printf("the power mode is %d\n", VL53L5CX_POWER_MODE_WAKEUP);





//    /*********************************/
//    /*         Ranging loop          */
//    /*********************************/

//    status = vl53l5cx_start_ranging(&myDev);
//    printf("Started ranging\n");

//    loop = 0;
//    while(1/*loop < 100*/)
//    {

//        system("clear"); //clear screan

//        /* Use polling function to know when a new measurement is ready.
//         * Another way can be to wait for HW interrupt raised on PIN A3
//         * (GPIO 1) when a new measurement is ready */

//        status = vl53l5cx_check_data_ready(&myDev, &isReady);

//         clock_t begin = clock();

//        if(isReady)
//        {
//            status = vl53l5cx_get_ranging_data(&myDev, &Results);


//            printf("Print data no : %3u status %d \n", myDev.streamcount, status);
//            for(i = 0; i < RES; i++)
//            {
//                for(j = 0; j < RES; j++)
//                {

//                    for (n = 0; n < VL53L5CX_NB_TARGET_PER_ZONE; n++)
//                    {
//                        index = i*RES + j*VL53L5CX_NB_TARGET_PER_ZONE + n;
//                        //printf("%5d, ", Results.distance_mm[index]);

//                        /* Status indicating the measurement validity
//                         5 is 100% good
//                         6,9 ~50%
//                         */
//                        if (5 == Results.target_status[index])
//                        {
//                            printf("%4d(%1d)", Results.distance_mm[index], Results.nb_target_detected[index]);
//                        }
//                        else
//                        {
//                            if ((6 == Results.target_status[index]) || (9 == Results.target_status[index]))
//                                printf("%4d(%~)", Results.distance_mm[index]);
//                            else
//                                printf("    (-)");
//                        }

//                    }



////                printf("Zone : %3d, Status : %3u, Distance : %4d mm\n",
////                    i,
////                    Results.target_status[VL53L5CX_NB_TARGET_PER_ZONE*i],
////                    Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*i]);
//                }

//                printf("\n");
//            }


//            clock_t end = clock();
//            // calculate elapsed time by finding difference (end - begin) and
//            // dividing the difference by CLOCKS_PER_SEC to convert to seconds
//            time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
//            //times[loop] = (double)(end - begin) / CLOCKS_PER_SEC;

//            loop++;

//            /* Wait according to frequency */
//            usleep(1000000/FREQ_HZ);
//        }
//        else
//        {
//            /* Wait a few ms to avoid too high polling (function in platform
//             * file, not in API) */
//            printf("not ready\n");
//            usleep(10*1000);
//            loop++;
//        }

//    }


//    printf(" ResultsSize is %ld Bytes\n ", sizeof(VL53L5CX_ResultsData));
//    printf("The avarege time spent reading a messurement %f seconds\n",( time_spent/(FREQ_HZ * NUM_OF_SEC) ));


//    status = vl53l5cx_stop_ranging(&myDev);
//    printf("End of ULD demo\n");


//    vl53l5cx_comms_close(&myDev.platform);
//    printf("closed device on address %d", myDev.platform.address);

    return 0;
}
