/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  oi_psurefcurset.c
 @author DEEPTI.K
 @date 06-Sep-2021

 @brief Description
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include <state_machine.h>
#include "F28x_Project.h"              // Standard libraries headers
#include "F2837xS_Device.h"
#include "math.h"
#include "stdlib.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_can.h"
#include "hal/driverlib/can.h"
#include "cana_PSUCom.h"
#include "scheduler.h"
#include "cana_defs.h"
#include "canb_defs.h"
#include "cana_vsc.h"
#include "mathcalc.h"
#include "control_defs.h"
/*==============================================================================
 Defines
 ==============================================================================*/

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/

union CONTROL_FLTS_REG CONTROLtzFaultRegs;

/*==============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

void CONTROL_fnPSU_IRef(void);
void control_fncellmaxval(void);

/*==============================================================================
 Local Variables
 ==============================================================================*/

uint32_t ui32NodePresent = 0;
uint16_t ui16SafeShutDownFlg = 0, ui16RampdownAtstackPwr = 0,
        ui16StH2CmdFrmMSFlg = 0;
uint16_t ui16InstShutDownFlg = 0;
uint16_t testflag = 1;
uint32_t ui32WaterOkCnt = 0, ui32WaterNotOkCnt = 0;
uint32_t ui32CellVoltTripCnt = 0, ui32stackCellNotokcnt = 0,
        ui32stackCellokcnt = 0;

