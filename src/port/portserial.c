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

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    if (xRxEnable) {
        USART1->CR1 |= USART_CR1_RE;
        USART1->CR1 |= USART_CR1_RXNEIE;
    } else {
        USART1->CR1 &= ~USART_CR1_RE;
        USART1->CR1 &= ~USART_CR1_RXNEIE;
    }
    if (xTxEnable) {
        USART1->CR1 |= USART_CR1_TE;
        USART1->CR1 |= USART_CR1_TXEIE;
    } else {
        // I added this code!
        while (!(USART1->SR & USART_SR_TC_Msk)) {
            __NOP();
        }
        USART1->CR1 &= ~USART_CR1_TE;
        USART1->CR1 &= ~USART_CR1_TXEIE;
    }
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    RCC->APB2ENR |= (RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN);

    GPIOA->CRH &= ~(GPIO_CRH_CNF9_Msk | GPIO_CRH_MODE9_Msk);
    GPIOA->CRH |= (GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9_1);

    GPIOA->CRH &= ~(GPIO_CRH_CNF10 | GPIO_CRH_MODE10);
    GPIOA->CRH |= GPIO_CRH_CNF10_1;  // Input floating

    if (eParity == MB_PAR_EVEN) {
        //USART1->CR1 |= USART_CR1_PS;
        USART1->CR1 |= USART_CR1_PCE;
    } else {
        return FALSE; // Not implemented.
    }

    if (ucDataBits == 9) {
        USART1->CR1 |= USART_CR1_M;
    } else if (ucDataBits != 8) {
        return FALSE;
    }

    USART1->BRR = PCLK2_FREQ / ulBaudRate;
    USART1->CR1 |= USART_CR1_UE;
    NVIC_SetPriority(USART1_IRQn, 0);
    NVIC_EnableIRQ(USART1_IRQn);
    return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    USART1->DR = ucByte;
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = USART1->DR;
    return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}

/* System ISR */

void USART1_IRQHandler(void) {
    if (USART1->SR & USART_SR_RXNE_Msk) {
        if (USART1->SR & (USART_SR_FE_Msk | USART_SR_NE_Msk)) {
            __NOP();
        }
        prvvUARTRxISR();
        GPIOC->ODR ^= GPIO_ODR_ODR13;
    } else if (USART1->SR & USART_SR_TXE_Msk) {
        prvvUARTTxReadyISR();
    }
}
