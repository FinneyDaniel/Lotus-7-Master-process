/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  oi_psucan.c
 @author DEEPTI.K
 @date 03-Sep-2021

 @brief Description of CAN communication with PSU's and Master Controller
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/
#include <state_machine.h>
#include "cana_defs.h"
#include "F28x_Project.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "hal/driverlib/can.h"
#include "hal/driverlib/debug.h"
#include "hal/driverlib/interrupt.h"
#include "math.h"
#include "isr.h"
#include "canb_defs.h"
#include "cana_PSUCom.h"

/*==============================================================================
 Defines
 ==============================================================================*/

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/

CANA_tzTXDRegs CANA_tzTxdRegs;
CANA_tzRXDRegs CANA_tzRxdRegs;
CANA_tzPSURegs CANA_PSURegs;

CANA_tzTIMERRegs CANA_tzTimerRegs;
CANA_tzQUERYTYPE CANA_tzQueryType;
union CANA_tzACTNODE_REGS CANA_tzActNodeRegs_PSU;
union CANA_tzACTNODE1_REGS CANA_tzActNode1Regs_PSU;

union CANA_tzFAULT_DCDCREGS CANA_tzDCDCFaultRegs[29];
union CANA_tzFAULT_FECHREGS CANA_tzFECHFaultRegs[29];
union CANA_tzFAULT_FECLREGS CANA_tzFECLFaultRegs[29];
/*==============================================================================
 *
 Macros
 ==============================================================================*/

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

void CANA_fnPSUTX_Event(void);

void cana_fnTurnON_PSU(int32 i32nodeID, int32 i32ON_command, int32 i32global);

void cana_fnSendImpCmds(int32 i32nodeID);

void cana_fnSetVoltage_PSU(int32 i32nodeID, float32 f32voltageValue,
                           Uint16 ui16global);

void cana_fnSetCurrent_PSU(int32 i32nodeID, float32 f32CurrentValue,
                           Uint16 ui16global);
void cana_fnReadMBox_PSU(void);
void cana_fnNodeCheckLogic_PSU(void);
void cana_fnTimeoutFaultsLogic_PSU(void);

/*==============================================================================
 Local Variables
 ==============================================================================*/

uint16_t ui16txMsgDataPSU[8] = { 0 };
uint16_t ui16RxMsgDataPSU[8] = { 0 };
uint16_t ui16Rx1MsgDataPSU[8] = { 0 };
uint16_t ui16Rx2MsgDataPSU[8] = { 0 };
uint16_t ui16Rx3MsgDataPSU[8] = { 0 };

uint16_t ui16CANAPSUFailTrig1 = 0;
uint32_t ui32CANAPSUFailCnt1 = 0;

