/*
 * Copyright (c) 2015,
 * National Instruments Corporation.
 * All rights reserved.
 */

#if !defined(LoopDuration)
#define LoopDuration    10  /* How long to output the signal, in seconds */
#endif

#include "DIO.h"
#include <stdio.h>
#include "MyRio.h"
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sched.h>
#include <sys/mman.h>
#include <string.h>

#define MY_PRIORITY (49) /* we use 49 as the PRREMPT_RT use 50
                            as the priority of kernel tasklets
                            and interrupt handler by default */

#define MAX_SAFE_STACK (8*1024) /* The maximum stack size which is
                                   guaranteed safe to access without
                                   faulting */

#define NSEC_PER_SEC    (1000000000) /* The number of nsecs per sec. */
extern NiFpga_Session myrio_session;
void stack_prefault(void) {

        unsigned char dummy[MAX_SAFE_STACK];

        memset(dummy, 0, MAX_SAFE_STACK);
        return;
}



/**
 * Overview:
 * myRIO main function. This template contains basic code to open the myRIO
 * FPGA session. You can remove this code if you only need to use the UART.
 *
 * Code in myRIO example projects is designed for reuse. You can copy source
 * files from the example project to use in this template.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the myRIO-1900.
 * Change the preprocessor symbol to use this example with the myRIO-1950.
 */
int main(int argc, char **argv)
{
    NiFpga_Status status;

    /*
     * Open the myRIO NiFpga Session.
     * This function MUST be called before all other functions. After this call
     * is complete the myRIO target will be ready to be used.
     */
    status = MyRio_Open();
    if (MyRio_IsNotSuccess(status))
    {
        return status;
    }

    /*
     * Your application code goes here.
     */

    /*
     * Close the myRIO NiFpga Session.
     * This function MUST be called after all other functions.
     */


    struct timespec t;
            struct sched_param param;
            int interval = 50000000; // changed from 5 Determines the time period of the task

    // required for looping for a set time
            time_t currentTime;
            time_t finalTime;

            /* Declare ourself as a real time task */

            param.sched_priority = MY_PRIORITY;
            if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
                    perror("sched_setscheduler failed");
                    exit(-1);
            }

            /* Lock memory */

            if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
                    perror("mlockall failed");
                    exit(-2);
            }

            /* Pre-fault our stack */

            stack_prefault();

            clock_gettime(CLOCK_MONOTONIC ,&t);
            /* start after one second */
            t.tv_sec++;

         //   while(1) {
            	/*
            	     * Normally, the main function runs a long running or infinite loop.
            	     * A finite loop is used for convenience in the labs.
            	     */
    	    time(&currentTime);
    	    finalTime = currentTime + LoopDuration;
    	    int i = 0;
    	    while (currentTime < finalTime){
                /* wait until next shot */
                // if t is less than the current time, no sleep occur
                clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

                NiFpga_WriteU8(myrio_session,DOLED30,i);
                i+=1;
                i=i%2;
                printf("Hello\n");

                /* calculate next shot */
                t.tv_nsec += interval;

                while (t.tv_nsec >= NSEC_PER_SEC) {
                       t.tv_nsec -= NSEC_PER_SEC;
                        t.tv_sec++;
                }
                time(&currentTime);
            }
    	    status = MyRio_Close();
        return 0;



    //return status;
}
