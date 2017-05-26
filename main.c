#include <msp430.h> 
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "I2C.h"
#include "MPU6050.h"
#include "GeneralFunctions.h"
#include "MpuUtil.h"


#define SAMPLES_FOR_AVG		5
#define MS					1
/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	float accXAvg = 0, accYAvg = 0, accZAvg = 0;
	float veloX = 0, veloY = 0, veloZ = 0;
	float distX = 0, distY = 0, distZ = 0;
	float initX = 0, initY = 0, initZ = 0;
	float accX[5], accY[5], accZ[5];
    volatile uint8_t i;
    volatile uint8_t count = 0;

    mpuInit();

    DelayInit();

	for (i = 0; i < SAMPLES_FOR_AVG; i++) {
		accX[i] = readMeasurement(ACCEL_X);
		accY[i] = readMeasurement(ACCEL_Y);
		accZ[i] = readMeasurement(ACCEL_Z);
		initX += accX[i];
		initY += accY[i];
		initZ += accZ[i];
		delayms(1);
	}

	initX = initX / SAMPLES_FOR_AVG;
	initY = initY / SAMPLES_FOR_AVG;
	initZ = initZ / SAMPLES_FOR_AVG;

    while(1) {
//    	for (i = 0; i < SAMPLES_FOR_AVG; i++) {
//    		accX[i] = readMeasurement(ACCEL_X);
//    		accY[i] = readMeasurement(ACCEL_Y);
//    		accZ[i] = readMeasurement(ACCEL_Z);
//    		accXAvg += accX[i];
//    		accYAvg += accY[i];
//    		accZAvg += accZ[i];
//    		delayms(1);
//    	}


//    	accXAvg = accXAvg / SAMPLES_FOR_AVG;
//    	accYAvg = accYAvg / SAMPLES_FOR_AVG;
//    	accZAvg = accZAvg / SAMPLES_FOR_AVG;
    	accXAvg = readMeasurement(ACCEL_X);
    	accYAvg = readMeasurement(ACCEL_Y);
    	accZAvg = readMeasurement(ACCEL_Z);

    	accXAvg -= initX;
    	accYAvg -= initY;
    	accZAvg -= initZ;

//    	if (fabsf(accXAvg) < .05) {
//    		accXAvg = 0;
//    	}
//    	if (fabsf(accYAvg) < .05) {
//    		accYAvg = 0;
//    	}
//    	if (fabsf(accZAvg) < .05) {
//    		accZAvg = 0;
//    	}

    	veloX += (accXAvg * (MS / 1000.0f * 9.81f));
    	veloY += (accYAvg * (MS / 1000.0f * 9.81f));
    	veloZ += (accZAvg * (MS / 1000.0f * 9.81f));

    	distX += (veloX * (MS / 1000.0f));
    	distY += (veloY * (MS / 1000.0f));
    	distZ += (veloZ * (MS / 1000.0f));

    	if (count == 250) {
    		count = 0;
    		printf("%f\t %f\t %f\n", distX, distY, distZ);
    	}
    	count++;
    	delayms(1);
    }

	return 0;
}
