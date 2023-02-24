/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  state_machine.c
 @author DEEPTI.K
 @date 06-Sep-2021

 @brief Description
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include "F28x_Project.h"              // Standard libraries headers
#include "F2837xS_Device.h"
#include "math.h"
#include "stdlib.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_can.h"
#include "hal/driverlib/can.h"
#include "cana_defs.h"
#include "canb_defs.h"
#include "cana_PSUCom.h"
#include "state_machine.h"
#include "scheduler.h"
#include "control_defs.h"
#include "mathcalc.h"

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

void STAT_fnFSMCheck(void);

static void stat_fnFSMNextState(void);
static void stat_IOReset(void);
/*==============================================================================
 Local Variables
 ==============================================================================*/

uint16_t ui32StateTimer = 0;
STAT_tzSTATEMAC STAT_tzStateMac;
STAT_tzSTATE_MSMAC STAT_tzStateMacMS;
extern uint16_t ui16IsTxCellCount;
/*==============================================================================
 Local Constants
 ==============================================================================*/

void stat_fnInitState(void)
{
    // Global Flags
    uint16_t i = 0;

    // State Variables Initialization
    STAT_tzStateMac.Next_st = STAND_BY;
    STAT_tzStateMac.Previous_st = STAND_BY;
    STAT_tzStateMac.Present_st = STAND_BY;
    CANA_tzMSRegs.StartCmd = 0;
    CANA_tzActNodeRegs_IO.all = 0;
    CANA_tzLPCIO1_DIFaultRegs.all = 0;
    CANA_tzLPCIO2_DIFaultRegs.all = 0;
    CANA_tzLHCIO1_DIFaultRegs.all = 0;
    CANA_tzLHCIO2_DIFaultRegs.all = 0;
    CANA_tzLPCIO1_AIFaultRegs.all = 0;
    CANA_tzLPCIO2_AIFaultRegs.all = 0;
    CANA_tzLHCIO1_AIFaultRegs.all = 0;
    CANA_tzLHCIO2_AIFaultRegs.all = 0;
    CANA_tzThermalFaultRegs.all = 0;
    CANA_tzActNodeRegs_PSU.all = 0;
    CANA_tzActNode1Regs_PSU.all = 0;
    for (i = 0; i < 2; i++)
    {
        CANA_tzLPCAIFlt_IORegs[i].all = 0;
        CANA_tzLHCAIFlt_IORegs[i].all = 0;
        CANA_tzLPCDI_IORegs[i].all = 0;
        CANA_tzLHCDI_IORegs[i].all = 0;
    }
    for (i = 0; i < 29; i++)
    {
        CANA_tzDCDCFaultRegs[i].all = 0;
        CANA_tzFECHFaultRegs[i].all = 0;
        CANA_tzFECLFaultRegs[i].all = 0;
    }
    CANA_tzIOflags.btLHC10CommnStart = 0;
    CANA_tzIOflags.btLHC11CommnStart = 0;
    CANA_tzIOflags.btLHC12CommnStart = 0;
    CANA_tzIOflags.btLHC13CommnStart = 0;
    CANA_tzIOflags.btLPC30CommnStart = 0;
    CANA_tzIOflags.btLPC31CommnStart = 0;
    CANA_tzIOflags.faultAtStChkSt = 0;
    CANA_tzIORegs.Node = 0;
    CANA_tzIORegs.CJC[0] = 0;
    CANA_tzIORegs.CJC[1] = 0;
    CANA_tzIORegs.uiMsgtypeLHCIO = 0;
    CANA_tzIORegs.uiNodeLHCIO = 0;
    CANA_tzIORegs.uiMsgtypeLPCIO = 0;
    CANA_tzIORegs.uiNodeLPCIO = 0;
    CANA_tzIORegs.uiUnitID = 0;
    CANA_tzActMS_FaultRegs.all = 0;
    CANA_tzActNodeRegs_VS1.all = 0;
    CANA_tzActNodeRegs_VS.all = 0;

    ui16Bleedh2 = 0;
}
/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */

void STAT_fnFSMCheck(void)
{
    {

        //  Determine the Next State depending on the flags set/reset in the rest of the code

        stat_fnFSMNextState();

        switch (STAT_tzStateMac.Present_st)
        {

        case STAND_BY:
        {

            break;
        }

        case READY:
        {

            break;
        }

        case STACK_CHECK:
        {

            break;
        }

        case STACK_POWER:
        {

            break;
        }

        case FAULT:
        {

            break;
        }

        case COMMISSION:
        {

            break;
        }

        default:
            break;
        }
    }

}

