/*
 * GeneralFunctions.h
 *
 *  Created on: Apr 20, 2017
 *      Author: Brian
 */

#ifndef GENERALFUNCTIONS_H_
#define GENERALFUNCTIONS_H_

#include <stdint.h>

/*
 * @fn LED1_INIT(void)
 * @brief Initializes the red LED
*/
#define LED1_INIT() P1SEL &= ~BIT0; P1DIR |= BIT0; P1OUT &= ~BIT0

/*
 * @fn LED2_INIT(void)
 * @brief Initializes the green LED
 */
#define LED2_INIT() P4SEL &= ~BIT7; P4DIR |= BIT7; P4OUT &= ~BIT7

/*
 * @fn LED1_ON(void)
 * @brief Turns the red LED on
 */
#define LED1_ON() P1OUT |= BIT0

/*
 * @fn LED2_ON(void)
 * @brief Turns the green LED on
 */
#define LED2_ON() P4OUT |= BIT7

/*
 * @fn LED1_OFF(void)
 * @brief Turns the red LED off
 */
#define LED1_OFF() P1OUT &= ~BIT0

/*
 * @fn LED2_OFF(void)
 * @brief Turns the green LED off
 */
#define LED2_OFF() P4OUT &= ~BIT7

/*
 * @fn DelayInit(void)
 * @brief Initializes the clock so that delayms can be used
 */
void DelayInit(void);

/*
 * @fn delayms(uint16_t ms)
 * @param ms Delay time in ms (stored as a 16-bit integer)
 * @brief Halts the processor for the specified amount of time
 */
void delayms(uint16_t);

#endif /* GENERALFUNCTIONS_H_ */
