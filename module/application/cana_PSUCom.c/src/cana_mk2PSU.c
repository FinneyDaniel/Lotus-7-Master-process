/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  canaEvent.c
 @author DEEPTI.K
 @date 20-Jun-2022

 @ Communication Events of MasterProcess on CAN-A Bus
 @ with IO cards, Master Safety, VS cards
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include <stdint.h>
#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "F2837xS_device.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_can.h"
#include "cana_defs.h"
#include "canb_defs.h"
#include "cana_PSUCom.h"
#include "cana_vsc.h"
#include "hal/driverlib/can.h"
#include "state_machine.h"
#include "isr.h"
#include "control_defs.h"
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



/**************PSU ******************************/
CANA_tzACTNODE_PSU CANA_ACTNODE_ACDC,CANA_ACTNODE_DCDC0,CANA_ACTNODE_DCDC1;
union CANA_tzACTNODEs_REGS CANA_ActnodeRegs_ACDC,CANA_ActnodeRegs_DCDC0,CANA_ActnodeRegs_DCDC1;
CANA_tzRXDRegsPSU_ACDC CANA_tzPSURegs_RxACDC[CANA_mTOT_ACDCPSUNODE + 1];
CANA_tzTXDRegsPSU_ACDC CANA_tzPSURegs_TxACDC;
CANA_tzRXDRegsPSU_DCDC CANA_tzPSURegs_RxDCDC0[CANA_mTOT_DCDCPSUNODE + 1],
        CANA_tzPSURegs_RxDCDC1[CANA_mTOT_DCDCPSUNODE + 1];
CANA_tzTXDRegsPSU_DCDC CANA_tzPSURegs_TxDCDC0,CANA_tzPSURegs_TxDCDC1;
union CANA_tzACDCACTNODE_REGS CANA_tzACDC_ACTnode[CANA_mTOT_ACDCPSUNODE + 1];
union CANA_tzACDCACTNODE_REGS CANA_tzDCDC_ACTnode[CANA_mTOT_ACDCPSUNODE + 1];
union CANA_tzACDC_FLTOthREGS CANA_tzACDC_FLTOth[CANA_mTOT_ACDCPSUNODE + 1];
union CANA_tzACDC_FLTHWREGS CANA_tzACDC_HWFLT[CANA_mTOT_ACDCPSUNODE + 1];
union CANA_tzACDC_FLTSWLREGS CANA_tzACDC_SWLFLT[CANA_mTOT_ACDCPSUNODE + 1];
union CANA_tzACDC_FLTSWHREGS CANA_tzACDC_SWHFLT[CANA_mTOT_ACDCPSUNODE + 1];
union CANA_tzDCDC_FLTHWREGS CANA_tzDCDC0_HWFLT[CANA_mTOT_DCDCPSUNODE + 1],
        CANA_tzDCDC1_HWFLT[CANA_mTOT_DCDCPSUNODE + 1];
union CANA_tzDCDC_FLTSWREGS CANA_tzDCDC0_SWFLT[CANA_mTOT_DCDCPSUNODE + 1],
        CANA_tzDCDC1_SWFLT[CANA_mTOT_DCDCPSUNODE + 1];
union CANA_tzDCDC_FLTOthLREGS CANA_tzDCDC0_FLTOthL[CANA_mTOT_DCDCPSUNODE + 1];
union CANA_tzDCDC_FLTOthHREGS CANA_tzDCDC0_FLTOthH[CANA_mTOT_DCDCPSUNODE + 1];
union CANA_tzDCDC_FLTOthLREGS CANA_tzDCDC1_FLTOthL[CANA_mTOT_DCDCPSUNODE + 1];
union CANA_tzDCDC_FLTOthHREGS CANA_tzDCDC1_FLTOthH[CANA_mTOT_DCDCPSUNODE + 1];
uint16_t i_cnt = 0, j_cnt = 0, k_cnt = 0;
/*==============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

void CANA_fnPSUComFailChk();
void CANA_fnPSUTx();
extern bool can_fnEnquedata(can_tzcirc_buff *ptr, uint16_t *data, uint32_t msgID,
                     uint16_t DLC);
extern bool can_fndequedata(can_tzcirc_buff *ptr, uint16_t *data, uint32_t *msgID,
                     uint16_t *DLC);


void CANA_fnPSU_ACDC_TxCmds();
static void CANA_fnPSU_DCDC0_TxCmds();
static void CANA_fnPSU_DCDC1_TxCmds();
  void cana_fnmsgPrcsACDC(uint16_t nodeIDACDC, uint16_t msgtypeACDC,
                               uint16_t *msgbufACDC);
  void cana_fnmsgPrcsDCDC(uint16_t nodeIDDCDC, uint16_t unitIDDCDC,
                               uint16_t msgtypeDCDC, uint16_t *msgbufDCDC);
/*==============================================================================
 Local Variables
 ==============================================================================*/
uint16_t uiCANtxMsgDataACDC[8] = { 0 };
uint16_t uiCANtxMsgDataDCDC0[8] = { 0 }, uiCANtxMsgDataDCDC1[8] = { 0 },
        uiMsgFrmcount1 = 0, uiMsgFrmcount = 0, uiPSU_DCDC0Nodecount = 0,
        uiPSU_Nodecount = 0, uiMsgFrmcount2 = 0, uiPSU_DCDC1Nodecount = 0;
int32_t ui32temp_vdcRef = 0, ui32temp_idcRef = 0, ui32temp_qper = 0;

void cana_fnmsgPrcsMASTERDEBG(uint16_t *msgbufMASTER_DEBUG);
/*==============================================================================
 Local Constants
 ==============================================================================*/
/*=============================================================================
 @Function name : void CANA_fnComFailChk(void)
 @brief  function to Detect CAN failure from different IO Cards/ Voltage Sensing Cards

 @param void
 @return void
 ============================================================================ */

