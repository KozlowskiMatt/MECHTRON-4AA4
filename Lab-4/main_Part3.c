/*
 * Copyright (c) 2015,
 * National Instruments.
 * All rights reserved.
 */

#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "TimerIRQ.h"
#include "MyRio.h"
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



#if !defined(LoopDuration)
#define LoopDuration          60   /* How long to monitor the signal, in seconds */
#endif


#if !defined(LoopSteps)
#define LoopSteps             3    /* How long to step between printing, in seconds */
#endif


extern NiFpga_Session myrio_session;

/*
 * Resources for the new thread.
 */
typedef struct
{
    NiFpga_IrqContext irqContext;      /* IRQ context reserved by Irq_ReserveContext() */
    NiFpga_Bool irqThreadRdy;           /* IRQ thread ready flag */
} ThreadResource;


/**
 * Overview:
 * Demonstrates how to use the timer IRQ. Once the timer IRQ occurs (5 s), print
 * the IRQ number, triggered times and main loop count number in the console.
 * The timer IRQ will be triggered only once in this example.
 * The output is maintained for 60 s.
 *
 * Instructions:
 * Run this program and observe the console.
 *
 * Output:
 * IRQ0, triggered times and main loop count number are shown in the console,
 * The output is maintained for 60 s.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the myRIO-1900.
 * Change the preprocessor symbol to use this example with the myRIO-1950.
 */
void *Timer_Irq_Thread(void* resource)
{
	int32_t status;

	    MyRio_IrqTimer irqTimer0;
	    ThreadResource irqThread0;

	    pthread_t thread;

	    time_t currentTime;
	    time_t finalTime;
	    time_t printTime;
	    const uint32_t timeoutValue = 5000000;
	    int32_t x;

	    irqTimer0.timerWrite = IRQTIMERWRITE;
	    irqTimer0.timerSet = IRQTIMERSETTIME;


    ThreadResource* threadResource = (ThreadResource*) resource;
    while (1)
    {
        uint32_t irqAssert = 0;
        static uint32_t irqCount = 0;

        status = Irq_RegisterTimerIrq(&irqTimer0, &irqThread0.irqContext,
                                                  timeoutValue);


        /*
         * Stop the calling thread, wait until a selected IRQ is asserted.
         */
        Irq_Wait(threadResource->irqContext, TIMERIRQNO,
                 &irqAssert, (NiFpga_Bool*) &(threadResource->irqThreadRdy));

        /*
         * If an IRQ was asserted.
         */
        if (irqCount % 2 == 0){
        	 NiFpga_WriteU8(myrio_session,DOLED30,7);
        }
        else{
        	 NiFpga_WriteU8(myrio_session,DOLED30,0);
        }




        if (irqAssert & (1 << TIMERIRQNO))
        {
            printf("IRQ%d,%d\n", TIMERIRQNO, ++irqCount);




            /*
             * Acknowledge the IRQ(s) when assertion is done.
             */
            Irq_Acknowledge(irqAssert);
        }


        status = Irq_UnregisterTimerIrq(&irqTimer0, irqThread0.irqContext);

        //NiFpga_WriteU8(myrio_session,DOLED30,0);

        /*
         * Check the indicator to see if the new thread is stopped.
         */
        if (!(threadResource->irqThreadRdy))
        {
            printf("The IRQ thread ends.\n");
            break;
        }
    }

    /*
     * Exit the new thread.
     */
    pthread_exit(NULL);

    return NULL;
}


int main(int argc, char **argv)
{







    int32_t status;

    MyRio_IrqTimer irqTimer0;
    ThreadResource irqThread0;

    pthread_t thread;

    time_t currentTime;
    time_t finalTime;
    time_t printTime;

    /*
     * Configure the timer IRQ and set the time interval. The IRQ occurs after the time interval.
     */
    const uint32_t timeoutValue = 5000000;

    printf("Timer IRQ:\n");

    /*
     * Specify the registers that correspond to the IRQ channel
     * that you need to access.
     */
    irqTimer0.timerWrite = IRQTIMERWRITE;
    irqTimer0.timerSet = IRQTIMERSETTIME;

    /*
     * Open the myRIO NiFpga Session.
     * You must use this function before using all the other functions. 
     * After you finish using this function, the NI myRIO target is ready to be used.
     */
    status = MyRio_Open();
    if (MyRio_IsNotSuccess(status))
    {
        return status;
    }

    /*
     * Configure the timer IRQ. The Time IRQ occurs only once after Timer_IrqConfigure().
     * If you want to trigger the timer IRQ repeatedly, use this function every time you trigger the IRQ.
     * Or you can put the IRQ in a loop.
     * Return a status message to indicate if the configuration is successful. The error code is defined in IRQConfigure.h.
     */

    //status = Irq_RegisterTimerIrq(&irqTimer0, &irqThread0.irqContext,timeoutValue);

    /*
     * Terminate the process if it is unsuccessful.
     */
    if (status != NiMyrio_Status_Success)
    {
        printf("CONFIGURE ERROR: %d, Configuration of Timer IRQ failed.", 
            status);

        return status;
    }

    /*
     * Set the indicator to allow the new thread.
     */
    irqThread0.irqThreadRdy = NiFpga_True;

    /*
     * Create new threads to catch the specified IRQ numbers.
     * Different IRQs should have different corresponding threads.
     */
    status = pthread_create(&thread, NULL, Timer_Irq_Thread, &irqThread0);
    if (status != NiMyrio_Status_Success)
    {
        printf("CONFIGURE ERROR: %d, Failed to create a new thread!", 
            status);

        return status;
    }

    /*
     * Normally, the main function runs a long running or infinite loop.
     * Read the console output for 60 seconds so that you can recognize the
     * explanation and loop times.
     */
    time(&currentTime);
    finalTime = currentTime + LoopDuration;
    printTime = currentTime;
    int counter = 0;
    while (currentTime < finalTime)
    {
        static uint32_t loopCount = 0;
        time(&currentTime);

        /* Don't print every loop iteration. */
        if (currentTime > printTime)
        {
            printf("main loop,%d\n", ++loopCount);
            //printf("")
        	//NiFpga_WriteU8(myrio_session,DOLED30,counter);
            printTime += LoopSteps;
        }
    }

    /*
     * Set the indicator to end the new thread.
     */
    irqThread0.irqThreadRdy = NiFpga_False;

    /*
     * Wait for the end of the IRQ thread.
     */
    pthread_join(thread, NULL);

    /*
     * Disable timer interrupt, so you can configure this I/O next time.
     * Every IrqConfigure() function should have its corresponding clear function,
     * and their parameters should also match.
     */
    //status = Irq_UnregisterTimerIrq(&irqTimer0, irqThread0.irqContext);
    if (status != NiMyrio_Status_Success)
    {
        printf("CONFIGURE ERROR: %d\n", status);
        printf("Clear configuration of Timer IRQ failed.");
        return status;
    }

    /*
     * Close the myRIO NiFpga Session.
     * You must use this function after using all the other functions.
     */
    status = MyRio_Close();

    /*
     * Returns 0 if successful.
     */
    return status;
}
