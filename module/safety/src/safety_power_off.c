/*=============================================================================
Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
All trademarks are owned by Enarka India Private Limited
============================================================================ */

/*==============================================================================
 @file  safety_power_off.c
 @author JOTHI RAMESH
 @date 26-Jan-2022

 @brief Description
==============================================================================*/

/*==============================================================================
 Includes
==============================================================================*/

#include "F28x_Project.h"
#include <stdint.h>
#include "safety_lib.h"
#include "safety_lib_prv.h"
#include "rte_io.h"
/*==============================================================================
 Defines
==============================================================================*/

/*==============================================================================
 Enums
==============================================================================*/

/*==============================================================================
 Structures
==============================================================================*/

/*==============================================================================
 Macros
==============================================================================*/

/*==============================================================================
 Local Function Prototypes
==============================================================================*/

void safety_fnPower_off(microelectronic_fault_t fault);

/*==============================================================================
 Local Variables
==============================================================================*/

/*==============================================================================
 Local Constants
==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
void safety_fnPower_off(microelectronic_fault_t fault)
{
    volatile int32_t i = 0;
    __disable_interrupts();

//    disable_loss_of_osc();
    // clear DSP beat
    HW_fnDisableDSP_beat();

     // Make all outputs off
    GpioDataRegs.GPACLEAR.all = 0xFFFFFFFFUL;
    GpioDataRegs.GPACLEAR.all = 0xFFFFFFFFUL;

    if (fault >= MEF_LAST_FAULT)
        fault = MEF_UNKNOWN;


    /* send the TURN off DIO commands to IO card */


//    ECanaMboxes.MBOX7.MDL.word.LOW_WORD = 0;
//    ECanaMboxes.MBOX7.MDL.word.HI_WORD = 0;
//    ECanaMboxes.MBOX7.MDH.word.LOW_WORD = 0x00AAU;
//    ECanaMboxes.MBOX7.MDH.word.HI_WORD = (0xC0 | fault & 0xF) << 8;

//    volatile uint32_t mask = (1UL << 7);
//    for(;;)
//    {
//        if ((ECanaRegs.CANTA.all & mask) || (!(ECanaRegs.CANTRS.all & mask)))
//            ECanaRegs.CANTRS.all = mask;
//        for ( i = 0; i < 1000000U; i++)
//            ;
//    }
}


/*==============================================================================
 End of File
==============================================================================*/