void CANA_fnPSUComFailChk()
{



    /********************** ACDC CAN FAIL *******************************/
    for (i_cnt = 1; i_cnt <= CANA_mTOT_ACDCPSUNODE; i_cnt++)
    {
        if (CANA_tzPSURegs_RxACDC[i_cnt].isReceived)
        {
            CANA_tzPSURegs_RxACDC[i_cnt].isReceived = 0;
            CANA_tzPSURegs_RxACDC[i_cnt].CANfailCnt = 0;
            CANA_ACTNODE_ACDC.uiNode_prsnt[i_cnt]=1;
        }
        else
        {
            if (++CANA_tzPSURegs_RxACDC[i_cnt].CANfailCnt >= 2000)
            {
                CANA_tzPSURegs_RxACDC[i_cnt].CANfailCnt = 2000;
                CANA_ACTNODE_ACDC.uiNode_prsnt[i_cnt]=0;
                CANA_tzPSURegs_RxACDC[i_cnt].tzPSUDataACDC.f32Grid_Volt = 0;
                CANA_tzPSURegs_RxACDC[i_cnt].tzPSUDataACDC.f32Grid_Curr = 0;
                CANA_tzPSURegs_RxACDC[i_cnt].tzPSUDataACDC.f32Grid_Freq = 0;
                CANA_tzPSURegs_RxACDC[i_cnt].tzPSUDataACDC.ui16Grid_Power_watt =
                        0;
                CANA_tzPSURegs_RxACDC[i_cnt].tzPSUDataACDC.f32DCparam_VDC = 0;
                CANA_tzPSURegs_RxACDC[i_cnt].tzPSUDataACDC.f32DCparam_IDC = 0;
                CANA_tzPSURegs_RxACDC[i_cnt].tzPSUDataACDC.ui16DCparam_Power_watt =
                        0;
                CANA_tzPSURegs_RxACDC[i_cnt].tzPSUDataACDC.uiDCparam_status = 0;
                CANA_tzPSURegs_RxACDC[i_cnt].tzPSUDataACDC.uiDCparam_version =
                        0;
                CANA_tzPSURegs_RxACDC[i_cnt].tzPSUDataACDC.uiTemp_P1 = 0;
                CANA_tzPSURegs_RxACDC[i_cnt].tzPSUDataACDC.uiTemp_P2 = 0;
                CANA_tzPSURegs_RxACDC[i_cnt].tzPSUDataACDC.uiTemp_P3 = 0;
                CANA_tzPSURegs_RxACDC[i_cnt].tzPSUDataACDC.uiTemp_A = 0;
            }
        }
    }
    /********************** DCDC 0 CAN FAIL *******************************/
    for (j_cnt = 1; j_cnt <= CANA_mTOT_DCDCPSUNODE; j_cnt++)
    {
        if (CANA_tzPSURegs_RxDCDC0[j_cnt].isReceived)
        {
            CANA_tzPSURegs_RxDCDC0[j_cnt].isReceived = 0;
            CANA_tzPSURegs_RxDCDC0[j_cnt].CANfailCnt = 0;
            CANA_ACTNODE_DCDC0.uiNode_prsnt[j_cnt]=1;
        }
        else
        {
            if (++CANA_tzPSURegs_RxDCDC0[j_cnt].CANfailCnt >= 2000)
            {
                CANA_tzPSURegs_RxDCDC0[j_cnt].CANfailCnt = 2000;
                CANA_ACTNODE_DCDC0.uiNode_prsnt[j_cnt]=0;
                CANA_tzPSURegs_RxDCDC0[j_cnt].tzPSUDataDCDC.f32VREF = 0;
                CANA_tzPSURegs_RxDCDC0[j_cnt].tzPSUDataDCDC.f32IREF = 0;
                CANA_tzPSURegs_RxDCDC0[j_cnt].tzPSUDataDCDC.f32VACT = 0;
                CANA_tzPSURegs_RxDCDC0[j_cnt].tzPSUDataDCDC.f32IACT = 0;
                CANA_tzPSURegs_RxDCDC0[j_cnt].tzPSUDataDCDC.f32VIN = 0;
                CANA_tzPSURegs_RxDCDC0[j_cnt].tzPSUDataDCDC.f32IIN = 0;
                CANA_tzPSURegs_RxDCDC0[j_cnt].tzPSUDataDCDC.uiDCparam_status =
                        0;
                CANA_tzPSURegs_RxDCDC0[j_cnt].tzPSUDataDCDC.uiDCparam_version =
                        0;
                CANA_tzPSURegs_RxDCDC0[j_cnt].tzPSUDataDCDC.f32CHA_Amps = 0;
                CANA_tzPSURegs_RxDCDC0[j_cnt].tzPSUDataDCDC.f32CHB_Amps = 0;
                CANA_tzPSURegs_RxDCDC0[j_cnt].tzPSUDataDCDC.f32CHC_Amps = 0;
                CANA_tzPSURegs_RxDCDC0[j_cnt].tzPSUDataDCDC.uiDerating_fact = 0;
            }
        }
    }
    /********************** DCDC 0 CAN FAIL *******************************/
    for (k_cnt = 1; k_cnt <= CANA_mTOT_DCDCPSUNODE; k_cnt++)
    {
        if (CANA_tzPSURegs_RxDCDC1[k_cnt].isReceived)
        {
            CANA_tzPSURegs_RxDCDC1[k_cnt].isReceived = 0;
            CANA_tzPSURegs_RxDCDC1[k_cnt].CANfailCnt = 0;
            CANA_ACTNODE_DCDC1.uiNode_prsnt[k_cnt]=1;
        }
        else
        {
            if (++CANA_tzPSURegs_RxDCDC1[k_cnt].CANfailCnt >= 2000)
            {
                CANA_tzPSURegs_RxDCDC1[k_cnt].CANfailCnt = 2000;
                CANA_ACTNODE_DCDC1.uiNode_prsnt[k_cnt]=0;
                CANA_tzPSURegs_RxDCDC1[k_cnt].tzPSUDataDCDC.f32VREF = 0;
                CANA_tzPSURegs_RxDCDC1[k_cnt].tzPSUDataDCDC.f32IREF = 0;
                CANA_tzPSURegs_RxDCDC1[k_cnt].tzPSUDataDCDC.f32VACT = 0;
                CANA_tzPSURegs_RxDCDC1[k_cnt].tzPSUDataDCDC.f32IACT = 0;
                CANA_tzPSURegs_RxDCDC1[k_cnt].tzPSUDataDCDC.f32VIN = 0;
                CANA_tzPSURegs_RxDCDC1[k_cnt].tzPSUDataDCDC.f32IIN = 0;
                CANA_tzPSURegs_RxDCDC1[k_cnt].tzPSUDataDCDC.uiDCparam_status =
                        0;
                CANA_tzPSURegs_RxDCDC1[k_cnt].tzPSUDataDCDC.uiDCparam_version =
                        0;
                CANA_tzPSURegs_RxDCDC1[k_cnt].tzPSUDataDCDC.f32CHA_Amps = 0;
                CANA_tzPSURegs_RxDCDC1[k_cnt].tzPSUDataDCDC.f32CHB_Amps = 0;
                CANA_tzPSURegs_RxDCDC1[k_cnt].tzPSUDataDCDC.f32CHC_Amps = 0;
                CANA_tzPSURegs_RxDCDC1[k_cnt].tzPSUDataDCDC.uiDerating_fact = 0;
            }
        }
    }



    CANA_ActnodeRegs_ACDC.bit.bt_node1 = (CANA_ACTNODE_ACDC.uiNode_prsnt[1]==1)?1:0;
    CANA_ActnodeRegs_ACDC.bit.bt_node2 = (CANA_ACTNODE_ACDC.uiNode_prsnt[2]==1)?1:0;
    CANA_ActnodeRegs_ACDC.bit.bt_node3 = (CANA_ACTNODE_ACDC.uiNode_prsnt[3]==1)?1:0;
    CANA_ActnodeRegs_ACDC.bit.bt_node4 = (CANA_ACTNODE_ACDC.uiNode_prsnt[4]==1)?1:0;
    CANA_ActnodeRegs_ACDC.bit.bt_node5 = (CANA_ACTNODE_ACDC.uiNode_prsnt[5]==1)?1:0;
    CANA_ActnodeRegs_ACDC.bit.bt_node6 = (CANA_ACTNODE_ACDC.uiNode_prsnt[6]==1)?1:0;
    CANA_ActnodeRegs_ACDC.bit.bt_node7 = (CANA_ACTNODE_ACDC.uiNode_prsnt[7]==1)?1:0;
    CANA_ActnodeRegs_ACDC.bit.bt_node8 = (CANA_ACTNODE_ACDC.uiNode_prsnt[8]==1)?1:0;
    CANA_ActnodeRegs_ACDC.bit.bt_node9 = (CANA_ACTNODE_ACDC.uiNode_prsnt[9]==1)?1:0;

    CANA_ActnodeRegs_DCDC0.bit.bt_node1 =  ((CANA_ACTNODE_DCDC0.uiNode_prsnt[1]==0)||(CANA_tzDCDC0_FLTOthH[1].all >= 1)||(CANA_tzDCDC0_FLTOthL[1].all >= 1)||(CANA_tzDCDC0_HWFLT[1].all >= 1)||(CANA_tzDCDC0_SWFLT[1].all >= 1))?0:1;
    CANA_ActnodeRegs_DCDC0.bit.bt_node2 =  ((CANA_ACTNODE_DCDC0.uiNode_prsnt[2]==0)||(CANA_tzDCDC0_FLTOthH[2].all >= 1)||(CANA_tzDCDC0_FLTOthL[2].all >= 1)||(CANA_tzDCDC0_HWFLT[2].all >= 1)||(CANA_tzDCDC0_SWFLT[2].all >= 1))?0:1;
    CANA_ActnodeRegs_DCDC0.bit.bt_node3 =  ((CANA_ACTNODE_DCDC0.uiNode_prsnt[3]==0)||(CANA_tzDCDC0_FLTOthH[3].all >= 1)||(CANA_tzDCDC0_FLTOthL[3].all >= 1)||(CANA_tzDCDC0_HWFLT[3].all >= 1)||(CANA_tzDCDC0_SWFLT[3].all >= 1))?0:1;
    CANA_ActnodeRegs_DCDC0.bit.bt_node4 =  ((CANA_ACTNODE_DCDC0.uiNode_prsnt[4]==0)||(CANA_tzDCDC0_FLTOthH[4].all >= 1)||(CANA_tzDCDC0_FLTOthL[4].all >= 1)||(CANA_tzDCDC0_HWFLT[4].all >= 1)||(CANA_tzDCDC0_SWFLT[4].all >= 1))?0:1;
    CANA_ActnodeRegs_DCDC0.bit.bt_node5 =  ((CANA_ACTNODE_DCDC0.uiNode_prsnt[5]==0)||(CANA_tzDCDC0_FLTOthH[5].all >= 1)||(CANA_tzDCDC0_FLTOthL[5].all >= 1)||(CANA_tzDCDC0_HWFLT[5].all >= 1)||(CANA_tzDCDC0_SWFLT[5].all >= 1))?0:1;
    CANA_ActnodeRegs_DCDC0.bit.bt_node6 =  ((CANA_ACTNODE_DCDC0.uiNode_prsnt[6]==0)||(CANA_tzDCDC0_FLTOthH[6].all >= 1)||(CANA_tzDCDC0_FLTOthL[6].all >= 1)||(CANA_tzDCDC0_HWFLT[6].all >= 1)||(CANA_tzDCDC0_SWFLT[6].all >= 1))?0:1;
    CANA_ActnodeRegs_DCDC0.bit.bt_node7 =  ((CANA_ACTNODE_DCDC0.uiNode_prsnt[7]==0)||(CANA_tzDCDC0_FLTOthH[7].all >= 1)||(CANA_tzDCDC0_FLTOthL[7].all >= 1)||(CANA_tzDCDC0_HWFLT[7].all >= 1)||(CANA_tzDCDC0_SWFLT[7].all >= 1))?0:1;
    CANA_ActnodeRegs_DCDC0.bit.bt_node8 =  ((CANA_ACTNODE_DCDC0.uiNode_prsnt[8]==0)||(CANA_tzDCDC0_FLTOthH[8].all >= 1)||(CANA_tzDCDC0_FLTOthL[8].all >= 1)||(CANA_tzDCDC0_HWFLT[8].all >= 1)||(CANA_tzDCDC0_SWFLT[8].all >= 1))?0:1;
    CANA_ActnodeRegs_DCDC0.bit.bt_node9 =  ((CANA_ACTNODE_DCDC0.uiNode_prsnt[9]==0)||(CANA_tzDCDC0_FLTOthH[9].all >= 1)||(CANA_tzDCDC0_FLTOthL[9].all >= 1)||(CANA_tzDCDC0_HWFLT[9].all >= 1)||(CANA_tzDCDC0_SWFLT[9].all >= 1))?0:1;

    CANA_ActnodeRegs_DCDC1.bit.bt_node1 = ((CANA_ACTNODE_DCDC1.uiNode_prsnt[1]==0)||(CANA_tzDCDC1_FLTOthH[1].all >= 1)||(CANA_tzDCDC1_FLTOthL[1].all >= 1)||(CANA_tzDCDC1_HWFLT[1].all >= 1)||(CANA_tzDCDC1_SWFLT[1].all >= 1))?0:1;
    CANA_ActnodeRegs_DCDC1.bit.bt_node2 = ((CANA_ACTNODE_DCDC1.uiNode_prsnt[2]==0)||(CANA_tzDCDC1_FLTOthH[2].all >= 1)||(CANA_tzDCDC1_FLTOthL[2].all >= 1)||(CANA_tzDCDC1_HWFLT[2].all >= 1)||(CANA_tzDCDC1_SWFLT[2].all >= 1))?0:1;
    CANA_ActnodeRegs_DCDC1.bit.bt_node3 = ((CANA_ACTNODE_DCDC1.uiNode_prsnt[3]==0)||(CANA_tzDCDC1_FLTOthH[3].all >= 1)||(CANA_tzDCDC1_FLTOthL[3].all >= 1)||(CANA_tzDCDC1_HWFLT[3].all >= 1)||(CANA_tzDCDC1_SWFLT[3].all >= 1))?0:1;
    CANA_ActnodeRegs_DCDC1.bit.bt_node4 = ((CANA_ACTNODE_DCDC1.uiNode_prsnt[4]==0)||(CANA_tzDCDC1_FLTOthH[4].all >= 1)||(CANA_tzDCDC1_FLTOthL[4].all >= 1)||(CANA_tzDCDC1_HWFLT[4].all >= 1)||(CANA_tzDCDC1_SWFLT[4].all >= 1))?0:1;
    CANA_ActnodeRegs_DCDC1.bit.bt_node5 = ((CANA_ACTNODE_DCDC1.uiNode_prsnt[5]==0)||(CANA_tzDCDC1_FLTOthH[5].all >= 1)||(CANA_tzDCDC1_FLTOthL[5].all >= 1)||(CANA_tzDCDC1_HWFLT[5].all >= 1)||(CANA_tzDCDC1_SWFLT[5].all >= 1))?0:1;
    CANA_ActnodeRegs_DCDC1.bit.bt_node6 = ((CANA_ACTNODE_DCDC1.uiNode_prsnt[6]==0)||(CANA_tzDCDC1_FLTOthH[6].all >= 1)||(CANA_tzDCDC1_FLTOthL[6].all >= 1)||(CANA_tzDCDC1_HWFLT[6].all >= 1)||(CANA_tzDCDC1_SWFLT[6].all >= 1))?0:1;
    CANA_ActnodeRegs_DCDC1.bit.bt_node7 = ((CANA_ACTNODE_DCDC1.uiNode_prsnt[7]==0)||(CANA_tzDCDC1_FLTOthH[7].all >= 1)||(CANA_tzDCDC1_FLTOthL[7].all >= 1)||(CANA_tzDCDC1_HWFLT[7].all >= 1)||(CANA_tzDCDC1_SWFLT[7].all >= 1))?0:1;
    CANA_ActnodeRegs_DCDC1.bit.bt_node8 = ((CANA_ACTNODE_DCDC1.uiNode_prsnt[8]==0)||(CANA_tzDCDC1_FLTOthH[8].all >= 1)||(CANA_tzDCDC1_FLTOthL[8].all >= 1)||(CANA_tzDCDC1_HWFLT[8].all >= 1)||(CANA_tzDCDC1_SWFLT[8].all >= 1))?0:1;
    CANA_ActnodeRegs_DCDC1.bit.bt_node9 = ((CANA_ACTNODE_DCDC1.uiNode_prsnt[9]==0)||(CANA_tzDCDC1_FLTOthH[9].all >= 1)||(CANA_tzDCDC1_FLTOthL[9].all >= 1)||(CANA_tzDCDC1_HWFLT[9].all >= 1)||(CANA_tzDCDC1_SWFLT[9].all >= 1))?0:1;
    /********************************************************************************/
}

