#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "vl53l5cx_api.h"

#define NUM_OF_SEC  10
#define FREQ_HZ     5

#define PRX_NUM 4

typedef enum E_I2C_BUS {
    E_I2C_BUS_BACK,
    E_I2C_BUS_FRONT,
    E_I2C_BUS_NUM
}E_PRX_TYPE;

const char* I2C_BUS_NAME[E_I2C_BUS_NUM] = {"/dev/i2c-0", "/dev/i2c-2"};
const u_int16_t I2C_BUS_NEW_ADDR[PRX_NUM] = {0x4B,0x4D,0x4F,0x50};

const char* GPIO_EXP_SET_OUT_P0[E_I2C_BUS_NUM] = {"i2cset -y -r 0 0x20 0x6 0x0", "i2cset -y -r 2 0x20 0x6 0x0"};
const char* GPIO_EXP_SET_OUT_P1[E_I2C_BUS_NUM] = {"i2cset -y -r 0 0x20 0x7 0x0", "i2cset -y -r 2 0x20 0x7 0x0"};
const char* VL6180_OFF_STR[E_I2C_BUS_NUM] = {"i2cset -y -r 0 0x20 0x3 0x0",  "i2cset -y -r 2 0x20 0x3 0x0"};
const char* VL6180_ON_STR[E_I2C_BUS_NUM]  = {"i2cset -y -r 0 0x20 0x3 0x08", "i2cset -y -r 2 0x20 0x3 0x08"};
const char* VL53L5_ON_STR[E_I2C_BUS_NUM]  = {"i2cset -y -r 0 0x20 0x2 0x0F", "i2cset -y -r 2 0x20 0x2 0x0F"};
const char* VL53L5C_SET_IDX_STR[E_I2C_BUS_NUM][PRX_NUM]= { \
{"i2cset -y -r 0 0x20 0x2 0x1", "i2cset -y -r 0 0x20 0x2 0x2", "i2cset -y -r 0 0x20 0x2 0x4", "i2cset -y -r 0 0x20 0x2 0x8"},
{"i2cset -y -r 2 0x20 0x2 0x1", "i2cset -y -r 2 0x20 0x2 0x2", "i2cset -y -r 2 0x20 0x2 0x4", "i2cset -y -r 2 0x20 0x2 0x8"}};

#define RES (4)
static const uint8_t	resolution = VL53L5CX_RESOLUTION_4X4;
//#define RES (8)
//static const uint8_t	resolution = VL53L5CX_RESOLUTION_8X8;
static uint8_t    sharpener_percent;
// VL53l5 device default I2C address is 0x29 <-- 0x52  according to VL53L5CX_DEFAULT_I2C_ADDRESS
//static const u_int16_t DFLT_VL6180ADD = 0x52;// VL6180X has a default I2C address of 0x29 <-- 0x52!

static int i2c_fd[E_I2C_BUS_NUM];
static VL53L5CX_Configuration myDev[E_I2C_BUS_NUM][PRX_NUM] ;
// to store the execution time of code
static double time_spent = 0.0;
//static double times[FREQ_HZ * NUM_OF_SEC];

void ranging_loop(u_int8_t i2c_bus,u_int8_t index);

