/*=============================================================================
Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
All trademarks are owned by Enarka India Private Limited
============================================================================ */

/*==============================================================================
 @file  safety_fault_detection_testing.c
 @author OHMLAP0112   
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
#include "isr.h"
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
static void hw_fnFaultProtection(void);
static void hw_fnCANtranscheck(void);
static void HW_fnReset(void);

/*==============================================================================
 Local Variables
==============================================================================*/
bool bt_Faultpin = false;
bool bt_FaultFBpin = false;
bool bt_DSP_beat = false;
uint16_t uifailCount = 0;
/*==============================================================================
 Local Constants
==============================================================================*/

/*=============================================================================
 @brief HW_fnFaultProtection - fault protection

 @param void
 @return void
 ============================================================================ */
static void hw_fnFaultProtection(void)
{
    if(HW_mFAULT_STATUS == HW_mFAULT)
    {
        HW_fnReset();
    }

}

/*=============================================================================
 @brief HW_fnFaultProtection - fault protection

 @param void
 @return void
 ============================================================================ */
static void hw_fnCANtranscheck(void)
{
    bt_Faultpin = HW_mFAULT_STATUS;
    bt_FaultFBpin = HW_mFAULT_FB_STATUS;

    if(bt_Faultpin != bt_FaultFBpin)
    {
        //can transceiver fail;
        uifailCount++;
        if(uifailCount > 1500)
        {
            safety_fnPower_off(MEF_TRANCEIVER);
            uifailCount = 1500;
        }

    }
    else
    {
        //canIO_tzbuff.zDO_buff.all  = canIO_tzbuff.zDO_buff.all;
        uifailCount = 0;
    }
}

/*=============================================================================
 @brief HW_fnFaultProtection - fault protection

 @param void
 @return void
 ============================================================================ */
void HW_fnSafeCheck(void)
{
    hw_fnFaultProtection();
    hw_fnCANtranscheck();
}

/*=============================================================================
 @brief HW_fnServiceWdog() = services watchdog module

 @param void
 @return void
 ============================================================================ */
void HW_fnServiceWdog(void)
{
    EALLOW;
    WdRegs.WDKEY.bit.WDKEY = 0x0055;
    WdRegs.WDKEY.bit.WDKEY = 0x00AA;
    EDIS;
}



/*=============================================================================
 @brief HW_fnReset = to generate reset pulse

 @param void
 @return void
 ============================================================================ */
static void HW_fnReset(void)
{
  uiResetTimer = 0;
}


/*=============================================================================
 @brief HW_fnInitDSP_beat = to start toggling dsp heart beat

 @param void
 @return void
 ============================================================================ */
void HW_fnEnableDSP_beat(void)
{
    bt_DSP_beat = true;
}

/*=============================================================================
 @brief HW_fnDisableDSP_beat = to stop toggling dsp heart beat

 @param void
 @return void
 ============================================================================ */
void HW_fnDisableDSP_beat(void)
{
    bt_DSP_beat = false;
}

/*=============================================================================
 @brief HW_fnDSP_beat = to  toggle dsp heart beat

 @param void
 @return void
 ============================================================================ */
void HW_fnDSP_beat(void)
{
    if(bt_DSP_beat)
    {
        GpioDataRegs.GPATOGGLE.bit.GPIO11 = 1;
    }
}
/*==============================================================================
 End of File
==============================================================================*/
