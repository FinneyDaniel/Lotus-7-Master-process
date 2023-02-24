/*=============================================================================
Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
All trademarks are owned by Enarka India Private Limited
============================================================================ */

/*==============================================================================
 @file  safety_logical_monitoring.c
 @author    JOTHI RAMESH
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
#include "rte_hardware.h"
#include "app_constants.h"
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
static safe_bool bEvenslot_sequence[NUM_mTIME_SLOTS];
void safety_fnLog_Monitoring_init(void);
void safety_fnLog_monitoring_slot_exe(uint16_t slot);
void safety_fnlog_monitoring_chk(void);
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

void safety_fnLog_Monitoring_init(void)
{
    uint16_t uiEventslot;
    // Start with everything good
    for (uiEventslot = 0; uiEventslot < NUM_mTIME_SLOTS; uiEventslot++)
    {
        bEvenslot_sequence[uiEventslot] = safe_true;
    }
}
/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */


//__attribute__((always_inline))
void safety_fnLog_monitoring_slot_exe(uint16_t uiEventslot)
{
    bEvenslot_sequence[uiEventslot] = safe_true;
}
/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */

void safety_fnlog_monitoring_chk(void)
{
    uint16_t uiEventslot;
    // Check program sequence
    safe_bool bEventslot_sequence_fail = safe_false;
    for (uiEventslot = 0; uiEventslot < NUM_mTIME_SLOTS; uiEventslot++)
    {
        if (bEvenslot_sequence[uiEventslot] != safe_true)
        {
           //  bEvenslot_sequence[uiEventslot]=safe_false;
            // A time slot did not execute
            bEventslot_sequence_fail = safe_true;
            break;
        }
            bEvenslot_sequence[uiEventslot] = safe_false;
    }

    // Take action
     if (bEventslot_sequence_fail == safe_false)
         hw_service_wdog();
     else
         safety_fnPower_off(MEF_LOGICAL_MONITORING);
}

/*==============================================================================
 End of File
==============================================================================*/
