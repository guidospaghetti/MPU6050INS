#include <msp430.h> 
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "I2C.h"
#include "MPU6050.h"
#include "GeneralFunctions.h"
#include "MpuUtil.h"


#define SAMPLES_FOR_AVG		100
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
	float errorX = 0, errorY = 0, errorZ = 0;
	float accX[SAMPLES_FOR_AVG], accY[SAMPLES_FOR_AVG], accZ[SAMPLES_FOR_AVG];
	uint64_t time, prevTime = 0;
	float timeDiff;
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

	for (i = 0; i < SAMPLES_FOR_AVG; i++) {
		accX[i] = readMeasurement(ACCEL_X);
		accY[i] = readMeasurement(ACCEL_Y);
		accZ[i] = readMeasurement(ACCEL_Z);
		errorX += accX[i];
		errorY += accY[i];
		errorZ += accZ[i];
		delayms(1);
	}

	errorX = errorX / SAMPLES_FOR_AVG;
	errorY = errorY / SAMPLES_FOR_AVG;
	errorZ = errorZ / SAMPLES_FOR_AVG;

	errorX -= initX;
	errorY -= initY;
	errorZ -= initZ;

    while(1) {
    	accXAvg = readMeasurement(ACCEL_X);
    	accYAvg = readMeasurement(ACCEL_Y);
    	accZAvg = readMeasurement(ACCEL_Z);
    	time = timeNow();

    	accXAvg -= (initX + errorX);
    	accYAvg -= (initY + errorY);
    	accZAvg -= (initZ + errorZ);

//    	if (fabsf(accXAvg) < .1) {
//    		accXAvg = 0;
//    	}
//    	if (fabsf(accYAvg) < .1) {
//    		accYAvg = 0;
//    	}
//    	if (fabsf(accZAvg) < .1) {
//    		accZAvg = 0;
//    	}

    	if (prevTime > 0) {
			timeDiff = timeSince(prevTime) / 100000.0f;

			veloX += (accXAvg * (timeDiff * 9.81f));
			veloY += (accYAvg * (timeDiff * 9.81f));
			veloZ += (accZAvg * (timeDiff * 9.81f));

			distX += (veloX * (timeDiff));
			distY += (veloY * (timeDiff));
			distZ += (veloZ * (timeDiff));

			if (count == 100) {
				count = 0;
				//printf("%f\t %f\t %f\t %f;\n", accXAvg, accYAvg, accZAvg, timeDiff);
			}


    	}
    	printf("%f\t %f\t %f\t %f;\n", accXAvg, accYAvg, accZAvg, timeDiff);
    	prevTime = time;
    	count++;
    }

	return 0;
}
