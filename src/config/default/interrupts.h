/*******************************************************************************
 System Interrupts File

  Company:
    Microchip Technology Inc.

  File Name:
    interrupt.h

  Summary:
    Interrupt vectors mapping

  Description:
    This file contains declarations of device vectors used by Harmony 3
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>



// *****************************************************************************
// *****************************************************************************
// Section: Handler Routines
// *****************************************************************************
// *****************************************************************************

void INPUT_CAPTURE_1_InterruptHandler( void );
void INPUT_CAPTURE_2_InterruptHandler( void );
void EXTERNAL_2_InterruptHandler( void );
void INPUT_CAPTURE_3_InterruptHandler( void );
void EXTERNAL_3_InterruptHandler( void );
void INPUT_CAPTURE_4_InterruptHandler( void );
void EXTERNAL_4_InterruptHandler( void );
void INPUT_CAPTURE_5_InterruptHandler( void );
void RTCC_InterruptHandler( void );
void COMPARATOR_1_InterruptHandler( void );
void COMPARATOR_2_InterruptHandler( void );
void I2C1_BUS_InterruptHandler( void );
void I2C1_MASTER_InterruptHandler( void );
void I2C2_BUS_InterruptHandler( void );
void I2C2_SLAVE_InterruptHandler( void );
void COMPARATOR_3_InterruptHandler( void );
void COMPARATOR_4_InterruptHandler( void );
void COMPARATOR_5_InterruptHandler( void );



#endif // INTERRUPTS_H