uint16_t ui16CANAIOFailTrig1 = 0;
uint32_t ui32CANAIOFailCnt1 = 0;
uint32_t ui32tempImp[3] = { 0 };
uint16_t ui16ModeChange = 0;
uint16_t ui16manualTesting = 0, PSUCommand = 0,reset_acdc =0,on_acdc=0,reset_dcdc0 =0,on_dcdc0=0,reset_dcdc1 =0,on_dcdc1=0;
uint16_t uiautoreset = 0, uiauto_oncmd =0,uidcdc_ONcmd=0,ui1secDly_ON_cmd=0,ui1secDly_OFF_CMD=0,uidcdc_offcmd=0;
float32  f32Curefset=0;
/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
void CANA_fnPSUTX_Event(void)
{
    uint16_t i,j;

    switch (STAT_tzStateMac.Present_st)
    {
    case STAND_BY:

        if (ui16manualTesting == 0)
        {
            uiautoreset =0;

            for (i = 0; i <= CANA_mTOT_ACDCPSUNODE; i++)
            {
                CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.uiON_cmd[i]= 0x55;
                CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.uiON_cmd[i]=0x55;
                CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.uiON_cmd[i]=0x55;
            }
        }

        break;

    case READY:
        uiautoreset = 0;
             for (j = 0; j <= CANA_mTOT_ACDCPSUNODE; j++)
        {
            CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.ui32Vdc_RefSet[j] = 0; // default Reference set as 0 and OFF
            CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.ui32Vdc_RefSet[j] = 0;
            CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.ui32Vdc_RefSet[j] = 0;
            CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.f32Idc_RefSet[j] =0;
            CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.f32Idc_RefSet[j] =0;
            CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.uiON_cmd[j]= 0x55;
            CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.uiON_cmd[j]=0x55;
            CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.uiON_cmd[j]=0x55;

        }

        break;

    case STACK_CHECK:

/*************************************************************************************/
        uiautoreset++;
         if (uiautoreset <= 7) // PSU NODES-7 FOR MK-1.5
         {
             CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.uiReset_cmd[uiautoreset] = 0xBB;
             CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.uiON_cmd[uiautoreset] =0xAA;
             CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.ui32Vdc_RefSet[uiautoreset] = 800; ////////////////default
             CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.ui32Vdc_RefSet[uiautoreset] = 450;   // DCDC not reset just set for ref.
             CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.ui32Vdc_RefSet[uiautoreset] = 450;
             CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.f32Idc_RefSet[uiautoreset] =0;
             CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.f32Idc_RefSet[uiautoreset] =0;
          }
         if (uiautoreset > 10)
        {
             uiautoreset = 10;
        }
/*************************************************************************************/
        break;

    case STACK_POWER:

            ui1secDly_ON_cmd++;
            if(ui1secDly_ON_cmd>20)   // 1sec delay provided for turn ON the DCDC
            {
                uidcdc_ONcmd++;
                ui1secDly_ON_cmd = 0;
            }
            if(uidcdc_ONcmd > 7)  // for testing only 2 DCDC on 2 * 2 DCDC   CANA_mTOT_DCDCPSUNODE
            {
                uidcdc_ONcmd=0;
            }
            if(CANA_tzPSURegs_RxACDC[uidcdc_ONcmd].tzPSUDataACDC.f32DCparam_VDC >700.0)
            {
                CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.uiReset_cmd[uidcdc_ONcmd] = 0x55;

                CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.uiReset_cmd[uidcdc_ONcmd] =0xBB;
                CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.uiReset_cmd[uidcdc_ONcmd] =0xBB;
                CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.uiON_cmd[uidcdc_ONcmd] =0xAA;
                CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.uiON_cmd[uidcdc_ONcmd] =0xAA;
                  if(CANA_tzTxdRegs.tzPSUData.CurrentSet>65.0) // for not exceeding more current.. than 6amps for each stamp
                  {
                      CANA_tzTxdRegs.tzPSUData.CurrentSet=65.0;
                  }

                CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.f32Idc_RefSet[uidcdc_ONcmd] =CANA_tzTxdRegs.tzPSUData.CurrentSet;//f32Curefset;
                CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.f32Idc_RefSet[uidcdc_ONcmd] =CANA_tzTxdRegs.tzPSUData.CurrentSet;//f32Curefset;
            }
            if(CANA_tzPSURegs_RxDCDC0[uidcdc_ONcmd].tzPSUDataDCDC.f32VACT>300)
            {
                CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.uiReset_cmd[uidcdc_ONcmd] =0x55;
                CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.uiReset_cmd[uidcdc_ONcmd] =0x55;

            }

        /*****************************************************/


        break;

    case FAULT:

    ui1secDly_OFF_CMD++;
    if (ui1secDly_OFF_CMD > 4) // 4 * 50 = 200msec delay provided for turn OFF the DCDC
    {
        uidcdc_offcmd++;
        ui1secDly_OFF_CMD = 0;
    }

    if (uidcdc_offcmd > CANA_mTOT_ACDCPSUNODE)
    {
        uidcdc_offcmd = 0;
    }
    CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.uiON_cmd[uidcdc_offcmd] = 0x55;
    CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.uiON_cmd[uidcdc_offcmd] = 0x55;
    CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.uiON_cmd[uidcdc_offcmd] = 0x55;
    break;

    case COMMISSION:
        if (ui16manualTesting == 1)
          {
            uiautoreset++;
            if (uiautoreset <= 9)
            {
                CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.uiReset_cmd[uiautoreset] = 0xBB;
                CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.uiON_cmd[uiautoreset] =0xAA;
                CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.ui32Vdc_RefSet[uiautoreset] = 800; ////////////////default
                CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.ui32Vdc_RefSet[uiautoreset] = 450;
                CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.ui32Vdc_RefSet[uiautoreset] = 450;

             }
             if (uiautoreset > 10)
            {
                 uiautoreset = 10;

                ui1secDly_ON_cmd++;
                if(ui1secDly_ON_cmd>20)  // 1sec delay provided for turn ON the DCDC
                {
                    uidcdc_ONcmd++;
                    ui1secDly_ON_cmd = 0;
                }
                if(uidcdc_ONcmd>2)
                {
                    uidcdc_ONcmd=0;
                }
                if(CANA_tzPSURegs_RxACDC[uidcdc_ONcmd].tzPSUDataACDC.f32DCparam_VDC >700)
                {
                    CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.uiReset_cmd[uidcdc_ONcmd] = 0x55;

                    CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.uiReset_cmd[uidcdc_ONcmd] =0xBB;
                    CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.uiReset_cmd[uidcdc_ONcmd] =0xBB;
                    CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.uiON_cmd[uidcdc_ONcmd] =0xAA;
                    CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.uiON_cmd[uidcdc_ONcmd] =0xAA;
                      if(CANA_tzTxdRegs.tzPSUData.CurrentSet>6.0)
                      {
                          CANA_tzTxdRegs.tzPSUData.CurrentSet=6.0;
                      }
                          //f32Curefset variable set as current reference we have to set manually
                    CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.f32Idc_RefSet[uidcdc_ONcmd] =f32Curefset;//CANA_tzTxdRegs.tzPSUData.CurrentSet;
                    CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.f32Idc_RefSet[uidcdc_ONcmd] =f32Curefset;//CANA_tzTxdRegs.tzPSUData.CurrentSet;
                }
                if(CANA_tzPSURegs_RxDCDC0[uidcdc_ONcmd].tzPSUDataDCDC.f32VACT>300)
                {
                    CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.uiReset_cmd[uidcdc_ONcmd] =0x55;
                    CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.uiReset_cmd[uidcdc_ONcmd] =0x55;

                }
            }
        }
   break;
    default:
        break;
   }
 }

/************************************************************************************************************************/

/*==============================================================================
 End of File
 ==============================================================================*/