/*=============================================================================
 @Function name : void CANA_fnTx(void)
 @brief  function to transmit CAN Messages over CAN-A bus to IO Cards

 @param void
 @return void
 ============================================================================ */

void CANA_fnPSUTx()
{
    // Common Messages Irrespective of States

    // Below Commands Can be used for Manual Testing Mode
    CANA_fnPSU_ACDC_TxCmds(); //Heartbeat

    CANA_fnPSU_DCDC0_TxCmds(); //Heartbeat
    CANA_fnPSU_DCDC1_TxCmds(); //Heartbeat

}

/*******************************************************************************************/
void CANA_fnPSU_ACDC_TxCmds()
{

    uiMsgFrmcount++;
    if (uiMsgFrmcount > 2)
    {
        uiMsgFrmcount = 0;
    }

    switch (uiMsgFrmcount)
    {
    case 1:
        uiPSU_Nodecount++;
        CAN_setupMessageObject(
        CANA_BASE,
                               CAN_mMAILBOX_30,
                               CANA_mTX_mPSUACDCMSGID1 | uiPSU_Nodecount,
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN4);

        uiCANtxMsgDataACDC[0] = CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.uiON_cmd[uiPSU_Nodecount];

        uiCANtxMsgDataACDC[1] =
                CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.uiReset_cmd[uiPSU_Nodecount];
//        CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.ui32Vdc_RefSet[uiPSU_Nodecount] = 800; ////////////////default
        ui32temp_vdcRef = (CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.ui32Vdc_RefSet[uiPSU_Nodecount]
                * 10);

        uiCANtxMsgDataACDC[2] = (ui32temp_vdcRef & 0xFF00) >> 8;
        uiCANtxMsgDataACDC[3] = (ui32temp_vdcRef & 0x00FF);

        uiCANtxMsgDataACDC[4] = 0;
        uiCANtxMsgDataACDC[5] = 0;
        uiCANtxMsgDataACDC[6] = 0;
        uiCANtxMsgDataACDC[7] = 0;

        CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_30, CAN_mLEN4,
                        uiCANtxMsgDataACDC);

        break;
    case 2:
        CAN_setupMessageObject(
        CANA_BASE,
                               CAN_mMAILBOX_30,
                               CANA_mTX_mPSUACDCMSGID2 | uiPSU_Nodecount,
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN6);
        CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.uiQ_cmd[uiPSU_Nodecount] = 0; //No_Q
        uiCANtxMsgDataACDC[0] = CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.uiQ_cmd[uiPSU_Nodecount];

        CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.uiQ_dir[uiPSU_Nodecount] = 0; //Lag
        uiCANtxMsgDataACDC[1] = CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.uiQ_dir[uiPSU_Nodecount];

        CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.ui32Q_percent[uiPSU_Nodecount]= 0;
        ui32temp_qper = CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.ui32Q_percent[uiPSU_Nodecount];

        uiCANtxMsgDataACDC[2] = (ui32temp_qper & 0xFF00) >> 8;
        uiCANtxMsgDataACDC[3] = (ui32temp_qper & 0x00FF);
        CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.ui32PF[uiPSU_Nodecount] = 1; ////////////////default
        ui32temp_qper = (CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.ui32PF[uiPSU_Nodecount] * 100);
        uiCANtxMsgDataACDC[4] = (ui32temp_qper & 0xFF00) >> 8;
        uiCANtxMsgDataACDC[5] = (ui32temp_qper & 0x00FF);

        uiCANtxMsgDataACDC[6] = 0;
        uiCANtxMsgDataACDC[7] = 0;

        CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_30, CAN_mLEN6,
                        uiCANtxMsgDataACDC);
        if (uiPSU_Nodecount >= 9)
        {
            uiPSU_Nodecount = 0;
        }

        break;
    default:
        break;
    }
}
/*******************************************************************************************/
static void CANA_fnPSU_DCDC0_TxCmds()
{

    uiMsgFrmcount1++;
    if (uiMsgFrmcount1 > 2)
    {
        uiMsgFrmcount1 = 0;
    }
    switch (uiMsgFrmcount1)
    {
    case 1:
        uiPSU_DCDC0Nodecount++;
        CAN_setupMessageObject(
        CANA_BASE,
                               CAN_mMAILBOX_28,
                               CANA_mTX_mPSUDCDC0MSGID1 | uiPSU_DCDC0Nodecount,
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN6);

        uiCANtxMsgDataDCDC0[0] = CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.uiON_cmd[uiPSU_DCDC0Nodecount];

        uiCANtxMsgDataDCDC0[1] =
                CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.uiReset_cmd[uiPSU_DCDC0Nodecount];
        // CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.ui32Vdc_RefSet[uiPSU_DCDC0Nodecount] = 450;
        ui32temp_vdcRef = (CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.ui32Vdc_RefSet[uiPSU_DCDC0Nodecount]
                * 10);

        uiCANtxMsgDataDCDC0[2] = (ui32temp_vdcRef & 0xFF00) >> 8;
        uiCANtxMsgDataDCDC0[3] = (ui32temp_vdcRef & 0x00FF);
        //CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.f32Idc_RefSet[uiPSU_DCDC0Nodecount] =67;
        ui32temp_vdcRef = (CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.f32Idc_RefSet[uiPSU_DCDC0Nodecount]
                * 10);

        uiCANtxMsgDataDCDC0[4] = (ui32temp_vdcRef & 0xFF00) >> 8;
        uiCANtxMsgDataDCDC0[5] = (ui32temp_vdcRef & 0x00FF);
        uiCANtxMsgDataDCDC0[6] = 0;
        uiCANtxMsgDataDCDC0[7] = 0;

        CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_28, CAN_mLEN6,
                        uiCANtxMsgDataDCDC0);

        break;
    case 2:
        CAN_setupMessageObject(
        CANA_BASE,
                               CAN_mMAILBOX_28,
                               CANA_mTX_mPSUDCDC0MSGID2 | uiPSU_DCDC0Nodecount,
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN6);

        uiCANtxMsgDataDCDC0[0] =
                CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.uiImpedanceSpc_mode[uiPSU_DCDC0Nodecount];

        uiCANtxMsgDataDCDC0[1] =
                CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.uiImpedanceSpc_mode[uiPSU_DCDC0Nodecount];

        ui32temp_qper = CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.ui32FREQ[uiPSU_DCDC0Nodecount];

        uiCANtxMsgDataDCDC0[2] = (ui32temp_qper & 0xFF00) >> 8;
        uiCANtxMsgDataDCDC0[3] = (ui32temp_qper & 0x00FF);

        ui32temp_qper = (CANA_tzPSURegs_TxDCDC0.tzPSUTxDataDCDC.ui32Magnitude[uiPSU_DCDC0Nodecount]);
        uiCANtxMsgDataDCDC0[4] = (ui32temp_qper & 0xFF00) >> 8;
        uiCANtxMsgDataDCDC0[5] = (ui32temp_qper & 0x00FF);

        uiCANtxMsgDataDCDC0[6] = 0;
        uiCANtxMsgDataDCDC0[7] = 0;

        CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_28, CAN_mLEN6,
                        uiCANtxMsgDataDCDC0);
        if (uiPSU_DCDC0Nodecount >= 9)
        {
            uiPSU_DCDC0Nodecount = 0;
        }

        break;
    default:
        break;
    }
}
/************************************************************************************/
/*******************************************************************************************/
static void CANA_fnPSU_DCDC1_TxCmds()
{

    uiMsgFrmcount2++;
    if (uiMsgFrmcount2 > 2)
    {
        uiMsgFrmcount2 = 0;
    }
    switch (uiMsgFrmcount1)
    {
    case 1:
        uiPSU_DCDC1Nodecount++;
        CAN_setupMessageObject(
        CANA_BASE,
                               CAN_mMAILBOX_29,
                               CANA_mTX_mPSUDCDC1MSGID1 | uiPSU_DCDC1Nodecount,
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN6);

        uiCANtxMsgDataDCDC1[0] = CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.uiON_cmd[uiPSU_DCDC0Nodecount];

        uiCANtxMsgDataDCDC1[1] =
                CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.uiReset_cmd[uiPSU_DCDC0Nodecount];
        //CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.ui32Vdc_RefSet[uiPSU_DCDC0Nodecount] = 450;
        ui32temp_vdcRef = (CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.ui32Vdc_RefSet[uiPSU_DCDC0Nodecount]
                * 10);

        uiCANtxMsgDataDCDC1[2] = (ui32temp_vdcRef & 0xFF00) >> 8;
        uiCANtxMsgDataDCDC1[3] = (ui32temp_vdcRef & 0x00FF);
       // CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.f32Idc_RefSet[uiPSU_DCDC0Nodecount]=67;
        ui32temp_vdcRef = (CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.f32Idc_RefSet[uiPSU_DCDC0Nodecount]
                * 10);

        uiCANtxMsgDataDCDC1[4] = (ui32temp_vdcRef & 0xFF00) >> 8;
        uiCANtxMsgDataDCDC1[5] = (ui32temp_vdcRef & 0x00FF);
        uiCANtxMsgDataDCDC1[6] = 0;
        uiCANtxMsgDataDCDC1[7] = 0;

        CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_29, CAN_mLEN6,
                        uiCANtxMsgDataDCDC1);

        break;
    case 2:
        CAN_setupMessageObject(
        CANA_BASE,
                               CAN_mMAILBOX_29,
                               CANA_mTX_mPSUDCDC1MSGID2 | uiPSU_DCDC1Nodecount,
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN6);

        uiCANtxMsgDataDCDC1[0] =
                CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.uiImpedanceSpc_mode[uiPSU_DCDC0Nodecount];

        uiCANtxMsgDataDCDC1[1] =
                CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.uiImpedanceSpc_mode[uiPSU_DCDC0Nodecount];

        ui32temp_qper = CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.ui32FREQ[uiPSU_DCDC0Nodecount];

        uiCANtxMsgDataDCDC1[2] = (ui32temp_qper & 0xFF00) >> 8;
        uiCANtxMsgDataDCDC1[3] = (ui32temp_qper & 0x00FF);

        ui32temp_qper = (CANA_tzPSURegs_TxDCDC1.tzPSUTxDataDCDC.ui32Magnitude[uiPSU_DCDC0Nodecount]);
        uiCANtxMsgDataDCDC1[4] = (ui32temp_qper & 0xFF00) >> 8;
        uiCANtxMsgDataDCDC1[5] = (ui32temp_qper & 0x00FF);

        uiCANtxMsgDataDCDC1[6] = 0;
        uiCANtxMsgDataDCDC1[7] = 0;

        CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_29, CAN_mLEN6,
                        uiCANtxMsgDataDCDC1);
        if (uiPSU_DCDC1Nodecount >= 9)
        {
            uiPSU_DCDC1Nodecount = 0;
        }

        break;
    default:
        break;
    }
}
/************************************************************************************/

