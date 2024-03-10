/*******************************************************************************
 Configurable Logic Cell (CLC2) PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_clc2.c

  Summary:
    CLC2 PLIB Implementation file

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
#include "plib_clc2.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope or Global Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: CLC2 Interface Routines
// *****************************************************************************
// *****************************************************************************
void CLC2_Initialize( void )
{
    /* Configure data sources */
    CLC2SEL =  (((0x6UL << _CLC2SEL_DS1_POSITION) & _CLC2SEL_DS1_MASK) |
                ((0x0UL << _CLC2SEL_DS2_POSITION) & _CLC2SEL_DS2_MASK) |
                ((0x0UL << _CLC2SEL_DS3_POSITION) & _CLC2SEL_DS3_MASK) |
                ((0x1UL << _CLC2SEL_DS4_POSITION) & _CLC2SEL_DS4_MASK));

    /* Configure gates */
    CLC2GLS =  (_CLC2GLS_G1D3T_MASK |
                _CLC2GLS_G2D4T_MASK |
                _CLC2GLS_G3D1T_MASK |
                _CLC2GLS_G4D4T_MASK);

    /* Configure logic cell */
    CLC2CON = (((0x0UL << _CLC2CON_MODE_POSITION) & _CLC2CON_MODE_MASK)
               | _CLC2CON_G2POL_MASK 
               | _CLC2CON_ON_MASK 
               | _CLC2CON_LCOE_MASK );

}

void CLC2_Enable(bool enable)
{
  if(enable == true)
  {
    CLC2CON |= _CLC2CON_ON_MASK;
  }
  else
  {
    CLC2CON &= ~(_CLC2CON_ON_MASK);
  }
}

