#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#include "vl53l5cx_api.h"

#define NUM_OF_SEC  10
#define FREQ_HZ     5
#define RES 4
uint8_t	resolution = RES*RES;
uint8_t sharpener_percent;
//uint8_t	resolution = VL53L5CX_RESOLUTION_8X8;

int main()
{

    VL53L5CX_Configuration myDev ;
    /*********************************/
    /*   VL53L5CX ranging variables  */
    /*********************************/
    uint8_t 				status, isAlive, isReady,i, j, n,index;
    VL53L5CX_ResultsData 	Results;		/* Results data from VL53L5CX */
    unsigned long loop;

    // to store the execution time of code
    double time_spent = 0.0;


    //double times[FREQ_HZ * NUM_OF_SEC];



    /* ----------------------------------------------------------------------------- */

    uint16_t addr = 0x52;  //vl53l5 device default address is 0x29 <-- 0x52
    status = vl53l5cx_comms_init(&myDev.platform,addr);

    if (status != 0)
    {
        printf("vl53l5cx_comms_init() ended with error %d\n", status);
        return -1;
    }
    else
       printf("\nvl53l5cx_comms_init() addr: %x, fd: %d\n ULD API version %s \n", \
               (myDev.platform.address>>1), myDev.platform.fd, VL53L5CX_API_REVISION);

    /*********************************/
    /*   Power on sensor and init    */
    /*********************************/

    /* (Optional) Check if there is a VL53L5CX sensor connected */
    status = vl53l5cx_is_alive(&myDev, &isAlive);
    if(!isAlive || status)
    {
        printf("VL53L5CX not detected at requested address\n");
        return status;
    }

    /* (Mandatory) Init VL53L5CX sensor */
    status = vl53l5cx_init(&myDev);
    if(status)
    {
        printf("VL53L5CX ULD Loading failed\n");
        return status;
    }

    /* Set resolution. WARNING : As others settings depend to thisone, it must be the first to use */
    status = vl53l5cx_set_resolution(&myDev, resolution);
    if(status)
    {
        printf("vl53l5cx_set_resolution failed, status %u\n", status);
        return status;
    }
    else
        printf("the resolution is %d\n", resolution);

    /* Set ranging frequency to 10Hz.
     * Using 4x4, min frequency is 1Hz and max is 60Hz
     * Using 8x8, min frequency is 1Hz and max is 15Hz
     */
    status = vl53l5cx_set_ranging_frequency_hz(&myDev, 10);
    if(status)
    {
        printf("vl53l5cx_set_ranging_frequency_hz failed, status %u\n", status);
        return status;
    }

    status = vl53l5cx_get_sharpener_percent(&myDev,&sharpener_percent);
    if(status)
    {
        printf("vl53l5cx_get_sharpener_percent failed, status %u\n", status);
        return status;
    }
    else
        printf("vl53l5cx_get_sharpener_percent returned %d\n", sharpener_percent);

    sharpener_percent = 40;

    status = vl53l5cx_set_sharpener_percent(&myDev, sharpener_percent);
    if(status)
    {
        printf("vl53l5cx_set_sharpener_percent failed, status %u\n", status);
        return status;
    }
    else
        printf("set_sharpener_percent is %d \n",sharpener_percent);

    status = vl53l5cx_set_ranging_mode(&myDev, VL53L5CX_RANGING_MODE_CONTINUOUS);
    if(status)
    {
        printf("vl53l5cx_set_ranging_mode failed, status %u\n", status);
        return status;
    }

    printf("VL53L5CX ULD ready ! (Version : %s)\n",
            VL53L5CX_API_REVISION);

//    sleep(5);
    uint16_t new_addr = 0x48; //0x11
    status = vl53l5cx_set_i2c_address(&myDev, new_addr);
    if(status)
    {
        printf("vl53l5cx_set_i2c_address failed, status %u\n", status);
        return status;
    }
    else
        printf("the i2c address is %x\n", new_addr);

    vl53l5cx_comms_close(&myDev.platform);
    return 0;
//    status = vl53l5cx_set_power_mode(&myDev, VL53L5CX_POWER_MODE_SLEEP);
//    if(status)
//    {
//        printf("vl53l5cx_set_power_mode failed, status %u\n", status);
//        return status;
//    }
//    else
//        printf("the power mode is %d\n", VL53L5CX_POWER_MODE_SLEEP);

    sleep(5);
    status = vl53l5cx_set_power_mode(&myDev, VL53L5CX_POWER_MODE_WAKEUP);
    if(status)
    {
        printf("vl53l5cx_set_power_mode failed, status %u\n", status);
        return status;
    }
    else
        printf("the power mode is %d\n", VL53L5CX_POWER_MODE_WAKEUP);





    /*********************************/
    /*         Ranging loop          */
    /*********************************/

    status = vl53l5cx_start_ranging(&myDev);
    printf("Started ranging\n");

    loop = 0;
    while(1/*loop < 100*/)
    {

        system("clear"); //clear screan

        /* Use polling function to know when a new measurement is ready.
         * Another way can be to wait for HW interrupt raised on PIN A3
         * (GPIO 1) when a new measurement is ready */

        status = vl53l5cx_check_data_ready(&myDev, &isReady);

         clock_t begin = clock();

        if(isReady)
        {
            status = vl53l5cx_get_ranging_data(&myDev, &Results);


            printf("Print data no : %3u status %d \n", myDev.streamcount, status);
            for(i = 0; i < RES; i++)
            {
                for(j = 0; j < RES; j++)
                {

                    for (n = 0; n < VL53L5CX_NB_TARGET_PER_ZONE; n++)
                    {
                        index = i*RES + j*VL53L5CX_NB_TARGET_PER_ZONE + n;
                        //printf("%5d, ", Results.distance_mm[index]);

                        /* Status indicating the measurement validity
                         5 is 100% good
                         6,9 ~50%
                         */
                        if (5 == Results.target_status[index])
                        {
                            printf("%4d(%1d)", Results.distance_mm[index], Results.nb_target_detected[index]);
                        }
                        else
                        {
                            if ((6 == Results.target_status[index]) || (9 == Results.target_status[index]))
                                printf("%4d(%~)", Results.distance_mm[index]);
                            else
                                printf("    (-)");
                        }

                    }



//                printf("Zone : %3d, Status : %3u, Distance : %4d mm\n",
//                    i,
//                    Results.target_status[VL53L5CX_NB_TARGET_PER_ZONE*i],
//                    Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*i]);
                }

                printf("\n");
            }


            clock_t end = clock();
            // calculate elapsed time by finding difference (end - begin) and
            // dividing the difference by CLOCKS_PER_SEC to convert to seconds
            time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
            //times[loop] = (double)(end - begin) / CLOCKS_PER_SEC;

            loop++;

            /* Wait according to frequency */
            usleep(1000000/FREQ_HZ);
        }
        else
        {
            /* Wait a few ms to avoid too high polling (function in platform
             * file, not in API) */
            printf("not ready\n");
            usleep(10*1000);
            loop++;
        }

    }


    printf(" ResultsSize is %ld Bytes\n ", sizeof(VL53L5CX_ResultsData));
    printf("The avarege time spent reading a messurement %f seconds\n",( time_spent/(FREQ_HZ * NUM_OF_SEC) ));


    status = vl53l5cx_stop_ranging(&myDev);
    printf("End of ULD demo\n");


    vl53l5cx_comms_close(&myDev.platform);
    printf("closed device on address %d", myDev.platform.address);

    return 0;
}
