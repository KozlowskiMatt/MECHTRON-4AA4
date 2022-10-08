/*
 * Copyright (c) 2015,
 * National Instruments Corporation.
 * All rights reserved.
 */



//#define GNU_SOURCE //Must be defined as a symbol in properties

#include "DIO.h"
#include <stdio.h>
#include "MyRio.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sched.h>
#include <sys/mman.h>
#include <string.h>


#define MY_PRIORITY1 (49) /* we use 49 as the PRREMPT_RT use 50
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

void* tfun1(void*n){
	struct timespec t;
  struct sched_param param;
  int interval = 500000000; /* 50us*/
  int num_runs = 10; // replaced infinite loop

  /* Declare ourself as a real time task */

  param.sched_priority = MY_PRIORITY1;
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
  int old=(int)pthread_self();
  int current;
  int counter  = 0;
  int i=0;
  while(num_runs) {
    /* wait until next shot */
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

    /* do the stuff */
    //NiFpga_WriteU8(myrio_session,DOLED30,i);
	/* do the stuff */
   // printf("%d",i);

    if (counter%2 == 0)
    	//printf("%d",0);
    	NiFpga_WriteU8(myrio_session,DOLED30,0);
    else
        NiFpga_WriteU8(myrio_session,DOLED30,2);
    counter++;
    /*


    printf("Hello! This is thread: %u Priority:%d\n", (int)pthread_self(), param.sched_priority);
   // pthread_t pthread_self()

   // this return current pthread_t, which is thread id, you can convert it to type "unsigned int",


    /* calculate next shot */
    t.tv_nsec += interval;

    while (t.tv_nsec >= NSEC_PER_SEC) {
           t.tv_nsec -= NSEC_PER_SEC;
            t.tv_sec++;
    }
    num_runs = num_runs -1;
  }
  return NULL;
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
     *
     *
     *
     *
     *
     *
     *
     */
    pthread_t tid1, tid2, tid3;
       cpu_set_t cpus;
       // Force the program to run on one cpu,
       CPU_ZERO(&cpus); //Initialize cpus to nothing clear previous info if any
       CPU_SET(0, &cpus); // Set cpus to a cpu number zeor here

       if (sched_setaffinity(0, sizeof(cpu_set_t), &cpus)< 0)
          perror("set affinity");

       pthread_create(&tid1, NULL, tfun1, NULL);
       pthread_create(&tid2, NULL, tfun1, NULL);
       pthread_create(&tid3, NULL, tfun1, NULL);

       pthread_join(tid1, NULL);
       pthread_join(tid2, NULL);
       pthread_join(tid3, NULL);


       return 0;







    /*
     * Close the myRIO NiFpga Session.
     * This function MUST be called after all other functions.
     */
    status = MyRio_Close();

    return status;
}
