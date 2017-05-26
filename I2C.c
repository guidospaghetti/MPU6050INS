#include <msp430.h>
#include "I2C.h"

unsigned char RX_Data[6];
unsigned char TX_Data[2];
unsigned char RX_ByteCtr;
unsigned char TX_ByteCtr;
unsigned char slaveAddress;
unsigned char repeatedStart;

void i2cInit(void)
{
	// set up I2C module
	UCB0CTL1 |= UCSWRST;							// Enable SW reset
	UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;			// I2C Master, synchronous mode
	UCB0CTL1 = UCSSEL_2 + UCSWRST;					// Use SMCLK, keep SW reset
	UCB0BR0 = 120;									// fSCL = SMCLK/(120 + 0*256) = ~100kHz
	UCB0BR1 = 0;
	UCB0CTL1 &= ~UCSWRST;							// Clear SW reset, resume operation
}

void i2cWrite(unsigned char address)
{
	__disable_interrupt();
	UCB0I2CSA = address;							// Load slave address
	UCB0IE |= UCTXIE;								// Enable TX interrupt
	while(UCB0CTL1 & UCTXSTP);						// Ensure stop condition sent
	UCB0CTL1 |= UCTR + UCTXSTT;						// TX mode and START condition
	__bis_SR_register(CPUOFF + GIE);				// sleep until UCB0TXIFG is set ...
}

void i2cRead(unsigned char address)
{
	__disable_interrupt();
	UCB0I2CSA = address;							// Load slave address
	UCB0IE |= UCRXIE;								// Enable RX interrupt
	if (repeatedStart != 0x01)
		while(UCB0CTL1 & UCTXSTP);					// Ensure stop condition sent
	UCB0CTL1 &= ~UCTR;								// RX mode
	UCB0CTL1 |= UCTXSTT;							// Start Condition
    __bis_SR_register(CPUOFF + GIE);				// sleep until UCB0RXIFG is set ...
}

/**********************************************************************************************/
// USCIAB0TX_ISR
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
{
	if (repeatedStart != 0x01)
	{
		if(UCB0CTL1 & UCTR)							// TX mode (UCTR == 1)
		{
			if (TX_ByteCtr)				  	  	    // TRUE if more bytes remain
			{
				TX_ByteCtr--;						// Decrement TX byte counter
				UCB0TXBUF = TX_Data[TX_ByteCtr];	// Load TX buffer
			}
			else									// no more bytes to send
			{
				UCB0CTL1 |= UCTXSTP;				// I2C stop condition
				UCB0IFG &= ~UCTXIFG;				// Clear USCI_B0 TX int flag
				__bic_SR_register_on_exit(CPUOFF);	// Exit LPM0
			}
		}
		else // (UCTR == 0)							// RX mode
		{
			RX_ByteCtr--;					        // Decrement RX byte counter
			if (RX_ByteCtr)					        // RxByteCtr != 0
			{
				RX_Data[RX_ByteCtr] = UCB0RXBUF;	// Get received byte
				if (RX_ByteCtr == 1)				// Only one byte left?
				UCB0CTL1 |= UCTXSTP;				// Generate I2C stop condition
			}
			else									// RxByteCtr == 0
			{
				RX_Data[RX_ByteCtr] = UCB0RXBUF;	// Get final received byte
				__bic_SR_register_on_exit(CPUOFF);	// Exit LPM0
			}
		}

	}
	else if (repeatedStart == 0x01)
	{
		if(UCB0CTL1 & UCTR)							// TX mode (UCTR == 1)
		{
			if (TX_ByteCtr)				    	    // TRUE if more bytes remain
			{
				TX_ByteCtr--;						// Decrement TX byte counter
				UCB0TXBUF = TX_Data[TX_ByteCtr];	// Load TX buffer
			}
			else									// no more bytes to send
			{
				//UCB0CTL1 |= UCTXSTP;				// I2C stop condition
				UCB0IFG &= ~UCTXIFG;				// Clear USCI_B0 TX int flag
				__bic_SR_register_on_exit(CPUOFF);	// Exit LPM0
			}
		}
		else // (UCTR == 0)							// RX mode
		{
			RX_ByteCtr--;				  		    // Decrement RX byte counter
			if (RX_ByteCtr)				  		    // RxByteCtr != 0
			{
				RX_Data[RX_ByteCtr] = UCB0RXBUF;	// Get received byte
				if (RX_ByteCtr == 1)				// Only one byte left?
				UCB0CTL1 |= UCTXSTP;				// Generate I2C stop condition
			}
			else									// RxByteCtr == 0
			{
				RX_Data[RX_ByteCtr] = UCB0RXBUF;	// Get final received byte
				__bic_SR_register_on_exit(CPUOFF);	// Exit LPM0
			}
		}
	}
}




