/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR( void );

/* ----------------------- Start implementation -----------------------------*/
/**
 * \todo Add different apb1 psc values
 */
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    TIM3->PSC = (PCLK1_FREQ / 500000) - 1;
    TIM3->ARR = usTim1Timerout50us - 1;
    TIM3->EGR |= TIM_EGR_UG; // Generate an event, so that ARR value is loaded
    return TRUE;
}


inline void
vMBPortTimersEnable(  )
{
    TIM3->CR1 |= TIM_CR1_CEN;
}

inline void
vMBPortTimersDisable(  )
{
    TIM3->CR1 &= ~TIM_CR1_CEN;
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */

void TIM3_IRQHandler(void) {
    TIM3->SR &= ~(TIM_SR_UIF); // Reset the timer status
    GPIOC->ODR ^= GPIO_ODR_ODR13;
    //prvvTIMERExpiredISR();
}

static void prvvTIMERExpiredISR( void )
{
    ( void )pxMBPortCBTimerExpired(  );
}