int main()
{
    for (int t=0;t<PRX_NUM;t++){
        printf("addess %d is %d 0x%x i2c-bus 0x%x\n", t, I2C_BUS_NEW_ADDR[t],I2C_BUS_NEW_ADDR[t], I2C_BUS_NEW_ADDR[t]>>1);
        //printf("set indx string is %s \n",VL53L5C_SET_IDX_STR[0][t]);
    }



    uint8_t status,isAlive;

    // TODO -verify back is i2c0 and front is i2c2
    // turned on all proximities by gpio 51,52 131,133
    system("./prox_on.sh");
    usleep(10000);

    for (u_int8_t i2c_bus = 0; i2c_bus < E_I2C_BUS_NUM; i2c_bus++)
    {
        // open FD for i2c-bus

        i2c_fd[i2c_bus] = i2c_fd_init(I2C_BUS_NAME[i2c_bus]);
        if (i2c_fd[i2c_bus] == -1) { return -1; }

        // The gpio expander on 0x20 address on each I2C is turned on.
        // Set all P0 and P1 pins to output by writing 0x0 to registers 0x6 and 0x7
        printf("Writing once registers 0x6 and 0x7 for output on %s", I2C_BUS_NAME[i2c_bus]);
        //status = i2cWrByte(i2c_fd[i2c_bus], 0x40, 0x6, 0);
        system(GPIO_EXP_SET_OUT_P0[i2c_bus]); usleep(10000);
        system(GPIO_EXP_SET_OUT_P1[i2c_bus]); usleep(10000);
        //status = i2cWrByte(i2c_fd[i2c_bus], 0x40, 0x7, 0);
        printf("  returned with status %u\n",status);
        system(VL6180_OFF_STR[i2c_bus]); usleep(10000);
        //status = i2cWrByte(i2c_fd[i2c_bus], 0x40, 0x3, 0);
        //printf("Turning off the VL6180 prox returned status %u\n", status);

        for (int index =0; index < PRX_NUM; index++)
        {
            // turn on just the one vl53l5cx proximity according to j
            uint8_t value = (0x00 | 0x1 << index);
            //status = i2cWrByte(i2c_fd[i2c_bus], 0x40, 0x2, value);
            printf("\n\nTurn on only index [%d] write %u to register 2 on 0x20 expander returned %d.", index, value, status);
            system(VL53L5C_SET_IDX_STR[i2c_bus][index]);

            myDev[i2c_bus][index].platform.fd = i2c_fd[i2c_bus];  //set device i2c fd
            status = vl53l5cx_comms_init(&myDev[i2c_bus][index].platform, VL53L5CX_DEFAULT_I2C_ADDRESS); //set device addr
            if (status != 0) { return -1;}
            status = vl53l5cx_is_alive(&myDev[i2c_bus][index], &isAlive);
            if(!isAlive || status)
            {
                printf("VL53L5CX not detected at %s at 0x%x\n", I2C_BUS_NAME[i2c_bus], myDev[i2c_bus][index].platform.address);
                return status;
            } else {printf("it's alive at address 0x%x:)) \n",myDev[i2c_bus][index].platform.address);}


            // change i2c addresses for all 4 VL53L5CX proximities on i2c bus, relevant LPn pins need to be set to Low
            // keep the vl6180 proximity at default address
            uint16_t new_addr = I2C_BUS_NEW_ADDR[index];

            status = vl53l5cx_set_i2c_address(&myDev[i2c_bus][index], new_addr);
            if(status)
            {
                printf("vl53l5cx_set_i2c_address() on [%s] to addr [%x] failed with Error status: %d \n",I2C_BUS_NAME[i2c_bus],new_addr>>1,status);
                //return status;
            }
            else
                printf("The I2C new address for vl53l5cx on - [%s] Number [%d] is %x\n", I2C_BUS_NAME[i2c_bus], index, new_addr>>1);

        }

        // Finished, turn all 5 prox on
        system(VL6180_ON_STR[i2c_bus]);
        system(VL53L5_ON_STR[i2c_bus]);
    }




    /* ----------------------------------------------------------------------------- */


    /*********************************
    Sensors are Powered, now init them
    *********************************/
    for (u_int8_t i2c_bus = 0; i2c_bus < E_I2C_BUS_NUM; i2c_bus++)
    {
        for (int index =0; index < PRX_NUM; index++)
        {
            /* (Mandatory) Init VL53L5CX sensor */
            status = vl53l5cx_init(&myDev[i2c_bus][index]);
            if(status)
            {
                printf("VL53L5CX ULD Loading failed on device [%d].[%d]\n", i2c_bus,index);
                return status;
            }
            /* Set resolution. WARNING : As others settings depend to thisone, it must be the first to use */
            status = vl53l5cx_set_resolution(&myDev[i2c_bus][index], resolution);
            if(status)
            {
                printf("vl53l5cx_set_resolution failed on device [%d].[%d], status %u\n", i2c_bus,index, status);
                return status;
            }
            else
                printf("the resolution is %d on device [%d].[%d]\n",i2c_bus,index, resolution);

            /* Set ranging frequency to 10Hz.
             * Using 4x4, min frequency is 1Hz and max is 60Hz
             * Using 8x8, min frequency is 1Hz and max is 15Hz
             */
            status = vl53l5cx_set_ranging_frequency_hz(&myDev[i2c_bus][index], 10);
            if(status)
            {
                printf("vl53l5cx_set_ranging_frequency_hz failed, status %u\n", status);
                return status;
            }

            status = vl53l5cx_get_sharpener_percent(&myDev[i2c_bus][index],&sharpener_percent);
            if(status)
            {
                printf("vl53l5cx_get_sharpener_percent failed, status %u\n", status);
                return status;
            }
            else
                printf("vl53l5cx_get_sharpener_percent returned %d\n", sharpener_percent);

            sharpener_percent = 40;

            status = vl53l5cx_set_sharpener_percent(&myDev[i2c_bus][index], sharpener_percent);
            if(status)
            {
                printf("vl53l5cx_set_sharpener_percent failed, status %u\n", status);
                return status;
            }
            else
                printf("set_sharpener_percent is %d \n",sharpener_percent);

            status = vl53l5cx_set_ranging_mode(&myDev[i2c_bus][index], VL53L5CX_RANGING_MODE_CONTINUOUS);
            if(status)
            {
                printf("vl53l5cx_set_ranging_mode failed, status %u\n", status);
                return status;
            }

            sleep(5);
            status = vl53l5cx_set_power_mode(&myDev[i2c_bus][index], VL53L5CX_POWER_MODE_WAKEUP);
            if(status)
            {
                printf("vl53l5cx_set_power_mode failed, status %u\n", status);
                return status;
            }
            else
                printf("the power mode is %d\n", VL53L5CX_POWER_MODE_WAKEUP);


            status = vl53l5cx_start_ranging(&myDev[i2c_bus][index]);
            printf("VL53L5CX ULD ready ! (Version : %s) on device [%d].[%d]  Started ranging\n\n\n",VL53L5CX_API_REVISION,i2c_bus,index);
        }
    }


    sleep(5);


////    status = vl53l5cx_set_power_mode(&myDev[i2c_bus][index], VL53L5CX_POWER_MODE_SLEEP);
////    if(status)
////    {
////        printf("vl53l5cx_set_power_mode failed, status %u\n", status);
////        return status;
////    }
////    else
////        printf("the power mode is %d\n", VL53L5CX_POWER_MODE_SLEEP);




    /*********************************/
    /*         Ranging loop          */
    /*********************************/
    unsigned long  loop = 0;
    uint8_t     isReady = 0;

    while(1/*loop < 100*/)
    {
        for (u_int8_t i2c_bus = 0; i2c_bus < E_I2C_BUS_NUM; i2c_bus++)
        {
            for (u_int8_t index =0; index < PRX_NUM; index++)
            {
                system("clear"); //clear screan

                /* Use polling function to know when a new measurement is ready.
                 * Another way can be to wait for HW interrupt raised on PIN A3
                 * (GPIO 1) when a new measurement is ready */

                status = vl53l5cx_check_data_ready(&myDev[i2c_bus][index], &isReady);

                if(isReady)
                {
                    ranging_loop(i2c_bus,index);
                    loop++;

                    /* Wait according to frequency */
                    usleep(1000000/FREQ_HZ);
                }
                else
                {
                    /* Wait a few ms to avoid too high polling (function in platform
                     * file, not in API) */
                    printf("[%d.%d]not ready\n",i2c_bus,index);
                    usleep(10*1000);
                    loop++;
                }
            }
        }
    }


    printf(" ResultsSize is %ld Bytes\n ", sizeof(VL53L5CX_ResultsData));
    printf("The avarege time spent reading a messurement %f seconds\n",( time_spent/(FREQ_HZ * NUM_OF_SEC) ));


    for (u_int8_t i2c_bus = 0; i2c_bus < E_I2C_BUS_NUM; i2c_bus++)
    {
        for (u_int8_t index =0; index < PRX_NUM; index++)
        {
            status = vl53l5cx_stop_ranging(&myDev[i2c_bus][index]);
            printf("End of ULD demo\n");


            vl53l5cx_comms_close(&myDev[i2c_bus][index].platform);
            printf("closed device on address %d", myDev[i2c_bus][index].platform.address);
        }
    }

    return 0;
}


void ranging_loop(u_int8_t i2c_bus,u_int8_t prox_index) {
    /*********************************/
    /*   VL53L5CX ranging variables  */
    /*********************************/
    uint8_t 				i, j, n,index;
    VL53L5CX_ResultsData 	Results;		/* Results data from VL53L5CX */
    uint8_t status;
    clock_t begin = clock();

    status = vl53l5cx_get_ranging_data(&myDev[i2c_bus][prox_index], &Results);

    printf("Print data [%d.%d] No : %3u status %d \n",i2c_bus,prox_index, myDev[i2c_bus][prox_index].streamcount, status);
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
}
