/*******************************************************************************
 Configurable Logic Cell (CLC3) PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_clc3.c

  Summary:
    CLC3 PLIB Implementation file

  Description:
    This file defines the interface to the CDAC peripheral library. This
    library provides access to and control of the associated peripheral
    instance.

*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
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
#include "plib_clc3.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope or Global Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: CLC3 Interface Routines
// *****************************************************************************
// *****************************************************************************
void CLC3_Initialize( void )
{
    /* Configure data sources */
    CLC3SEL =  (((0x0UL << _CLC3SEL_DS1_POSITION) & _CLC3SEL_DS1_MASK) |
                ((0x0UL << _CLC3SEL_DS2_POSITION) & _CLC3SEL_DS2_MASK) |
                ((0x0UL << _CLC3SEL_DS3_POSITION) & _CLC3SEL_DS3_MASK) |
                ((0x0UL << _CLC3SEL_DS4_POSITION) & _CLC3SEL_DS4_MASK));

    /* Configure gates */
    CLC3GLS = 0x0;

    /* Configure logic cell */
    CLC3CON = (((0x0UL << _CLC3CON_MODE_POSITION) & _CLC3CON_MODE_MASK)
               | _CLC3CON_G1POL_MASK 
               | _CLC3CON_G2POL_MASK 
               | _CLC3CON_ON_MASK );

}

void CLC3_Enable(bool enable)
{
  if(enable == true)
  {
    CLC3CON |= _CLC3CON_ON_MASK;
  }
  else
  {
    CLC3CON &= ~(_CLC3CON_ON_MASK);
  }
}

