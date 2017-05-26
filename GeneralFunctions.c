/*
 * GeneralFunctions.c
 *
 *  Created on: Apr 20, 2017
 *      Author: Brian
 */

#include "GeneralFunctions.h"
#include <msp430.h>
#include <stdint.h>

static void SetClk12MHz(void);

// time in uints of 10 us
uint64_t time = 0;

void DelayInit(void){
    SetClk12MHz();

    TA0CTL = TASSEL__SMCLK + ID_0 + MC__UP;
    TA0CCTL0 = OUTMOD_3 + CCIE;
    TA0CCR0 = 120;

}

void delayms(uint16_t ms){
    // 1,000 ms per sec
    // 12,000,000 clocks per sec
    // 12,000,000 / 1,000 = 12,000 clocks per ms
    while(ms--){
        __delay_cycles(12000);
    }
}

/*
 * The following function is used to set the
 * MSP430's clock speed to 12MHz.
 * It was obtained from the example at:
 * https://e2e.ti.com/support/microcontrollers/msp430/f/166/p/145592/530948
 */

static void SetClk12MHz(void){
    WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT
    UCSCTL3 |= SELREF_2;                      // Set DCO FLL reference = REFO
    UCSCTL4 |= SELA_2;                        // Set ACLK = REFO
    __bis_SR_register(SCG0);                  // Disable the FLL control loop
    UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
    UCSCTL1 = DCORSEL_5;                      // Select DCO range 24MHz operation
    UCSCTL2 = FLLD_1 + 374;                 // Set DCO Multiplier for 12MHz
                                                // (N + 1) * FLLRef = Fdco
                                                // (374 + 1) * 32768 = 12MHz
                                                // Set FLL Div = fDCOCLK/2
      __bic_SR_register(SCG0);                  // Enable the FLL control loop

      // Worst-case settling time for the DCO when the DCO range bits have been
      // changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
      // UG for optimization.
      // 32 x 32 x 12 MHz / 32,768 Hz = 375000 = MCLK cycles for DCO to settle
      __delay_cycles(375000);//
      // Loop until XT1,XT2 & DCO fault flag is cleared
      do
      {
        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                                // Clear XT2,XT1,DCO fault flags
        SFRIFG1 &= ~OFIFG;                      // Clear fault flags
      }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag
}

uint64_t timeNow(void) {
	return time;
}

uint64_t timeSince(uint64_t prevTime) {
	return time - prevTime;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
	time++;
}
