/*******************************************************************************
  TMR Peripheral Library Interface Source File

  Company
    Microchip Technology Inc.

  File Name
    plib_tmr5.c

  Summary
    TMR5 peripheral library source file.

  Description
    This file implements the interface to the TMR peripheral library.  This
    library provides access to and control of the associated peripheral
    instance.

*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "device.h"
#include "plib_tmr5.h"
#include "interrupts.h"



void TMR5_Initialize(void)
{
    /* Disable Timer */
    T5CONCLR = _T5CON_ON_MASK;

    /*
    SIDL = 1
    SYNC = 0
    TGATE = 0
    TCKPS =0
    T32   = 1
    TCS = 0
    */
    T5CONSET = 0x2008;

    /* Clear counter */
    TMR5 = 0x0;

    /*Set period */
    PR5 = 19979999U;


}


void TMR5_Start(void)
{
    T5CONSET = _T5CON_ON_MASK;
}


void TMR5_Stop (void)
{
    T5CONCLR = _T5CON_ON_MASK;
}

void TMR5_PeriodSet(uint32_t period)
{
    PR5  = period;
}

uint32_t TMR5_PeriodGet(void)
{
    return PR5;
}

uint32_t TMR5_CounterGet(void)
{
    return (TMR5);
}


uint32_t TMR5_FrequencyGet(void)
{
    return (60000000);
}


bool TMR5_PeriodHasExpired(void)
{
    bool status = (IFS0bits.T5IF != 0U);
    IFS0CLR = _IFS0_T5IF_MASK;

    return status;
}