int16_t n1 = 16, i, j, z;
int16_t n2 = 12;
int16_t n3 = 32; //Array Size Declaration
float32_t c[32]; //Array Declaration
float32_t f32maxCellVolt;
uint16_t ui16Act_PsuNodes = 0;
/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
void CONTROL_fnPSU_IRef(void)
{

    control_fncellmaxval();

    if (f32maxCellVolt > CANB_tzSiteRxRegs.CellMaxLimit )
     {
        CANA_tzActNodeRegs_IO.bit.CellFault = 1;
     }
    // else if (f32maxCellVolt <= CANB_tzSiteRxRegs.CellMinLimit) //CANB_tzSiteRxRegs.CellMinLimit
    else
     {
         CANA_tzActNodeRegs_IO.bit.CellFault= 0;
     }

    // Receiving Count value in seconds => 1 count = 50msec, ....cnts = 1sec => 1/50ms = 20

     ui32CellVoltTripCnt = (ceil)(CANB_tzSiteRxRegs.CellNotokTripCnt * 20.0);

     if (CANA_tzActNodeRegs_IO.bit.CellFault== 1)
     {
         ui32stackCellokcnt = 0;
         ui32stackCellNotokcnt++;
         if (ui32stackCellNotokcnt >= ui32CellVoltTripCnt)
         {
             ui32stackCellNotokcnt = ui32CellVoltTripCnt;
             CONTROLtzFaultRegs.bit.StackcellFault = 1;
         }
     }
     else if (CANA_tzActNodeRegs_IO.bit.CellFault== 0)
     {
         ui32stackCellNotokcnt = 0;
         ui32stackCellokcnt++;
         if (ui32stackCellokcnt >= ui32CellVoltTripCnt)
         {
             ui32stackCellokcnt = ui32CellVoltTripCnt;
             CONTROLtzFaultRegs.bit.StackcellFault = 0;
         }
     }
/*******************************************************************************************************************/
    // Receiving Current References from SP and Move to Stack Check from Ready State
//     if(CANB_tzSiteRegs.f32CurrSet>100.0f) //for safety
//     {
//         CANB_tzSiteRegs.f32CurrSet = 100.0f;
//     }
    if (CANB_tzSiteRegs.f32CurrSet >= 10.0f)
    {
        CANB_tzSiteRxRegs.Start_H2Cmd = 1;
        CANB_tzSiteRxRegs.Start_cmd = 2;
    }
    else if (CANB_tzSiteRegs.f32CurrSet < 1.0f)
    {
        CANB_tzSiteRxRegs.Start_H2Cmd = 0;
    }


    if (MATHConvtzRegs.AISensorPRT101 < 0.0f)
    {
        MATHConvtzRegs.AISensorPRT101 = 0;
    }

    if (MATHConvtzRegs.AISensorPRT401 < 0.0f)
    {
        MATHConvtzRegs.AISensorPRT401 = 0;
    }

    if (MATHConvtzRegs.AISensorPRT1 < 0.0f)
    {
        MATHConvtzRegs.AISensorPRT1 = 0;
    }

    // Keeping a Count of No. of Active Power Supplies
    // Required to divide the Current references based on Active Nodes

    // ui32NodePresent = 2;  // for testing purpose directly 2 DCDC set  change ui16Act_PsuNodes variable to ui32NodePresent for actual final test
     ui32NodePresent =  (CANA_ActnodeRegs_DCDC0.bit.bt_node1
            +CANA_ActnodeRegs_DCDC0.bit.bt_node2
            + CANA_ActnodeRegs_DCDC0.bit.bt_node3
            + CANA_ActnodeRegs_DCDC0.bit.bt_node4
            + CANA_ActnodeRegs_DCDC0.bit.bt_node5
            + CANA_ActnodeRegs_DCDC0.bit.bt_node6
            + CANA_ActnodeRegs_DCDC0.bit.bt_node7
            + CANA_ActnodeRegs_DCDC0.bit.bt_node8
            + CANA_ActnodeRegs_DCDC0.bit.bt_node9
            + CANA_ActnodeRegs_DCDC1.bit.bt_node1
            + CANA_ActnodeRegs_DCDC1.bit.bt_node2
            + CANA_ActnodeRegs_DCDC1.bit.bt_node3
            + CANA_ActnodeRegs_DCDC1.bit.bt_node4
            + CANA_ActnodeRegs_DCDC1.bit.bt_node5
            + CANA_ActnodeRegs_DCDC1.bit.bt_node6
            + CANA_ActnodeRegs_DCDC1.bit.bt_node7
            + CANA_ActnodeRegs_DCDC1.bit.bt_node8
            + CANA_ActnodeRegs_DCDC1.bit.bt_node9);



    // If No of Active Nodes are 0, Make the Current reference also 0

    if ((ui32NodePresent == 0) && (CANA_tzMSRegs.StartCmd == 1))
    {
        CANA_tzTxdRegs.tzPSUData.CurrentSet = 0;
    }

    // Make the Current reference 0 in Other States except Stack Power
    else
    {
        if (((STAT_tzStateMac.Present_st == STAND_BY)
                || (STAT_tzStateMac.Present_st == READY)
                || (STAT_tzStateMac.Present_st == STACK_CHECK))
                && (ui16manualTesting == 0))
        {
            CANA_tzTxdRegs.tzPSUData.TotalCurrentSet = 0;
        }

        // In stack Power there are 3 Conditions
        // 1. Hydrogen Generation State
        // 2. Ramping Down of Current References and remain in
        //    Stack Power(If MS Sends Ready Command)
        //    Goes to StandBy State If MS Stops Ready Command
        //    Goes to Ready State if Current reference Set is Less than 10%(for now 1%)
        // 3. Shutdown and goto Fault State upon Critical Faults

        else if ((STAT_tzStateMac.Present_st == STACK_POWER)
                || (ui16manualTesting == 1))
        {

            // Hydrogen Generation State

            if ((CANB_tzSiteRxRegs.Start_H2Cmd == 1)
                    && (CANA_tzMSRegs.StartCmd == 1)
                    && (CANB_tzSiteRxRegs.Start_H2CmdStkpwr == 1)
                    && (CANA_tzLPCIO2_AIFaultRegs.bit.HYS_NotOk == 0)
                    && (CANA_tzLHCIO1_AIFaultRegs.bit.PRT_101 == 0)
                    && (CANA_tzLHCIO1_AIFaultRegs.bit.PRT_102 == 0)
                    && (CANA_tzLHCIO2_AIFaultRegs.bit.PRT_401 == 0)
                  //  && (CANA_tzLHCIO2_AIFaultRegs.bit.PRT_402 == 0)
                    && (CANA_tzLHCIO1_AIFaultRegs.bit.COS_101 == 0)
                    && (CANA_tzThermalFaultRegs.bit.TTC_101 == 0)
                    && (CANA_tzThermalFaultRegs.bit.TTC_102 == 0)
                    && (CANA_tzThermalFaultRegs.bit.TTC_301 == 0)
                    && (CANA_tzThermalFaultRegs.bit.KTC_401 == 0)
                    && (CANA_tzLHCIO2_AIFaultRegs.bit.TE_401 == 0)
                    && (CONTROLtzFaultRegs.bit.StackcellFault == 0)
                    && (CANA_tzLPCIO2_AIFaultRegs.bit.OXS_101_RmpDwn == 0)
                    && (CANA_tzLPCIO2_AIFaultRegs.bit.OXS_101_ShtDwn == 0)
                    && (CANA_tzLPCIO2_AIFaultRegs.bit.HYS_102_ShtDwn == 0))

            {
                CANA_tzTimerRegs.tzPSU.CurRampDowncnt = 0;
                CANA_tzTimerRegs.tzPSU.InstShutDowncnt = 0;

                ui16SafeShutDownFlg = 0;
                ui16RampdownAtstackPwr = 0;
                ui16InstShutDownFlg = 0;
                ui16Bleedh2 = 0;
                ui16StH2CmdFrmMSFlg = 0;

                CANA_tzTimerRegs.tzPSU.CurRampUpcnt++;

                CANA_tzTxdRegs.tzPSUData.TotalCurrentSet =
                        CANB_tzSiteRegs.f32CurrSet;

                CANA_tzTxdRegs.tzPSUData.TotalISetTemp =
                        (CANA_tzTxdRegs.tzPSUData.TotalCurrentSet
                                / ui32NodePresent);

                // Ramping up of Current Reference by 2.0A to each PSU, once in 4.5 Sec
                // Ramping up of Current Reference by 6.923A to each PSU, once in 1 Sec, Change delay to 20 and Current to 6.923A
                // i.e CANA_tzTimerRegs.tzPSU.CurRampUpcnt = 20, CANA_tzTxdRegs.tzPSUData.CurrentSet += 6.923

                if (CANA_tzTimerRegs.tzPSU.CurRampUpcnt >= 90)
                {
                    CANA_tzTxdRegs.tzPSUData.CurrentSet += 2;

                    CANA_tzTimerRegs.tzPSU.CurRampUpcnt = 0;
                }

                if (CANA_tzTxdRegs.tzPSUData.CurrentSet
                        >= CANA_tzTxdRegs.tzPSUData.TotalISetTemp)
                {
                    CANA_tzTxdRegs.tzPSUData.CurrentSet =
                            CANA_tzTxdRegs.tzPSUData.TotalISetTemp;
                }

                CONTROLtzFaultRegs.bit.LPCCurHealthy = 0; // Bit used by MS to detect H2 Generation State

            }

            else
            {
                // Ramping Down of Current References

                if ((CANB_tzSiteRxRegs.Start_H2CmdStkpwr == 0)
                        || (CANB_tzSiteRxRegs.Start_H2Cmd == 0)
                        || (CANA_tzMSRegs.StartCmd == 0)
                        || (CANA_tzLPCIO2_AIFaultRegs.bit.HYS_NotOk == 1)
                        || (CANA_tzLHCIO1_AIFaultRegs.bit.PRT_101 == 1)
                        || (CANA_tzLHCIO1_AIFaultRegs.bit.PRT_102 == 1)
                        || (CANA_tzLHCIO2_AIFaultRegs.bit.PRT_401 == 1)
                      //  || (CANA_tzLHCIO2_AIFaultRegs.bit.PRT_402 == 1)
                        || (CANA_tzLHCIO1_AIFaultRegs.bit.COS_101 == 1)
                        || (CANA_tzThermalFaultRegs.bit.TTC_101 == 1)
                        || (CANA_tzThermalFaultRegs.bit.TTC_102 == 1)
                        || (CANA_tzThermalFaultRegs.bit.TTC_301 == 1)
                        || (CANA_tzThermalFaultRegs.bit.KTC_401 == 1)
                        || (CANA_tzLHCIO2_AIFaultRegs.bit.TE_401 == 1)
                        || (CONTROLtzFaultRegs.bit.StackcellFault == 1)
                        || (CANA_tzLPCIO2_AIFaultRegs.bit.OXS_101_RmpDwn == 1))
                {

                    CANA_tzTimerRegs.tzPSU.CurRampUpcnt = 0;
                    ui16InstShutDownFlg = 0;

                    CANA_tzTxdRegs.tzPSUData.TotalCurrentSet =
                            CANB_tzSiteRegs.f32CurrSet;

                    CANA_tzTxdRegs.tzPSUData.TotalISetTemp =
                            (CANA_tzTxdRegs.tzPSUData.TotalCurrentSet
                                    / ui32NodePresent);

                    CANA_tzTimerRegs.tzPSU.CurRampDowncnt++;

                    CONTROLtzFaultRegs.bit.LPCCurHealthy = 1;

                    if (CANA_tzTimerRegs.tzPSU.CurRampDowncnt >= 90)
                    {
                        CANA_tzTxdRegs.tzPSUData.CurrentSet -= 2;

                        CANA_tzTimerRegs.tzPSU.CurRampDowncnt = 0;
                    }

                    // Stop Command from Master Safety - Goto SafeShutdown --> StandBy State
                    if (CANA_tzMSRegs.StartCmd == 0)
                    {
                        ui16SafeShutDownFlg = 1;
                        ui16RampdownAtstackPwr = 0;
                    }

                    if (CANA_tzTxdRegs.tzPSUData.CurrentSet <= 0.5)
                    {
                        CANA_tzTxdRegs.tzPSUData.CurrentSet = 0.0;
                        CANA_tzTxdRegs.tzPSUData.TotalISetTemp = 0;

                        // In any of the above Conditions bleed H2 should happen - Dryer Valves

                        //ui16Bleedh2 = 1;

                        // H2 Percent < 10% from System Process - Goto Ready State --> Restart the process

                        if (CANB_tzSiteRxRegs.Start_H2Cmd == 0)
                        {
                            ui16StH2CmdFrmMSFlg = 1;
                            ui16SafeShutDownFlg = 0;
                            ui16RampdownAtstackPwr = 0;

                        }

                        //Any Ramp down Faults occur --> Ramp down and stay in Stack Power

                        else
                        {
                            ui16StH2CmdFrmMSFlg = 0;
                            //ui16SafeShutDownFlg = 0;
                            ui16RampdownAtstackPwr = 1;
                        }

                    }

                }

                // Shutdown and goto Fault State

                if ((CANA_tzLPCIO2_AIFaultRegs.bit.HYS_102_ShtDwn == 1)
                        || (CANA_tzLPCIO2_AIFaultRegs.bit.OXS_101_ShtDwn == 1))
                {
                    CANA_tzTimerRegs.tzPSU.CurRampDowncnt = 0;
                    CANA_tzTimerRegs.tzPSU.CurRampUpcnt = 0;
                    CONTROLtzFaultRegs.bit.LPCCurHealthy = 1;
                    ui16SafeShutDownFlg = 0;
                    ui16RampdownAtstackPwr = 0;
                    CANA_tzTimerRegs.tzPSU.InstShutDowncnt++;
                    if (CANA_tzTimerRegs.tzPSU.InstShutDowncnt >= 20) // delay of 1.2s( 60m * 20) for Instantaneous trip
                    {
                        CANA_tzTimerRegs.tzPSU.InstShutDowncnt = 20;
                        ui16InstShutDownFlg = 1;
                        //ui16Bleedh2 = 1;

                    }
                }

            }
        }
    }
}