//#################################################################################################################
static void stat_fnFSMNextState()
{
    if(CANB_tzSiteRxRegs.Start_cmd == 5)
            {
                STAT_tzStateMac.Next_st = COMMISSION;
            }

    switch (STAT_tzStateMac.Next_st)
    {
    case STAND_BY:
    {
        ui16IsTxCellCount = 0;
        if (CANA_tzMSRegs.StartCmd == 0)
        {
            STAT_tzStateMac.Next_st = STAND_BY;

            CANB_tzSiteRxRegs.StateChngStandBy = 1;
            stat_IOReset();
        }

        else if ((CANA_tzMSRegs.StartCmd == 1)
                && (CANA_tzActNodeRegs_IO.bit.bt_LPC30 == 1)
                && (CANA_tzActNodeRegs_IO.bit.bt_LPC31 == 1)
                && (CANA_tzActNodeRegs_IO.bit.bt_LHC10 == 1)
                && (CANA_tzActNodeRegs_IO.bit.bt_LHC11 == 1)
                && (CANB_tzSiteRegs.StartCmd == 1)) // Manually change it for now
        {

            STAT_tzStateMac.Next_st = READY;
        }

        if (CANA_tzIOtimers.LHC10ComFailCnt == 3001)
        {
            MATHConvtzRegs.AISensorLVL101 = 0;
            MATHConvtzRegs.AISensorPRT101 = 0;
            MATHConvtzRegs.AISensorPRT102 = 0;
            MATHConvtzRegs.AISensorPRT401 = 0;
            MATHConvtzRegs.AISensorPRT1 = 0;
            MATHConvtzRegs.AISensorEBV801 = 0;
            MATHConvtzRegs.AISensorTE401 = 0;
            MATHConvtzRegs.TempSensorKTC401 = 0;
            MATHConvtzRegs.TempSensorTTC101 = 0;
            MATHConvtzRegs.TempSensorTTC102 = 0;
            MATHConvtzRegs.TempSensorTTC301 = 0;
            MATHConvtzRegs.AISensorCOS101 = 0;
        }

        break;
    }
    case READY:
    {
        ui16IsTxCellCount = 0;
        if (CANA_tzMSRegs.StartCmd == 0)
        {
            STAT_tzStateMac.Next_st = STAND_BY;

            CANB_tzSiteRxRegs.StateChngStandBy = 1;
            stat_IOReset();
        }

        else if ((CANA_tzMSRegs.StartCmd == 1) // 1 FOR TESTING
                && (CANB_tzSiteRxRegs.Start_cmd == 2))
        {
            STAT_tzStateMac.Next_st = STACK_CHECK;
        }

        else if (CANB_tzSiteRxRegs.Start_cmd == 5)
        {
            STAT_tzStateMac.Next_st = COMMISSION;
        }

        if (CANB_tzSiteRegs.f32CurrSet >= 10.0f)
        {
            CANB_tzSiteRxRegs.Start_cmd = 2;
        }

        break;
    }
    case STACK_CHECK:
    {
        ui16IsTxCellCount = 0;
        if (CANA_tzMSRegs.StartCmd == 0)
        {
            STAT_tzStateMac.Next_st = STAND_BY;

            CANB_tzSiteRxRegs.StateChngStandBy = 1;
            stat_IOReset();
        }

        else if ((CANA_tzMSRegs.StartCmd == 1)
                && (CANB_tzSiteRxRegs.Start_cmd == 3))
        {
            STAT_tzStateMac.Next_st = STACK_POWER;
        }

        break;
    }
    case STACK_POWER:
    {
        ui16IsTxCellCount = 1;
        if (ui16StH2CmdFrmMSFlg == 1)
        {
            stat_IOReset();
            STAT_tzStateMac.Next_st = READY;
            CANB_tzSiteRxRegs.Start_cmd = 1;
            CANA_tzQueryType.PSU = QUERY_PROGPARAM;

        }
        else if (ui16InstShutDownFlg == 1)
        {
            CANB_tzSiteRxRegs.StateChngFault = 0;
            //CANB_tzSiteRxRegs.Start_cmd = 1;
            CANB_tzSiteRegs.f32CurrSet = 0;
            CANA_tzTxdRegs.tzPSUData.CurrentSet = 0;

            //stat_IOReset();
            STAT_tzStateMac.Next_st = FAULT;
        }

        else if (ui16SafeShutDownFlg == 1)
        {
//            ui16StateTnstCnt = 0;
//            ui16StateRstCnt = 0;
//            CANB_tzSiteRxRegs.f32CurrSet = 0;
//            CANA_tzTxdRegs.tzPSUData.CurrentSet = 0;

            CANB_tzSiteRxRegs.StateChngStandBy = 1;
            CANB_tzSiteRxRegs.Start_cmd = 0;
            STAT_tzStateMac.Next_st = STAND_BY;
        }

        break;
    }
    case FAULT:
    {

        if ((CANA_tzLPCIO2_AIFaultRegs.bit.HYS_102_ShtDwn == 0)
                && (CANA_tzLPCIO2_AIFaultRegs.bit.OXS_101_ShtDwn == 0)
                && (CONTROLtzFaultRegs.bit.VFDPumpFail == 0))
        {
            ui16InstShutDownFlg = 0;
        }

        if (CANB_tzSiteRxRegs.Start_cmd == 0)
        {
            STAT_tzStateMac.Next_st = STAND_BY;

            CANB_tzSiteRxRegs.StateChngStandBy = 1;
            stat_IOReset();
        }

        else if (ui16InstShutDownFlg == 0)
        {
            ui16StateTnstCnt = 0;
            ui16StateRstCnt = 0;
            CANB_tzSiteRxRegs.StateChngFault = 0;
            CANB_tzSiteRxRegs.Start_cmd = 1;
            stat_IOReset();
            STAT_tzStateMac.Next_st = READY;

        }

        if (CANA_tzMSRegs.StartCmd == 0)
        {
            STAT_tzStateMac.Next_st = STAND_BY;

            CANB_tzSiteRxRegs.StateChngStandBy = 1;
            stat_IOReset();
        }
        break;
    }

    case COMMISSION:
    {
        if (CANB_tzSiteRxRegs.Start_cmd == 0)
        {
            STAT_tzStateMac.Next_st = STAND_BY;

            CANB_tzSiteRxRegs.StateChngStandBy = 1;
            stat_IOReset();
        }

        break;
    }

    default:
        break;
    }

// If the State.Next remains the same as State.Current no action needs to be taken
// However if two above mentioned states are different, the System is disabled
// and the state change flags, which track the time after a state change are reset
// to their initial states.
    if (STAT_tzStateMac.Next_st == STAT_tzStateMac.Present_st)
    {
        // asm ("NOP");
    }
    else
    {

    }
    STAT_tzStateMac.Previous_st = STAT_tzStateMac.Present_st;
    STAT_tzStateMac.Present_st = STAT_tzStateMac.Next_st;
}

