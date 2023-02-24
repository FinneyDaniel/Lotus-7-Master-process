/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  isr.c
 @author JOTHI RAMESH
 @date 26-Jan-2022

 @brief Description
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include "F28x_Project.h"              // Standard libraries headers
#include "F2837xS_Device.h"
#include "math.h"
#include "stdlib.h"
#include "safety_lib.h"
#include "safety_lib_prv.h"
#include "isr.h"
#include "scheduler.h"
#include "cana_defs.h"
#include "canb_defs.h"

#include "state_machine.h"
/*==============================================================================
 Defines
 ==============================================================================*/
#define CPU_mtimerT0_T1mincnt   (95U)
#define CPU_mtimerT0_T1maxcnt   (105U)
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

/*==============================================================================
 Local Variables
 ==============================================================================*/

bool bl_clock_OKflg = 0;
uint16_t uiResetTimer = 0;
uint16_t LEDCount = 0;
uint16_t ui16fsmCounter = 0;
uint16_t ui16CANAVSFailTrig1 = 0;
uint32_t ui32CANAVSFailCnt1 = 0;
/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 Description : cpu_timer0_isr - CPU Timer0 ISR with interrupt counter
 ============================================================================ */
interrupt void cpu_timer0_isr(void)  //100usec
{

    if (CanaRegs.CAN_CTL.bit.Init == 1)
    {
        EALLOW;
        CanaRegs.CAN_CTL.bit.ABO = 1;
        DELAY_US(0.1);
        EDIS;
    }

    if (CanbRegs.CAN_CTL.bit.Init == 1)
    {
        EALLOW;
        CanbRegs.CAN_CTL.bit.ABO = 1;
        DELAY_US(0.1);
        EDIS;
    }

    CpuTimer0Regs.TCR.bit.TIF = 1;
    CpuTimer0.InterruptCount++;
    if (CpuTimer1.InterruptCount == 1)
    {
        CpuTimer1.InterruptCount = 0;
        bl_clock_OKflg = 0;
    }

    ui16fsmCounter++;

    if (ui16fsmCounter > 20)  // FSM occurs once evry 2ms (100us * 20 = 2ms)
    {
        STAT_fnFSMCheck();
        ui16fsmCounter = 0;
    }

    CANA_fnRXevent();

    CANB_fnRXevent();

    /******** Latch Reset Pulse Starts here    ****/

    uiResetTimer++;

    if ((uiResetTimer >= 2) && (uiResetTimer <= 4))
    {
        //high for 2ms
        GpioDataRegs.GPBSET.bit.GPIO57 = 1;
    }

    if (uiResetTimer > 5)
    {
        uiResetTimer = 5;

        GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1;
    }
    //
    // Acknowledge this interrupt to receive more interrupts from group 1
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 Description : cpu_timer1_isr - CPU Timer1 ISR
 ============================================================================ */
interrupt void cpu_timer1_isr(void)  // 10msec
{

     GpioDataRegs.GPCSET.bit.GPIO85 = 1;
     GpioDataRegs.GPCSET.bit.GPIO86 = 1;


    CpuTimer1.InterruptCount++;
    if (CpuTimer1.InterruptCount == 1)
    {
        if ((CpuTimer0.InterruptCount > CPU_mtimerT0_T1mincnt)
                && (CpuTimer0.InterruptCount < CPU_mtimerT0_T1maxcnt))
        {
            bl_clock_OKflg = 1;
            CpuTimer0.InterruptCount = 0;
        }
        else
        {
            safety_fnPower_off(MEF_TIMING);
        }
    }
//     GpioDataRegs.GPCTOGGLE.bit.GPIO92 = 1;
    scheduler_task();

    CANA_fnTask();
    CANB_fnTask();

    ui32CANAVSFailCnt1++;

    if (ui32CANAVSFailCnt1 > 3000)
    {
        ui16CANAVSFailTrig1 = 1;
        ui32CANAVSFailCnt1 = 3000;
    }
    else
    {
        ui16CANAVSFailTrig1 = 0;
    }

    CANA_fnComFailChk();


    CANA_fnTx();

    CANB_fnTX_SiteEvent();

    cana_fnVSCTx();

    LEDCount++;

    if (LEDCount > 50)
    {
        LEDCount = 0;
    }

    if (LEDCount <= 25)
    {
        GpioDataRegs.GPCSET.bit.GPIO92 = 1;
    }
    if ((LEDCount >= 26) && (LEDCount <= 50))
    {
        GpioDataRegs.GPCCLEAR.bit.GPIO92 = 1;
    }
    CpuTimer1Regs.TCR.bit.TIF = 1;
    EINT;

    GpioDataRegs.GPCCLEAR.bit.GPIO85 = 1;
    GpioDataRegs.GPCCLEAR.bit.GPIO86 = 1;

}

/*==============================================================================
 End of File
 ==============================================================================*/