/*==============================================================================
 End of File
 ==============================================================================*/
void control_fncellmaxval()
{
#if 0
    uint16_t uiMaxCelltemp = 0;

    while(uiMaxCelltemp < 16)
    {
        canA_tzVSC_info[uiMaxCelltemp].f32MaxCellVolt[0]=canA_tzVSC_info[uiMaxCelltemp].f32Cellvolt[1];

        for (j = 2; j <= 8; j++)
        {
            if (canA_tzVSC_info[uiMaxCelltemp].f32Cellvolt[1] > canA_tzVSC_info[uiMaxCelltemp].f32MaxCellVolt[0])
            {
                canA_tzVSC_info[uiMaxCelltemp].f32MaxCellVolt[0] = canA_tzVSC_info[uiMaxCelltemp].f32Cellvolt[1];
                canA_tzVSC_info[uiMaxCelltemp].f32MaxCellVolt[1]  = canA_tzVSC_info[uiMaxCelltemp].f32Cellvolt[1];
                canA_tzVSC_info[uiMaxCelltemp].uiMaxcellNum[0] = j;
                canA_tzVSC_info[uiMaxCelltemp].uiMaxcellNum[1] = j;
            }
        }

        uiMaxCelltemp = uiMaxCelltemp + 4;
    }
#endif

    for (i = 0; i < n1; i++)
    {
        c[i] = canA_tzVSC_info[i].f32MaxCellVolt[0];
    }
    for (j = n1; j < n3; j++)
    {
        c[j] = canA_tzVSC_info[j - n1].f32MaxCellVolt[1];
    }

    f32maxCellVolt = c[0];

    for (z = 0; z < n3; z++)
    {
        if (c[z] > f32maxCellVolt)
        {
            f32maxCellVolt = c[z];
        }
    }

 }