/*==============================================================================
 End of File
 ==============================================================================*/
void stat_IOReset(void)
{

    ui16StateTnstCnt = 0;
    ui16SafeShutDownFlg = 0;
    ui16StH2CmdFrmMSFlg = 0;
    CANB_tzSiteRegs.f32CurrSet = 0;
    ui16InstShutDownFlg = 0;
    ui16CycleCount = 0;
    CANA_tzTxdRegs.tzPSUData.CurrentSet = 0;
    CANB_tzSiteRegs.f32VoltSet = 0;
    //val1_2Cnt = 0;
    CANB_tzSiteRxRegs.Start_cmd = 0;
    CANB_tzSiteRegs.StartCmd = 0;
    CANA_tzTimerRegs.tzPSU.CurRampDowncnt = 0;
    CANA_tzTimerRegs.tzPSU.CurRampUpcnt = 0;
    CANB_tzSiteRegs.H2Percent = 0;


    if(CANA_tzIOtimers.LHC10ComFailCnt >= 3000)
    {
        MATHConvtzRegs.AISensorLVL101 = 0;
        MATHConvtzRegs.AISensorCOS101 = 0;
        MATHConvtzRegs.AISensorPRT101 = 0;
        MATHConvtzRegs.AISensorPRT102 = 0;

    }


    if(CANA_tzIOtimers.LHC11ComFailCnt >= 3000)
    {
        MATHConvtzRegs.AISensorPRT401 = 0;
        MATHConvtzRegs.AISensorPRT1 = 0;
        MATHConvtzRegs.AISensorTE401 = 0;
        MATHConvtzRegs.AISensorPRT1 = 0;

    }
}
