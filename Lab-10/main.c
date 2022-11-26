/*
 * Copyright (c) 2015,
 * National Instruments Corporation.
 * All rights reserved.
 */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "AIO.h"
#include "MyRio.h"
#if !defined(LoopDuration)
#define LoopDuration    60  /* How long to output the signal, in seconds */
#endif

/**
 * Overview:
 * Demonstrates using the analog input and output (AIO). Reads initial values
 * of two analog input channels from connector A and writes the difference of the read
 * values on connector B. Prints the values to the console.
 *
 * Instructions:
 * 1. Connect a DC voltage input (between 0 V and 5 V) to AI0 on connector A.
 * 2. Connect a DC voltage input (between 0 V and 5 V) to AI3 on connector A.
 * 3. Connect a voltmeter to AO0 on connector B.
 * 4. Run this program.
 *
 * Output:
 * The program reads the initial voltage on AI0 and AI3 on connector A, and
 * writes the difference to AO0 on connector B. The output is maintained for 60 s.
 * Input voltages and the difference are written to the console.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the myRIO-1900.
 * Change the preprocessor symbol to use this example with the myRIO-1950.
 */
int main(int argc, char **argv)
{
    NiFpga_Status status;
    double ai_A0;
    double ai_A3;
    double ao_B0;
    double positionV;
    double positionDeg;
    double outputV = 2;
    double initialDegree;
    double targetDegree;
    //2.5,0.1,2.3stop
    //2 0.005 0.9
    double kp =2.6;
    double ki=0.005;
    double kd =0.9;
    double add;
    double a,b,c = 0;
    double errorC,errorP,T;
    double PID;
    double CurrentDegree;

    MyRio_Aio CO;
    MyRio_Aio A0;
    MyRio_Aio A3;
    MyRio_Aio B0;
    MyRio_Aio CO0;
    MyRio_Aio CI0;

    time_t currentTime;
    time_t finalTime;
    printf("AnalogInputOutput\n");
    /*
     * Specify the registers that correspond to the AIO channel
     * that needs to be accessed.
     *
     * Assign constants to the weight and offset of the connector.
     *
     * For connector A and B the value of AI and AO is between 0 and 5v,
     * and the is_signed flag should be False; for connecor C, the range
     * is -10~10v, and the is_signed flag should be True.
     */
//    A0.val = AIA_0VAL;

    A0.val = NiFpga_MyRio1900Fpga60_IndicatorU16_AIA_0VAL;
    A0.wght = AIA_0WGHT;
    A0.ofst = AIA_0OFST;
    A0.is_signed = NiFpga_False;
    //A3.val = AIA_3VAL;
    A3.val = NiFpga_MyRio1900Fpga60_IndicatorU16_AIA_3VAL;
    A3.wght = AIA_3WGHT;
    A3.ofst = AIA_3OFST;
    A3.is_signed = NiFpga_False;
    //B0.val = AOB_0VAL;
    B0.val = NiFpga_MyRio1900Fpga60_ControlU16_AOB_0VAL;
    B0.wght = AOB_0WGHT;
    B0.ofst = AOB_0OFST;
    //B0.set = AOSYSGO;
    B0.set = NiFpga_MyRio1900Fpga60_ControlBool_AOSYSGO;
    B0.is_signed = NiFpga_False;
    CI0.val = AIC_0VAL;
    CI0.wght = AIC_0WGHT;
    CI0.ofst = AIC_0OFST;
    CI0.is_signed = NiFpga_True;
    CO0.val = AOC_0VAL;
    CO0.wght = AOC_0WGHT;
    CO0.ofst = AOC_0OFST;
    CO0.set = NiFpga_MyRio1900Fpga60_ControlBool_AOSYSGO;// AOSYSGO;
    CO0.is_signed = NiFpga_True;

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
     * Read the scaling factors, initializing the struct
     */
    Aio_Scaling(&A0);
    Aio_Scaling(&A3);
    Aio_Scaling(&B0);
    Aio_Scaling(&CO0);
    Aio_Scaling(&CI0);

    /*
     * Read from AI channels A/AI0 and A/AI3.
     * Each channel is on Connector A.
     */
    ai_A0 = Aio_Read(&A0);
    ai_A3 = Aio_Read(&A3);
    positionV = Aio_Read(&CI0);
    positionDeg = positionV*176/5.0;
    /*
     * Print out the logic level of each channel.
     */
    //printf("A/AI0 = %f\n", ai_A0);
    //printf("A/AI3 = %f\n", ai_A3);
    printf("PositionV = %f\n", positionV);
    printf("PositionDeg = %f\n",positionDeg);
    /*
     * Calculate the difference between the channels
     */
    ao_B0 = ai_A0 - ai_A3;
    /*
     * Print out the desired logic level of B/AO0.
     * AO on connector A and B supports 0~5v, and if the value is negative
     * the output voltage will be 0v.
     */
    //printf("B/AO0 = %f\n", (ao_B0 > 0 ? ao_B0 : 0));
    /*
     * Write to channel B/AO0 to set it to the desired value.
     */
    //Aio_Write(&B0, ao_B0);
    if (outputV>6.0)
        outputV = 6.0;
    if (outputV<-6.0)
        outputV=-6.0;
    //Aio_Write(&CO0, outputV);
    /*
     * Normally, the main function runs a long running or infinite loop.
     * Keep the program running so that you can measure the output from B7 using
     * an external instrument.
     */
    //period T = 1 ms
    T = 0.001;
    add = -90;

    positionV = Aio_Read(&CI0);
    positionDeg = positionV*176/5.0;
    initialDegree = (Aio_Read(&CI0))*(176/5.0);
    targetDegree = initialDegree + add; //+/- 90 180
    if ((add==180)&&(targetDegree > 180)){
        targetDegree = (initialDegree - 180);
    }
    if ((add==-180)&&(targetDegree <- 180)){
        targetDegree = (initialDegree + 180);
    }

/*
    if ((add==180)&&(targetDegree+add)>180){
            targetDegree = targetDegree-180;
    }
    if ((add==-180)&&(targetDegree-add)<-180){
            targetDegree = targetDegree+180;
    }

    printf("Current Pos: %f \n", CurrentDegree);
    printf("Target pos: %f \n ", targetDegree);
*/
    errorC = targetDegree - initialDegree; //current direction and

    //what if errorc is error being AIN and AIO? wrong b/c of whileloop condition is errorC>0.5
    errorP = 0;
    while (abs(errorC)>0.5){
        positionV = Aio_Read(&CI0);
        CurrentDegree = positionV*176/5.0;
        a = kp*errorC;
        b = b + ki*T*errorC;
        c = kd/T*(errorC - errorP);
        //b = c = 0;
        //PID = ki + kp +kd;
        PID = a+b+c;
        //outputV = (targetDegree - CurrentDegree) * PID *5/176; // DEG * V / DEG
        outputV = PID*5/176;
        printf("a = %f\n,b = %f\n, c = %f\n",a,b,c);
        errorC = (targetDegree - CurrentDegree); //V/DEG
        printf("Current Pos: %f \n", CurrentDegree);
        printf("Target pos: %f \n ", targetDegree);
        printf("kp = %f, ki = %f, kd = %f \n",kp,ki,kd);
        printf("PID = %f \n",PID);
        printf("ErrorC %f \n", errorC);
        printf("ErrorP %f \n", errorP);
        if (outputV>6.0)
            outputV = 6.0;
        if (outputV<-6.0)
            outputV=-6.0;
        printf("outputV: %f \n",outputV);
        Aio_Write(&CO0, outputV);
        errorP = errorC;
    }
    printf("ErrorC %f \n", errorC);

    time(&currentTime);
    finalTime = currentTime + LoopDuration;
    while (currentTime < finalTime)
    {
        time(&currentTime);
    }
    /*
     * Close the myRIO NiFpga Session.
     * This function MUST be called after all other functions.
     */
    status = MyRio_Close();
    /*
     * Returns 0 if successful.
     */
    return status;
}