/*=============================================================================
 @Function name : void can_fnmsgProcess(void)
 @brief  function to Process ACDC

 @param void
 @return void
 ============================================================================ */
  void cana_fnmsgPrcsACDC(uint16_t nodeIDACDC, uint16_t msgtypeACDC,
                               uint16_t *msgbufACDC)
{

    // Messages from Master safety Controller are being processed here
    CANA_tzPSURegs_RxACDC[nodeIDACDC].isReceived = 1;
    switch (msgtypeACDC)
    {
    case 1:

        CANA_tzPSURegs_RxACDC[nodeIDACDC].tzPSUDataACDC.f32Grid_Volt =
                ((msgbufACDC[0] << 8) | (msgbufACDC[1])) * 0.1;
        CANA_tzPSURegs_RxACDC[nodeIDACDC].tzPSUDataACDC.f32Grid_Freq =
                ((msgbufACDC[2] << 8) | (msgbufACDC[3])) * 0.01;
        CANA_tzPSURegs_RxACDC[nodeIDACDC].tzPSUDataACDC.f32Grid_Curr =
                ((msgbufACDC[4] << 8) | (msgbufACDC[5])) * 0.1;
        CANA_tzPSURegs_RxACDC[nodeIDACDC].tzPSUDataACDC.ui16Grid_Power_watt =
                ((msgbufACDC[6] << 8) | (msgbufACDC[7]));
        break;
    case 2:
        CANA_tzPSURegs_RxACDC[nodeIDACDC].tzPSUDataACDC.f32DCparam_VDC =
                ((msgbufACDC[0] << 8) | (msgbufACDC[1])) * 0.1;
        CANA_tzPSURegs_RxACDC[nodeIDACDC].tzPSUDataACDC.f32DCparam_IDC =
                ((msgbufACDC[2] << 8) | (msgbufACDC[3])) * 0.01;
        CANA_tzPSURegs_RxACDC[nodeIDACDC].tzPSUDataACDC.ui16DCparam_Power_watt =
                ((msgbufACDC[4] << 8) | (msgbufACDC[5]));
        CANA_tzPSURegs_RxACDC[nodeIDACDC].tzPSUDataACDC.uiDCparam_status =
                msgbufACDC[6];
        CANA_tzPSURegs_RxACDC[nodeIDACDC].tzPSUDataACDC.uiDCparam_version =
                msgbufACDC[7];
        break;
    case 3:
        CANA_tzPSURegs_RxACDC[nodeIDACDC].tzPSUDataACDC.uiTemp_P1 =
                msgbufACDC[0];
        CANA_tzPSURegs_RxACDC[nodeIDACDC].tzPSUDataACDC.uiTemp_P2 =
                msgbufACDC[1];
        CANA_tzPSURegs_RxACDC[nodeIDACDC].tzPSUDataACDC.uiTemp_P3 =
                msgbufACDC[2];
        CANA_tzPSURegs_RxACDC[nodeIDACDC].tzPSUDataACDC.uiTemp_A =
                msgbufACDC[3];
        //CANA_tzACDC_FLTOth[nodeIDACDC].all= msgbufACDC[4];
        CANA_tzPSURegs_RxACDC[nodeIDACDC].tzPSUDataACDC.uiDerating_fact =
                msgbufACDC[4];
        CANA_tzACDC_HWFLT[nodeIDACDC].all = msgbufACDC[5];
        CANA_tzACDC_SWLFLT[nodeIDACDC].all = msgbufACDC[6];
        CANA_tzACDC_SWHFLT[nodeIDACDC].all = msgbufACDC[7];
        break;

    default:
        break;

    }

}
/*=============================================================================
 @Function name : void can_fnmsgProcess(void)
 @brief  function to Process DCDC

 @param void
 @return void
 ============================================================================ */
  void cana_fnmsgPrcsDCDC(uint16_t nodeIDDCDC, uint16_t unitIDDCDC,
                               uint16_t msgtypeDCDC, uint16_t *msgbufDCDC)
{

    // Messages from Master safety Controller are being processed here
    switch (unitIDDCDC)
    {
    case 0:
        CANA_tzPSURegs_RxDCDC0[nodeIDDCDC].isReceived = 1;
        switch (msgtypeDCDC)
        {
        case 1:

            CANA_tzPSURegs_RxDCDC0[nodeIDDCDC].tzPSUDataDCDC.f32VREF =
                    ((msgbufDCDC[0] << 8) | (msgbufDCDC[1])) * 0.1;
            CANA_tzPSURegs_RxDCDC0[nodeIDDCDC].tzPSUDataDCDC.f32IREF =
                    ((msgbufDCDC[2] << 8) | (msgbufDCDC[3])) * 0.1;
            CANA_tzPSURegs_RxDCDC0[nodeIDDCDC].tzPSUDataDCDC.f32VACT =
                    ((msgbufDCDC[4] << 8) | (msgbufDCDC[5])) * 0.1;
            CANA_tzPSURegs_RxDCDC0[nodeIDDCDC].tzPSUDataDCDC.f32IACT =
                    ((msgbufDCDC[6] << 8) | (msgbufDCDC[7])) *0.1;// added 08/02/2023
            break;
        case 2:
            CANA_tzPSURegs_RxDCDC0[nodeIDDCDC].tzPSUDataDCDC.f32VIN =
                    ((msgbufDCDC[0] << 8) | (msgbufDCDC[1])) * 0.1;
            CANA_tzPSURegs_RxDCDC0[nodeIDDCDC].tzPSUDataDCDC.f32IIN =
                    ((msgbufDCDC[2] << 8) | (msgbufDCDC[3])) * 0.1;
            CANA_tzPSURegs_RxDCDC0[nodeIDDCDC].tzPSUDataDCDC.f32TEMP =
                    ((msgbufDCDC[4] << 8) | (msgbufDCDC[5])) * 0.01;
            CANA_tzPSURegs_RxDCDC0[nodeIDDCDC].tzPSUDataDCDC.uiDCparam_status =
                    msgbufDCDC[6];
            CANA_tzPSURegs_RxDCDC0[nodeIDDCDC].tzPSUDataDCDC.uiDCparam_version =
                    msgbufDCDC[7];
            break;
        case 3:
            CANA_tzPSURegs_RxDCDC0[nodeIDDCDC].tzPSUDataDCDC.f32CHA_Amps =
                    msgbufDCDC[0] * 0.1;
            CANA_tzPSURegs_RxDCDC0[nodeIDDCDC].tzPSUDataDCDC.f32CHB_Amps =
                    msgbufDCDC[1] * 0.1;
            CANA_tzPSURegs_RxDCDC0[nodeIDDCDC].tzPSUDataDCDC.f32CHC_Amps =
                    msgbufDCDC[2] * 0.1;
            CANA_tzPSURegs_RxDCDC1[nodeIDDCDC].tzPSUDataDCDC.uiDerating_fact =
                    msgbufDCDC[3];
            CANA_tzDCDC0_FLTOthH[nodeIDDCDC].all = msgbufDCDC[4];
            CANA_tzDCDC0_FLTOthL[nodeIDDCDC].all = msgbufDCDC[5];
            CANA_tzDCDC0_HWFLT[nodeIDDCDC].all = msgbufDCDC[6];
            CANA_tzDCDC0_SWFLT[nodeIDDCDC].all = msgbufDCDC[7];
            break;

        default:
            break;

        }
        break;
    case 1:
        CANA_tzPSURegs_RxDCDC1[nodeIDDCDC].isReceived = 1;
        switch (msgtypeDCDC)
        {

        case 1:

            CANA_tzPSURegs_RxDCDC1[nodeIDDCDC].tzPSUDataDCDC.f32VREF =
                    ((msgbufDCDC[0] << 8) | (msgbufDCDC[1])) * 0.1;
            CANA_tzPSURegs_RxDCDC1[nodeIDDCDC].tzPSUDataDCDC.f32IREF =
                    ((msgbufDCDC[2] << 8) | (msgbufDCDC[3])) * 0.1;
            CANA_tzPSURegs_RxDCDC1[nodeIDDCDC].tzPSUDataDCDC.f32VACT =
                    ((msgbufDCDC[4] << 8) | (msgbufDCDC[5])) * 0.1;
            CANA_tzPSURegs_RxDCDC1[nodeIDDCDC].tzPSUDataDCDC.f32IACT =
                    ((msgbufDCDC[6] << 8) | (msgbufDCDC[7])) * 0.1;
            break;
        case 2:
            CANA_tzPSURegs_RxDCDC1[nodeIDDCDC].tzPSUDataDCDC.f32VIN =
                    ((msgbufDCDC[0] << 8) | (msgbufDCDC[1])) * 0.1;
            CANA_tzPSURegs_RxDCDC1[nodeIDDCDC].tzPSUDataDCDC.f32IIN =
                    ((msgbufDCDC[2] << 8) | (msgbufDCDC[3])) * 0.1;
            CANA_tzPSURegs_RxDCDC1[nodeIDDCDC].tzPSUDataDCDC.f32TEMP =
                    ((msgbufDCDC[4] << 8) | (msgbufDCDC[5])) * 0.01;
            CANA_tzPSURegs_RxDCDC1[nodeIDDCDC].tzPSUDataDCDC.uiDCparam_status =
                    msgbufDCDC[6];
            CANA_tzPSURegs_RxDCDC1[nodeIDDCDC].tzPSUDataDCDC.uiDCparam_version =
                    msgbufDCDC[7];
            break;
        case 3:
            CANA_tzPSURegs_RxDCDC1[nodeIDDCDC].tzPSUDataDCDC.f32CHA_Amps =
                    msgbufDCDC[0];
            CANA_tzPSURegs_RxDCDC1[nodeIDDCDC].tzPSUDataDCDC.f32CHB_Amps =
                    msgbufDCDC[1];
            CANA_tzPSURegs_RxDCDC1[nodeIDDCDC].tzPSUDataDCDC.f32CHC_Amps =
                    msgbufDCDC[2];
            CANA_tzPSURegs_RxDCDC1[nodeIDDCDC].tzPSUDataDCDC.uiDerating_fact =
                    msgbufDCDC[3];
            CANA_tzDCDC1_FLTOthH[nodeIDDCDC].all = msgbufDCDC[4];
            CANA_tzDCDC1_FLTOthL[nodeIDDCDC].all = msgbufDCDC[5];
            CANA_tzDCDC1_HWFLT[nodeIDDCDC].all = msgbufDCDC[6];
            CANA_tzDCDC1_SWFLT[nodeIDDCDC].all = msgbufDCDC[7];
            break;

        default:
            break;

        }

        break;

    default:
        break;

    }

}
  /*=============================================================================
   @Function name : void can_fnmsgProcess(void)
   @brief  function to Process DCDC

   @param void
   @return void
   ============================================================================ */
    void cana_fnmsgPrcsMASTERDEBG(uint16_t *msgbufMASTER_DEBUG)
      {

        CANA_tzMP_PSU_DebgRegs.ACDC_on_cmd = msgbufMASTER_DEBUG[0];
        CANA_tzMP_PSU_DebgRegs.ACDC_node_no = msgbufMASTER_DEBUG[1];
        CANA_tzMP_PSU_DebgRegs.ACDC_reset_cmd = msgbufMASTER_DEBUG[2];

        if (((CANA_tzMP_PSU_DebgRegs.ACDC_node_no & 0x0001) == 0x0001) && (CANA_tzMP_PSU_DebgRegs.ACDC_on_cmd == 0xAA))
        {
            CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.uiON_cmd[1] = 0xAA;
        }
        else if(((CANA_tzMP_PSU_DebgRegs.ACDC_node_no & 0x0001) == 0x0001) && (CANA_tzMP_PSU_DebgRegs.ACDC_on_cmd == 0x55))
        {
            CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.uiON_cmd[1] = 0x55;
        }
        if (((CANA_tzMP_PSU_DebgRegs.ACDC_node_no & 0x0002) == 0x0002) && (CANA_tzMP_PSU_DebgRegs.ACDC_on_cmd == 0xAA))
        {
            CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.uiON_cmd[2] = 0xAA;
        }
        else if(((CANA_tzMP_PSU_DebgRegs.ACDC_node_no & 0x0002) == 0x0002) && (CANA_tzMP_PSU_DebgRegs.ACDC_on_cmd == 0x55))
        {
            CANA_tzPSURegs_TxACDC.tzPSUTxDataACDC.uiON_cmd[2] = 0x55;
        }

      }

/*==============================================================================
 End of File
 ==============================================================================*/
