/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  oi_canb.c
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
#include "cana_defs.h"
#include "cana_PSUCom.h"
#include "../main/ver.h"

#include "canb_defs.h"

#include "scheduler.h"
#include "control_defs.h"
#include "mathcalc.h"
#include "cana_vsc.h"

/*==============================================================================
 Defines
 ==============================================================================*/

CIRC1_BUF_DEF(uiRxbufferSP, 100);

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/

CANB_tzSITERXREGS CANB_tzSiteRxRegs;
CANB_tzSITEREGS CANB_tzSiteRegs;
extern union H2_tzVALVES H2_tzValves_PHS;
/*==============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

void CANB_fnTX_SiteEvent(void);
void canb_fnParamsUpdate(void);
void CANB_fnRXevent(void);

void CANB_fnTask(void);
void CAN_fnComFailChk(void);

static void canb_fnmsgPrcsSP(uint16_t uiMsgtype, uint16_t *msgDataSP);

bool canb_fnEnquedata(canb_tzcirc_buff *ptr, uint16_t *data, uint32_t msgID,
                      uint16_t DLC);
bool canb_fndequedata(canb_tzcirc_buff *ptr, uint16_t *data, uint32_t *msgID,
                      uint16_t *DLC);

/*==============================================================================
 Local Variables
 ==============================================================================*/

uint16_t ui16RxMsgDataSite7[8] = { 0 };
uint16_t ui16RxMsgDataSite8[8] = { 0 };

uint16_t ui16txMsgDataSite1[8] = { 0 };
uint16_t ui16txMsgDataSite2[8] = { 0 };
uint16_t ui16txMsgDataSite3[8] = { 0 };
uint16_t ui16txMsgDataSite4[8] = { 0 };
uint16_t ui16txMsgDataSite5[8] = { 0 };

uint16_t ui16rxMsgSP[8] = { 0 };
uint16_t uirxPrcsMsgSP[8] = { 0 };

uint16_t ui16CANBTxCntPSU = 0, ui16CANBTxNodeCntPSU = 0;

uint16_t ui16nodeIDVS = 0, ui16CANBTxCntSnsr = 0, ui16CANBTxCntFlts = 0,
        ui16CANBTxCntVSC = 0;

uint32_t u32CANBmsgID1 = 0;
uint16_t uiCANBDataLength1 = 0;
extern uint16_t i_cnt,j_cnt,k_cnt;
uint16_t ui16Actnode_ACDC_cnt=0;
uint16_t ui16CANBTxCntACDC = 0,ui16Actnode_DCDC0_cnt = 0,ui16Actnode_DCDC1_cnt = 0, ui16CANBTxCntDCDC0 = 0, ui16CANBTxCntDCDC1 = 0;
static uint16_t ui16CANBTxCntVer = 0;

/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
void CANB_fnTX_SiteEvent(void)
{
    static uint16_t cnt = 0;

    if (++cnt < 10)
        return;
    cnt = 0;

    canb_fnParamsUpdate();

    // ACDC Power Supply messages

    ui16CANBTxCntPSU++;

    if (ui16CANBTxCntPSU > 6)
    {
        ui16CANBTxCntPSU = 1;
    }

    switch (ui16CANBTxCntPSU)
    {

    case 1:

        CANB_tzSiteRegs.txHBCnt++;
        if (CANB_tzSiteRegs.txHBCnt >= 255)
        {
            CANB_tzSiteRegs.txHBCnt = 0;
        }
        CAN_setupMessageObject(CANB_BASE, CANB_mMAILBOX_1,
                               (CANB_mTX_mPSUACDCMSGID1),
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                               0x1FFFFFFF,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN8);

        ui16txMsgDataSite1[0] = CANB_tzSiteRegs.txHBCnt;
        ui16txMsgDataSite1[1] = 0x01;
        ui16txMsgDataSite1[2] = (CANA_ActnodeRegs_ACDC.all & 0xFF00) >> 8;
        ui16txMsgDataSite1[3] = (CANA_ActnodeRegs_ACDC.all & 0x00FF);

        ui16txMsgDataSite1[4] = (CANA_ActnodeRegs_DCDC0.all & 0xFF00) >> 8;
        ui16txMsgDataSite1[5] = (CANA_ActnodeRegs_DCDC0.all & 0x00FF);

        ui16txMsgDataSite1[6] = (CANA_ActnodeRegs_DCDC1.all & 0xFF00) >> 8;
        ui16txMsgDataSite1[7] = (CANA_ActnodeRegs_DCDC1.all & 0x00FF);

        CAN_sendMessage(CANB_BASE, CANB_mMAILBOX_1, CAN_mLEN8,
                        ui16txMsgDataSite1);

        break;

    case 2:

        ui16Actnode_ACDC_cnt++;  //can fail
        if(ui16Actnode_ACDC_cnt >9)
         {
             ui16Actnode_ACDC_cnt =1;
         }

         CAN_setupMessageObject(CANB_BASE, CANB_mMAILBOX_1,
                               (CANB_mTX_mPSUACDCMSGID1),// | (ui16Actnode_ACDC_cnt)),
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                               0x1FFFFFFF,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN8);
         CANB_tzSiteRegs.ui16Grid_Volt[ui16Actnode_ACDC_cnt]=4125;
        ui16txMsgDataSite1[0] = ui16Actnode_ACDC_cnt;
        ui16txMsgDataSite1[1] = 0x02;
        ui16txMsgDataSite1[2] =
                (CANB_tzSiteRegs.ui16Grid_Volt[ui16Actnode_ACDC_cnt] >> 8);
        ui16txMsgDataSite1[3] =
                (CANB_tzSiteRegs.ui16Grid_Volt[ui16Actnode_ACDC_cnt] & 0x00FF);

        ui16txMsgDataSite1[4] =
                (CANB_tzSiteRegs.ui16Grid_Freq[ui16Actnode_ACDC_cnt]>> 8);
        ui16txMsgDataSite1[5] =
                (CANB_tzSiteRegs.ui16Grid_Freq[ui16Actnode_ACDC_cnt] & 0x00FF);

        ui16txMsgDataSite1[6] =
                (CANB_tzSiteRegs.ui16Grid_Curr[ui16Actnode_ACDC_cnt]>> 8);
        ui16txMsgDataSite1[7] =
                (CANB_tzSiteRegs.ui16Grid_Curr[ui16Actnode_ACDC_cnt] & 0x00FF);

        CAN_sendMessage(CANB_BASE, CANB_mMAILBOX_1, CAN_mLEN8,
                        ui16txMsgDataSite1);

        break;

    case 3:

        CAN_setupMessageObject(CANB_BASE, CANB_mMAILBOX_1,
                               (CANB_mTX_mPSUACDCMSGID1),// | (ui16Actnode_ACDC_cnt)),
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                               0x1FFFFFFF,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN8);

        ui16txMsgDataSite1[0] = ui16Actnode_ACDC_cnt;
        ui16txMsgDataSite1[1] = 0x03;
        ui16txMsgDataSite1[2] =
                 CANA_tzPSURegs_RxACDC[ui16Actnode_ACDC_cnt].tzPSUDataACDC.ui16Grid_Power_watt >> 8;
         ui16txMsgDataSite1[3] =
                 (CANA_tzPSURegs_RxACDC[ui16Actnode_ACDC_cnt].tzPSUDataACDC.ui16Grid_Power_watt & 0x00FF);

         ui16txMsgDataSite1[4] =
                 (CANB_tzSiteRegs.ui16DCparam_VDC[ui16Actnode_ACDC_cnt]>> 8);
         ui16txMsgDataSite1[5] =
                 (CANB_tzSiteRegs.ui16DCparam_VDC[ui16Actnode_ACDC_cnt] & 0x00FF);

         ui16txMsgDataSite1[6] =
                 (CANB_tzSiteRegs.ui16DCparam_IDC[ui16Actnode_ACDC_cnt]>> 8);
         ui16txMsgDataSite1[7] =
                 (CANB_tzSiteRegs.ui16DCparam_IDC[ui16Actnode_ACDC_cnt] & 0x00FF);

        CAN_sendMessage(CANB_BASE, CANB_mMAILBOX_1, CAN_mLEN8,
                        ui16txMsgDataSite1);

        break;

    case 4:

        CAN_setupMessageObject(CANB_BASE, CANB_mMAILBOX_1,
                               (CANB_mTX_mPSUACDCMSGID1),// | (ui16Actnode_ACDC_cnt)),
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                               0x1FFFFFFF,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN8);

        ui16txMsgDataSite1[0] = ui16Actnode_ACDC_cnt;
        ui16txMsgDataSite1[1] = 0x04;
        ui16txMsgDataSite1[2] =
                  CANA_tzPSURegs_RxACDC[ui16Actnode_ACDC_cnt].tzPSUDataACDC.ui16DCparam_Power_watt >> 8;
          ui16txMsgDataSite1[3] =
                  (CANA_tzPSURegs_RxACDC[ui16Actnode_ACDC_cnt].tzPSUDataACDC.ui16DCparam_Power_watt & 0x00FF);

          ui16txMsgDataSite1[4] =
                  CANA_tzPSURegs_RxACDC[ui16Actnode_ACDC_cnt].tzPSUDataACDC.uiDCparam_status;
          ui16txMsgDataSite1[5] =
                  CANA_tzPSURegs_RxACDC[ui16Actnode_ACDC_cnt].tzPSUDataACDC.uiDCparam_version;

          ui16txMsgDataSite1[6] =
                  (CANA_tzPSURegs_RxACDC[ui16Actnode_ACDC_cnt].tzPSUDataACDC.uiTemp_P1);
          ui16txMsgDataSite1[7] =
                  (CANA_tzPSURegs_RxACDC[ui16Actnode_ACDC_cnt].tzPSUDataACDC.uiTemp_P2);

        CAN_sendMessage(CANB_BASE, CANB_mMAILBOX_1, CAN_mLEN8,
                        ui16txMsgDataSite1);

        break;
    case 5:

         CAN_setupMessageObject(CANB_BASE, CANB_mMAILBOX_1,
                                (CANB_mTX_mPSUACDCMSGID1),// | (ui16Actnode_ACDC_cnt)),
                                CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                                0x1FFFFFFF,
                                CAN_MSG_OBJ_NO_FLAGS,
                                CAN_mLEN8);

         ui16txMsgDataSite1[0] = ui16Actnode_ACDC_cnt;
         ui16txMsgDataSite1[1] = 0x05;
         ui16txMsgDataSite1[2] =
                 (CANA_tzPSURegs_RxACDC[ui16Actnode_ACDC_cnt].tzPSUDataACDC.uiTemp_P3);
           ui16txMsgDataSite1[3] =
                   (CANA_tzPSURegs_RxACDC[ui16Actnode_ACDC_cnt].tzPSUDataACDC.uiTemp_A);

           ui16txMsgDataSite1[4] =
                   CANA_tzPSURegs_RxACDC[ui16Actnode_ACDC_cnt].tzPSUDataACDC.uiDerating_fact;
           ui16txMsgDataSite1[5] =
                   CANA_tzACDC_HWFLT[ui16Actnode_ACDC_cnt].all;

           ui16txMsgDataSite1[6] =
                   CANA_tzACDC_SWLFLT[ui16Actnode_ACDC_cnt].all;
           ui16txMsgDataSite1[7] =
                   CANA_tzACDC_SWHFLT[ui16Actnode_ACDC_cnt].all;

         CAN_sendMessage(CANB_BASE, CANB_mMAILBOX_1, CAN_mLEN8,
                         ui16txMsgDataSite1);


         break;
    default:
           break;
       }
    /*********************************************************************************************/
    // DCDC-0 Power Supply messages

     ui16CANBTxCntDCDC0++;

     if (ui16CANBTxCntDCDC0 > 4)
     {
         ui16CANBTxCntDCDC0 = 1;
     }

     switch (ui16CANBTxCntDCDC0)
     {

    case 1:
        ui16Actnode_DCDC0_cnt++;  //can fail

        if (ui16Actnode_DCDC0_cnt > 9)
         {
            ui16Actnode_DCDC0_cnt = 1;
         }

         CAN_setupMessageObject(CANB_BASE, CANB_mMAILBOX_6,
                                (CANB_mTX_mPSUDCDC0MSGID1),// | (ui16Actnode_DCDC0_cnt)),
                                CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                                0x1FFFFFFF,
                                CAN_MSG_OBJ_NO_FLAGS,
                                CAN_mLEN8);

         ui16txMsgDataSite1[0] = ui16Actnode_DCDC0_cnt;
         ui16txMsgDataSite1[1] = 0x01;
         ui16txMsgDataSite1[2] =
                 CANB_tzSiteRegs.ui16VREF[ui16Actnode_DCDC0_cnt]>>8;
           ui16txMsgDataSite1[3] =
                   (CANB_tzSiteRegs.ui16VREF[ui16Actnode_DCDC0_cnt]& 0x00FF);

           ui16txMsgDataSite1[4] =
                   CANB_tzSiteRegs.ui16IREF[ui16Actnode_DCDC0_cnt]>>8;
           ui16txMsgDataSite1[5] =
                   (CANB_tzSiteRegs.ui16IREF[ui16Actnode_DCDC0_cnt]& 0x00FF);

           ui16txMsgDataSite1[6] =
                   CANB_tzSiteRegs.ui16VACT[ui16Actnode_DCDC0_cnt]>>8;
           ui16txMsgDataSite1[7] =
                   (CANB_tzSiteRegs.ui16VACT[ui16Actnode_DCDC0_cnt]& 0x00FF);

         CAN_sendMessage(CANB_BASE, CANB_mMAILBOX_6, CAN_mLEN8,
                         ui16txMsgDataSite1);

         break;
    case 2:

         CAN_setupMessageObject(CANB_BASE, CANB_mMAILBOX_6,
                                (CANB_mTX_mPSUDCDC0MSGID1),// | (ui16Actnode_DCDC0_cnt)),
                                CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                                0x1FFFFFFF,
                                CAN_MSG_OBJ_NO_FLAGS,
                                CAN_mLEN8);

         ui16txMsgDataSite1[0] = ui16Actnode_DCDC0_cnt;
         ui16txMsgDataSite1[1] = 0x02;
         ui16txMsgDataSite1[2] =
                 CANB_tzSiteRegs.ui16IACT[ui16Actnode_DCDC0_cnt]>>8;
           ui16txMsgDataSite1[3] =
                   (CANB_tzSiteRegs.ui16IACT[ui16Actnode_DCDC0_cnt]& 0x00FF);

           ui16txMsgDataSite1[4] =
                   CANB_tzSiteRegs.ui16VIN[ui16Actnode_DCDC0_cnt]>>8;
           ui16txMsgDataSite1[5] =
                   (CANB_tzSiteRegs.ui16VIN[ui16Actnode_DCDC0_cnt]& 0x00FF);

           ui16txMsgDataSite1[6] =
                   CANB_tzSiteRegs.ui16IIN[ui16Actnode_DCDC0_cnt]>>8;
           ui16txMsgDataSite1[7] =
                   (CANB_tzSiteRegs.ui16IIN[ui16Actnode_DCDC0_cnt]& 0x00FF);

         CAN_sendMessage(CANB_BASE, CANB_mMAILBOX_6, CAN_mLEN8,
                         ui16txMsgDataSite1);

         break;
    case 3:

             CAN_setupMessageObject(CANB_BASE, CANB_mMAILBOX_6,
                                    (CANB_mTX_mPSUDCDC0MSGID1),// | (ui16Actnode_DCDC0_cnt)),
                                    CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                                    0x1FFFFFFF,
                                    CAN_MSG_OBJ_NO_FLAGS,
                                    CAN_mLEN8);

             ui16txMsgDataSite1[0] = ui16Actnode_DCDC0_cnt;
             ui16txMsgDataSite1[1] = 0x03;
             ui16txMsgDataSite1[2] =
                     CANB_tzSiteRegs.ui16TEMP[ui16Actnode_DCDC0_cnt]>>8;
               ui16txMsgDataSite1[3] =
                       (CANB_tzSiteRegs.ui16TEMP[ui16Actnode_DCDC0_cnt]& 0x00FF);

               ui16txMsgDataSite1[4] =
                       CANA_tzPSURegs_RxDCDC0[ui16Actnode_DCDC0_cnt].tzPSUDataDCDC.uiDCparam_status;
               ui16txMsgDataSite1[5] =
                       CANA_tzPSURegs_RxDCDC0[ui16Actnode_DCDC0_cnt].tzPSUDataDCDC.uiDCparam_version;

               ui16txMsgDataSite1[6] =
                       CANB_tzSiteRegs.ui16CHA_Amps[ui16Actnode_DCDC0_cnt];
               ui16txMsgDataSite1[7] =
                       CANB_tzSiteRegs.ui16CHB_Amps[ui16Actnode_DCDC0_cnt];

             CAN_sendMessage(CANB_BASE, CANB_mMAILBOX_6, CAN_mLEN8,
                             ui16txMsgDataSite1);

             break;

    case 4:

             CAN_setupMessageObject(CANB_BASE, CANB_mMAILBOX_6,
                                    (CANB_mTX_mPSUDCDC0MSGID1),//| (ui16Actnode_DCDC0_cnt)),
                                    CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                                    0x1FFFFFFF,
                                    CAN_MSG_OBJ_NO_FLAGS,
                                    CAN_mLEN8);

             ui16txMsgDataSite1[0] = ui16Actnode_DCDC0_cnt;
             ui16txMsgDataSite1[1] = 0x04;
             ui16txMsgDataSite1[2] =
                     CANB_tzSiteRegs.ui16CHC_Amps[ui16Actnode_DCDC0_cnt];
               ui16txMsgDataSite1[3] =
                       CANB_tzSiteRegs.ui16Derating_Fact[ui16Actnode_DCDC0_cnt];

               ui16txMsgDataSite1[4] =
                       CANA_tzDCDC0_FLTOthH[ui16Actnode_DCDC0_cnt].all;
               ui16txMsgDataSite1[5] =
                       CANA_tzDCDC0_FLTOthL[ui16Actnode_DCDC0_cnt].all;

               ui16txMsgDataSite1[6] =
                       CANA_tzDCDC0_HWFLT[ui16Actnode_DCDC0_cnt].all;
               ui16txMsgDataSite1[7] =
                       CANA_tzDCDC0_SWFLT[ui16Actnode_DCDC0_cnt].all;

             CAN_sendMessage(CANB_BASE, CANB_mMAILBOX_6, CAN_mLEN8,
                             ui16txMsgDataSite1);

             break;
    default:
        break;
     }
/******************************** DCDC 1 PARAMETERS ***************************************/
     /*********************************************************************************************/
      // DCDC-0 Power Supply messages

       ui16CANBTxCntDCDC1++;

       if (ui16CANBTxCntDCDC1 > 4)
       {
           ui16CANBTxCntDCDC1 = 1;
       }

       switch (ui16CANBTxCntDCDC1)
       {

      case 1:
          ui16Actnode_DCDC1_cnt++;  //can fail

          if (ui16Actnode_DCDC1_cnt > 9)
           {
              ui16Actnode_DCDC1_cnt = 1;
           }

         CAN_setupMessageObject(CANB_BASE, CANB_mMAILBOX_7,
                                (CANB_mTX_mPSUDCDC1MSGID1),// | (ui16Actnode_DCDC1_cnt)),
                                CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                                0x1FFFFFFF,
                                CAN_MSG_OBJ_NO_FLAGS,
                                CAN_mLEN8);
         CANB_tzSiteRegs.ui16VREF_DCDC1[ui16Actnode_DCDC1_cnt] = 9921;
         ui16txMsgDataSite1[0] = ui16Actnode_DCDC1_cnt;
         ui16txMsgDataSite1[1] = 0x1;
         ui16txMsgDataSite1[2] =
                 CANB_tzSiteRegs.ui16VREF_DCDC1[ui16Actnode_DCDC1_cnt]>>8;
           ui16txMsgDataSite1[3] =
                   (CANB_tzSiteRegs.ui16VREF_DCDC1[ui16Actnode_DCDC1_cnt]& 0x00FF);

           ui16txMsgDataSite1[4] =
                   CANB_tzSiteRegs.ui16IREF_DCDC1[ui16Actnode_DCDC1_cnt]>>8;
           ui16txMsgDataSite1[5] =
                   (CANB_tzSiteRegs.ui16IREF_DCDC1[ui16Actnode_DCDC1_cnt]& 0x00FF);

           ui16txMsgDataSite1[6] =
                   CANB_tzSiteRegs.ui16VACT_DCDC1[ui16Actnode_DCDC1_cnt]>>8;
           ui16txMsgDataSite1[7] =
                   (CANB_tzSiteRegs.ui16VACT_DCDC1[ui16Actnode_DCDC1_cnt]& 0x00FF);

         CAN_sendMessage(CANB_BASE, CANB_mMAILBOX_7, CAN_mLEN8,
                         ui16txMsgDataSite1);

         break;
    case 2:

         CAN_setupMessageObject(CANB_BASE, CANB_mMAILBOX_7,
                                (CANB_mTX_mPSUDCDC1MSGID1),// | (ui16Actnode_DCDC1_cnt)),
                                CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                                0x1FFFFFFF,
                                CAN_MSG_OBJ_NO_FLAGS,
                                CAN_mLEN8);

         ui16txMsgDataSite1[0] = ui16Actnode_DCDC1_cnt;
         ui16txMsgDataSite1[1] = 0x02;
         ui16txMsgDataSite1[2] =
                 CANB_tzSiteRegs.ui16IACT_DCDC1[ui16Actnode_DCDC1_cnt]>>8;
           ui16txMsgDataSite1[3] =
                   (CANB_tzSiteRegs.ui16IACT_DCDC1[ui16Actnode_DCDC1_cnt]& 0x00FF);

           ui16txMsgDataSite1[4] =
                   CANB_tzSiteRegs.ui16VIN_DCDC1[ui16Actnode_DCDC1_cnt]>>8;
           ui16txMsgDataSite1[5] =
                   (CANB_tzSiteRegs.ui16VIN_DCDC1[ui16Actnode_DCDC1_cnt]& 0x00FF);

           ui16txMsgDataSite1[6] =
                   CANB_tzSiteRegs.ui16IIN_DCDC1[ui16Actnode_DCDC1_cnt]>>8;
           ui16txMsgDataSite1[7] =
                   (CANB_tzSiteRegs.ui16IIN_DCDC1[ui16Actnode_DCDC1_cnt]& 0x00FF);

         CAN_sendMessage(CANB_BASE, CANB_mMAILBOX_7, CAN_mLEN8,
                         ui16txMsgDataSite1);

         break;
    case 3:

             CAN_setupMessageObject(CANB_BASE, CANB_mMAILBOX_7,
                                    (CANB_mTX_mPSUDCDC1MSGID1),// | (ui16Actnode_DCDC1_cnt)),
                                    CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                                    0x1FFFFFFF,
                                    CAN_MSG_OBJ_NO_FLAGS,
                                    CAN_mLEN8);

             ui16txMsgDataSite1[0] = ui16Actnode_DCDC1_cnt;
             ui16txMsgDataSite1[1] = 0x03;
             ui16txMsgDataSite1[2] =
                     CANB_tzSiteRegs.ui16TEMP_DCDC1[ui16Actnode_DCDC1_cnt]>>8;
               ui16txMsgDataSite1[3] =
                       (CANB_tzSiteRegs.ui16TEMP_DCDC1[ui16Actnode_DCDC1_cnt]& 0x00FF);

               ui16txMsgDataSite1[4] =
                       CANA_tzPSURegs_RxDCDC1[ui16Actnode_DCDC1_cnt].tzPSUDataDCDC.uiDCparam_status;
               ui16txMsgDataSite1[5] =
                       CANA_tzPSURegs_RxDCDC1[ui16Actnode_DCDC1_cnt].tzPSUDataDCDC.uiDCparam_version;

               ui16txMsgDataSite1[6] =
                       CANB_tzSiteRegs.ui16CHA_Amps_DCDC1[ui16Actnode_DCDC1_cnt];
               ui16txMsgDataSite1[7] =
                       CANB_tzSiteRegs.ui16CHB_Amps_DCDC1[ui16Actnode_DCDC1_cnt];

             CAN_sendMessage(CANB_BASE, CANB_mMAILBOX_7, CAN_mLEN8,
                             ui16txMsgDataSite1);

             break;

    case 4:

             CAN_setupMessageObject(CANB_BASE, CANB_mMAILBOX_7,
                                    (CANB_mTX_mPSUDCDC1MSGID1),// | (ui16Actnode_DCDC1_cnt)),
                                    CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                                    0x1FFFFFFF,
                                    CAN_MSG_OBJ_NO_FLAGS,
                                    CAN_mLEN8);

             ui16txMsgDataSite1[0] = ui16Actnode_DCDC1_cnt;
             ui16txMsgDataSite1[1] = 0x4;
             ui16txMsgDataSite1[2] =
                     CANB_tzSiteRegs.ui16CHC_Amps_DCDC1[ui16Actnode_DCDC1_cnt];
               ui16txMsgDataSite1[3] =
                       CANB_tzSiteRegs.ui16Derating_Fact_DCDC1[ui16Actnode_DCDC1_cnt];

               ui16txMsgDataSite1[4] =
                       CANA_tzDCDC1_FLTOthH[ui16Actnode_DCDC1_cnt].all;
               ui16txMsgDataSite1[5] =
                       CANA_tzDCDC1_FLTOthL[ui16Actnode_DCDC1_cnt].all;

               ui16txMsgDataSite1[6] =
                       CANA_tzDCDC1_HWFLT[ui16Actnode_DCDC1_cnt].all;
               ui16txMsgDataSite1[7] =
                       CANA_tzDCDC1_SWFLT[ui16Actnode_DCDC1_cnt].all;

             CAN_sendMessage(CANB_BASE, CANB_mMAILBOX_7, CAN_mLEN8,
                             ui16txMsgDataSite1);

             break;


    default:
        break;
    }

    // Sensor Data from Master Controller

    ui16CANBTxCntSnsr++;

    if (ui16CANBTxCntSnsr > 14)
    {
        ui16CANBTxCntSnsr = 1;
    }

    switch (ui16CANBTxCntSnsr)
    {

    int32_t ui32temp;

case 1:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11943020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0x1;

    ui32temp = (MATHConvtzRegs.AISensorHYS101 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[3] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorHYS501 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[5] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorHYS401 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);

    break;

case 2:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11943020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0x2;

    ui32temp = (MATHConvtzRegs.AISensorHYS102 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[3] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorLVL101 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[5] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorOXS101 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);

    break;


case 3:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11943020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0x3;

    ui32temp = (MATHConvtzRegs.AISensorPRT101 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[3] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorPRT102 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[5] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorPRT401 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);
    break;

case 4:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11943020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0x4;

    ui32temp = (MATHConvtzRegs.AISensorPRT1 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[3] = (ui32temp & 0x00FF);

    ui32temp = (int32_t)(MATHConvtzRegs.AISensorDPT401 * 100);
    ui16txMsgDataSite2[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[5] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorTE401 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);
    break;

case 5:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11943020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0x5;
    ui16txMsgDataSite2[2] = (CANB_tzSiteRegs.WaterDemand);
    ui16txMsgDataSite2[3] = (CANB_tzSiteRegs.TurnONLCC);
    ui32temp = (MATHConvtzRegs.AISensorCOS101 * 100); // Added newly

     if (ui32temp < 0)
     {
         ui32temp = 0;
     }

     ui16txMsgDataSite2[4] = (ui32temp & 0xFF00) >> 8;
     ui16txMsgDataSite2[5] = (ui32temp & 0x00FF);
     ui16txMsgDataSite2[6] = STAT_tzStateMacMS.Present_st;
     ui16txMsgDataSite2[7] = STAT_tzStateMac.Present_st;

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);
    break;

case 6:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11943020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0x6;

    ui32temp = (MATHConvtzRegs.TempSensorTTC101 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[3] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.TempSensorTTC102 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[5] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.TempSensorTTC301 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);
    break;

case 7:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11943020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0x7;

    ui32temp = (MATHConvtzRegs.TempSensorKTC401 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[3] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorEBV801 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[5] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorPMP101 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);
    break;

case 8:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11943020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0x8;

    extern union H2_tzVALVES H2_tzValves;
    ui32temp = ((uint32_t)(CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].bit.DI_bit4) << 0) |
               ((uint32_t)(CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].bit.DI_bit5) << 1) |
               ((uint32_t)(CANA_tzLHCDI_IORegs[CANA_mLHC11_IO].bit.DI_bit3) << 2) |
               ((uint32_t)(CANA_tzLHCDI_IORegs[CANA_mLHC11_IO].bit.DI_bit4) << 3) |
               ((uint32_t)(CANA_tzLHCDI_IORegs[CANA_mLHC11_IO].bit.DI_bit5) << 4) |
               ((uint32_t)(CANA_tzLHCDI_IORegs[CANA_mLHC11_IO].bit.DI_bit6) << 5) |
               ((uint32_t)(CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC10_IO].bit.DO2) << 6) |
               ((uint32_t)(CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC10_IO].bit.DO3) << 7) |
               ((uint32_t)(H2_tzValves.bit.bt_Svalve1) << 8) |
               ((uint32_t)(H2_tzValves.bit.bt_Svalve2) << 9) |
               ((uint32_t)(H2_tzValves.bit.bt_Svalve3) << 10) |
               ((uint32_t)(H2_tzValves.bit.bt_Svalve4) << 11) |
               ((uint32_t)(H2_tzValves.bit.bt_Svalve5) << 12) |
               ((uint32_t)(H2_tzValves.bit.bt_Svalve6) << 13) |
               ((uint32_t)(H2_tzValves.bit.bt_Svalve7) << 14) |
               ((uint32_t)(CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC10_IO].bit.DO1) << 15);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }
    ui16txMsgDataSite2[2] = (ui32temp >> 8) & 0xFF;
    ui16txMsgDataSite2[3] = ui32temp & 0xFF;

    ui32temp = 0;
    if (ui32temp < 0)
    {
        ui32temp = 0;
    }
    ui16txMsgDataSite2[4] = H2_tzValves_PHS.all;
    ui16txMsgDataSite2[5] = ui32temp & 0xFF;

    ui32temp = 0;
    if (ui32temp < 0)
    {
        ui32temp = 0;
    }
    ui16txMsgDataSite2[6] = (ui32temp >> 8) & 0xFF;
    ui16txMsgDataSite2[7] = ui32temp & 0xFF;

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);
    break;

case 9:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11943020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0x9;

    ui32temp = CANA_tzDISensorData.PURGE101;

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[3] = (ui32temp & 0x00FF);

    ui32temp = CANA_tzDISensorData.PURGE102;

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[5] = (ui32temp & 0x00FF);

    ui32temp = CANA_tzDISensorData.PURGE501;

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);
    break;

case 10:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11943020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0xA;

    ui32temp = CANA_tzDISensorData.PURGE502;

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[3] = (ui32temp & 0x00FF);

    ui32temp = CANA_tzDISensorData.PURGE401;

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[5] = (ui32temp & 0x00FF);

    ui32temp = CANA_tzDISensorData.PURGE402;

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);
    break;
case 11:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11943020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0xB;

    ui32temp = (MATHConvtzRegs.AISensorPRT402 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[3] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorPRT403 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[5] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorPRT404 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);
    break;
case 12:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11943020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0xC;

    ui32temp = (MATHConvtzRegs.AISensorPRT405 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[3] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorPRT406 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[5] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorPRT407 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);
    break;
case 13:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11943020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0xD;

    ui32temp = (MATHConvtzRegs.AISensorRTD401 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[3] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorRTD402 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[5] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorRTD403 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);
    break;
case 14:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11943020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0xE;

    ui32temp = (MATHConvtzRegs.AISensorRTD404 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[3] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorMFM401 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[5] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorMFM402 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);
    break;
default:
    break;

    }

    // LHC and LPC faults

    ui16CANBTxCntFlts++;

    if (ui16CANBTxCntFlts > 4)
    {
        ui16CANBTxCntFlts = 1;
    }

    switch (ui16CANBTxCntFlts)
    {
    case 1:

        CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_3,
                               (0x11A43020 | (CANA_tzIORegs.uiUnitID << 8)),
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                               0x1FFFFFFF,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN8);

        ui16txMsgDataSite3[0] = 0x0;
        ui16txMsgDataSite3[1] = 0x1;
        ui16txMsgDataSite3[2] = CANA_tzLPCIO1_DIFaultRegs.all;
        ui16txMsgDataSite3[3] = CANA_tzLPCIO1_AIFaultRegs.all;
        //ui16txMsgDataSite3[4] = CANA_tzLPCIO1_AOFaultRegs.all ;
        //ui16txMsgDataSite3[5] = CANA_tzLPCIO1_DOFaultRegs.all ;
        ui16txMsgDataSite3[6] = CANA_tzActNodeRegs_IO.all;

        CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_3, CAN_mLEN8,
                        ui16txMsgDataSite3);

        break;

    case 2:

        CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_3,
                               (0x11A43020 | (CANA_tzIORegs.uiUnitID << 8)),
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                               0x1FFFFFFF,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN8);

        ui16txMsgDataSite3[0] = 0x0;
        ui16txMsgDataSite3[1] = 0x2;
        ui16txMsgDataSite3[2] = CANA_tzLPCIO2_DIFaultRegs.all;
        ui16txMsgDataSite3[3] = CANA_tzLPCIO2_AIFaultRegs.all;
        //ui16txMsgDataSite3[4] = CANA_tzLPCIO2_AOFaultRegs.all ;
        //ui16txMsgDataSite3[5] = CANA_tzLPCIO2_DOFaultRegs.all ;
        ui16txMsgDataSite3[6] = CONTROLtzFaultRegs.all;

        CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_3, CAN_mLEN8,
                        ui16txMsgDataSite3);

        break;

    case 3:

        CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_3,
                               (0x11A43020 | (CANA_tzIORegs.uiUnitID << 8)),
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                               0x1FFFFFFF,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN8);

        ui16txMsgDataSite3[0] = 0x0;
        ui16txMsgDataSite3[1] = 0x3;
        ui16txMsgDataSite3[2] = CANA_tzLHCIO1_DIFaultRegs.all;
        ui16txMsgDataSite3[3] = CANA_tzLHCIO1_AIFaultRegs.all;
        //ui16txMsgDataSite3[4] = CANA_tzLHCIO1_AOFaultRegs.all ;
        //ui16txMsgDataSite3[5] = CANA_tzLHCIO1_DOFaultRegs.all ;
        ui16txMsgDataSite3[6] = CANA_tzThermalFaultRegs.all;

        CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_3, CAN_mLEN8,
                        ui16txMsgDataSite3);

        break;

    case 4:

        CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_3,
                               (0x11A43020 | (CANA_tzIORegs.uiUnitID << 8)),
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                               0x1FFFFFFF,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN8);

        ui16txMsgDataSite3[0] = 0x0;
        ui16txMsgDataSite3[1] = 0x4;
        ui16txMsgDataSite3[2] = CANA_tzLHCIO2_DIFaultRegs.all;
        ui16txMsgDataSite3[3] = CANA_tzLHCIO2_AIFaultRegs.all;
        //ui16txMsgDataSite3[4] = CANA_tzLHCIO2_AOFaultRegs.all ;
        //ui16txMsgDataSite3[5] = CANA_tzLHCIO2_DOFaultRegs.all ;
        ui16txMsgDataSite3[6] = CANA_tzActMS_FaultRegs.all;

        CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_3, CAN_mLEN8,
                        ui16txMsgDataSite3);

        break;

    default:
        break;
    }

    // VSC data

    ui16CANBTxCntVSC++;

    if (ui16CANBTxCntVSC > 9)
    {
        ui16CANBTxCntVSC = 1;
    }

    switch (ui16CANBTxCntVSC)
    {
    int32_t ui32temp;

case 1:

    ui16nodeIDVS++;
    if (ui16nodeIDVS > 16)
    {
        ui16nodeIDVS = 0;
    }

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_4,
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite4[0] = ui16nodeIDVS;
    ui16txMsgDataSite4[1] = 0x1;

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[1] * 1000);

    ui16txMsgDataSite4[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[3] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[2] * 1000);

    ui16txMsgDataSite4[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[5] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[3] * 1000);

    ui16txMsgDataSite4[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_4, CAN_mLEN8, ui16txMsgDataSite4);

    break;

case 2:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_4,
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite4[0] = ui16nodeIDVS;
    ui16txMsgDataSite4[1] = 0x2;

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[4] * 1000);

    ui16txMsgDataSite4[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[3] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[5] * 1000);

    ui16txMsgDataSite4[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[5] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[6] * 1000);

    ui16txMsgDataSite4[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_4, CAN_mLEN8, ui16txMsgDataSite4);

    break;

case 3:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_4,
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite4[0] = ui16nodeIDVS;
    ui16txMsgDataSite4[1] = 0x3;

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[7] * 1000);

    ui16txMsgDataSite4[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[3] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[8] * 1000);

    ui16txMsgDataSite4[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[5] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[9] * 1000);

    ui16txMsgDataSite4[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_4, CAN_mLEN8, ui16txMsgDataSite4);

    break;

case 4:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_4,
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite4[0] = ui16nodeIDVS;
    ui16txMsgDataSite4[1] = 0x4;

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[10] * 1000);

    ui16txMsgDataSite4[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[3] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[11] * 1000);

    ui16txMsgDataSite4[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[5] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[12] * 1000);

    ui16txMsgDataSite4[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_4, CAN_mLEN8, ui16txMsgDataSite4);

    break;

case 5:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_4,
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite4[0] = ui16nodeIDVS;
    ui16txMsgDataSite4[1] = 0x5;

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[13] * 1000);

    ui16txMsgDataSite4[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[3] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[14] * 1000);

    ui16txMsgDataSite4[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[5] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[15] * 1000);

    ui16txMsgDataSite4[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_4, CAN_mLEN8, ui16txMsgDataSite4);

    break;

case 6:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_4,
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite4[0] = ui16nodeIDVS;
    ui16txMsgDataSite4[1] = 0x6;

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[16] * 1000);

    ui16txMsgDataSite4[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[3] = (ui32temp & 0x00FF);

    ui32temp = 0;

    ui16txMsgDataSite4[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[5] = (ui32temp & 0x00FF);

    ui32temp = 0;

    ui16txMsgDataSite4[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_4, CAN_mLEN8, ui16txMsgDataSite4);

    break;

case 7:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_4,
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite4[0] = ui16nodeIDVS;
    ui16txMsgDataSite4[1] = 0x7;

    ui16txMsgDataSite4[2] = (canA_tzVSC_info[ui16nodeIDVS].uiMaxcellNum[0]
            & 0xFF);

    ui16txMsgDataSite4[3] = (canA_tzVSC_info[ui16nodeIDVS].uiMaxcellNum[1]
            & 0xFF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32MaxCellVolt[0] * 1000);

    ui16txMsgDataSite4[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[5] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32MaxCellVolt[1] * 1000);

    ui16txMsgDataSite4[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[5] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_4, CAN_mLEN8, ui16txMsgDataSite4);

    break;

case 8:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_4,
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite4[0] = ui16nodeIDVS;
    ui16txMsgDataSite4[1] = 0x8;

    ui16txMsgDataSite4[2] = (canA_tzVSC_info[ui16nodeIDVS].uiMincellNum[0]
            & 0xFF);

    ui16txMsgDataSite4[3] = (canA_tzVSC_info[ui16nodeIDVS].uiMincellNum[1]
            & 0xFF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32MinCellVolt[0] * 1000);

    ui16txMsgDataSite4[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[5] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32MinCellVolt[1] * 1000);

    ui16txMsgDataSite4[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_4, CAN_mLEN8, ui16txMsgDataSite4);

    break;

case 9:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_4,
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite4[0] = 0;
    ui16txMsgDataSite4[1] = 0x9;

    extern float32_t f32maxCellVolt;
    ui32temp = (f32maxCellVolt * 1000);

    ui16txMsgDataSite4[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[3] = (ui32temp & 0x00FF);

    ui32temp = 0;

    ui16txMsgDataSite4[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[5] = (ui32temp & 0x00FF);

    ui32temp = 0;

    ui16txMsgDataSite4[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_4, CAN_mLEN8, ui16txMsgDataSite4);

    break;

default:
    break;
    }

        // Version data

        ui16CANBTxCntVer++;

        if (ui16CANBTxCntVer > 10)
        {
            ui16CANBTxCntVer = 1;
        }

        CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_12,
                                   (0x11F43020 | (CANA_tzIORegs.uiUnitID << 8)),
                                   CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                                   CAN_MSG_OBJ_NO_FLAGS,
                                   CAN_mLEN8);
        switch (ui16CANBTxCntVer)
        {
            case 1:
                ui16txMsgDataSite5[0] = (ui16CANBTxCntVer >> 8) & 0xFF;
                ui16txMsgDataSite5[1] = ui16CANBTxCntVer & 0xFF;
                ui16txMsgDataSite5[2] = MAJOR;
                ui16txMsgDataSite5[3] = MINOR;
                ui16txMsgDataSite5[4] = (BUILD >> 8) & 0xFF;
                ui16txMsgDataSite5[5] = BUILD & 0xFF;
                ui16txMsgDataSite5[6] = 0;
                ui16txMsgDataSite5[7] = 0;

                CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_12, CAN_mLEN8, ui16txMsgDataSite5);
                break;
        }

}

/*=============================================================================
 @Function name : void can_fnEnquedata(void)
 @brief  function to add data to queue

 @param void
 @return void
 ============================================================================ */

bool canb_fnEnquedata(canb_tzcirc_buff *ptr, uint16_t *data, uint32_t msgID,
                      uint16_t DLC)
{

    int16_t i_count;

    i_count = ptr->i_head + 1;

    if (i_count >= ptr->i_maxlen)
    {
        i_count = 0;
    }

    if (i_count == ptr->i_tail)
        return false;  // buffer is full

    memcpy(ptr->canB_tzRxC_buffer[ptr->i_head].uiDataBuffer, data, 8);
    ptr->canB_tzRxC_buffer[ptr->i_head].u32_msgID = msgID;
    ptr->canB_tzRxC_buffer[ptr->i_head].i_DataLength = (uint16_t) DLC;
    ptr->i_head = i_count;

    return true;
}

/*=============================================================================
 @Function name : void can_fndequedata(void)
 @brief  function to Delete data from queue

 @param void
 @return void
 ============================================================================ */
bool canb_fndequedata(canb_tzcirc_buff *ptr, uint16_t *data, uint32_t *msgID,
                      uint16_t *DLC)
{
    int16_t i_count;

    if (ptr->i_head == ptr->i_tail)
    {
        return false; // buffer is empty
    }

    i_count = ptr->i_tail + 1;

    if (i_count >= ptr->i_maxlen)
    {
        i_count = 0;
    }

    memcpy(data, ptr->canB_tzRxC_buffer[ptr->i_tail].uiDataBuffer, 8);
    *msgID = ptr->canB_tzRxC_buffer[ptr->i_tail].u32_msgID;
    *DLC = ptr->canB_tzRxC_buffer[ptr->i_tail].i_DataLength;
    ptr->i_tail = i_count;

    return true;

}

/*=============================================================================
 @Function name : void CAN_fnRXevent(void)
 @brief  function to receive messages over CAN

 @param void
 @return void
 ============================================================================ */
void CANB_fnRXevent(void)
{

    //any commands for AO/DO control
    if (CAN_IsMessageReceived(CANB_BASE, CAN_mMAILBOX_11))
    {
        CAN_readMessage(CANB_BASE, CAN_mMAILBOX_11, ui16rxMsgSP);

        canb_fnEnquedata(&uiRxbufferSP, ui16rxMsgSP, CanbRegs.CAN_IF2ARB.bit.ID,
                         CanbRegs.CAN_IF2MCTL.bit.DLC);

        CANB_tzSiteRegs.btMSComStart = 1;
        CANB_tzSiteRegs.SPComFailCnt = 0;

    }

}

/*=============================================================================
 @Function name : void CAN_fnRXevent(void)
 @brief  function to receive messages over CAN

 @param void
 @return void
 ============================================================================ */
void CANB_fnTask(void)
{
    uint32_t ui32temp;

    while (canb_fndequedata(&uiRxbufferSP, uirxPrcsMsgSP, &u32CANBmsgID1,
                            &uiCANBDataLength1))
    {

        ui32temp = (u32CANBmsgID1 & 0x00F00000);
        CANB_tzSiteRegs.uiMsgtype = (uint16_t) (ui32temp >> 20);

        canb_fnmsgPrcsSP(CANB_tzSiteRegs.uiMsgtype, uirxPrcsMsgSP);
    }

}

/*=============================================================================
 @Function name : void can_fnmsgProcess(void)
 @brief  function to Process Master Safety messages over CAN

 @param void
 @return void
 ============================================================================ */
static void canb_fnmsgPrcsSP(uint16_t uiMsgtype, uint16_t *msgDataSP)
{

    switch (uiMsgtype)
    {

    case 1:

//        if (CANB_tzSiteRegs.RxCntSP != msgDataSP[0])
    {
        CANB_tzSiteRegs.RxCntSP = msgDataSP[0];
//            CANB_tzSiteRegs.StartCmd = msgDataSP[1];
//            CANB_tzSiteRegs.PresentStMS = msgDataSP[2];
//            CANB_tzSiteRegs.AOCmd = msgDataSP[3];
//            CANB_tzSiteRegs.all = msgDataSP[4];
//            CANB_tzSiteRegs.AOVFan101 = msgDataSP[5];
//            CANB_tzSiteRegs.AOVFan501 = msgDataSP[6];
//            CANB_tzSiteRegs.AOVFan401 = msgDataSP[7];
        CANB_tzSiteRegs.btMSComStart = true;
        CANB_tzSiteRegs.SPComFailCnt = 0;

    }

        if ((msgDataSP[1] == 0x01) && (msgDataSP[2] == 0xAA)
                && (msgDataSP[3] == 0xAA))
        {
            CANB_tzSiteRegs.StartCmd = 0x01;
        }
        else if ((msgDataSP[1] == 0x01) && (msgDataSP[2] == 0x0)
                && (msgDataSP[3] == 0x0))
        {
            CANB_tzSiteRegs.StartCmd = 0x0;
        }

        if ((msgDataSP[1] == 0x02) && (msgDataSP[2] == 0xBB)
                && (msgDataSP[3] == 0xBB))
        {
            CANB_tzSiteRxRegs.Start_H2CmdStkpwr = 0x01;
        }
        else if ((msgDataSP[1] == 0x02) && (msgDataSP[2] == 0x0)
                && (msgDataSP[3] == 0x0))
        {
            CANB_tzSiteRxRegs.Start_H2CmdStkpwr = 0x0;
        }


/*********************************************************************************/
        if (msgDataSP[1] == 0x03)
         {
            CANB_tzSiteRxRegs.CellNotokTripCnt = ((msgDataSP[2] << 8)|(msgDataSP[3])) * 0.01;
             CANB_tzSiteRxRegs.CellMaxLimit =  ((msgDataSP[4] << 8)|(msgDataSP[5])) * 0.001;
             CANB_tzSiteRxRegs.CellMinLimit =  ((msgDataSP[6] << 8)|(msgDataSP[7])) * 0.001;
         }
/****************************************************************************************/
        CANB_tzSiteRegs.SPComFailCnt++;
        if (CANB_tzSiteRegs.SPComFailCnt >= 3000)
        {
            CANB_tzSiteRegs.btMSComStart = false;
            CANB_tzSiteRegs.SPComFailCnt = 3001;
        }

        break;

    case 2:

        if((msgDataSP[0] == 0x0) && (msgDataSP[1] == 0x10))

        CANB_tzSiteRegs.H2Percent = ((msgDataSP[2] << 8) | (msgDataSP[3]))
                * 0.01;

        CANB_tzSiteRegs.f32VoltSet = 400.0;

        CANB_tzSiteRegs.f32CurrSet =
                (CANB_tzSiteRegs.H2Percent * 0.01 * 1000.0);

        CANB_tzSiteRegs.numofCells = ((msgDataSP[4] << 8) | (msgDataSP[5]));

        CANB_tzSiteRegs.numofStack = ((msgDataSP[6] << 8) | (msgDataSP[7]));

        break;

    default:
        break;
    }
}

/*void canb_fnParamsUpdate()
{
    CANB_tzSiteRegs.ui16MeasVolt[CANA_PSURegs.Node] =
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[CANA_PSURegs.Node] * 100;
    CANB_tzSiteRegs.ui16MeasCurr[CANA_PSURegs.Node] =
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[CANA_PSURegs.Node] * 100;
    CANB_tzSiteRegs.ui16DCDCFlts[CANA_PSURegs.Node] =
            CANA_tzDCDCFaultRegs[CANA_PSURegs.Node].all;
    CANB_tzSiteRegs.ui16PFCHFlts[CANA_PSURegs.Node] =
            CANA_tzFECHFaultRegs[CANA_PSURegs.Node].all;
    CANB_tzSiteRegs.ui16PFCLFlts[CANA_PSURegs.Node] =
            CANA_tzFECLFaultRegs[CANA_PSURegs.Node].all;

    CANB_tzSiteRegs.ui16ACFreq[CANA_PSURegs.Node] =
            (CANA_tzRxdRegs.tzPSUData.ACparamFreq[CANA_PSURegs.Node] * 10);
    CANB_tzSiteRegs.ui16ACVolt[CANA_PSURegs.Node] =
            (CANA_tzRxdRegs.tzPSUData.AClineVolt[CANA_PSURegs.Node]);

    CANB_tzSiteRegs.ui16ACCurr[CANA_PSURegs.Node] =
            (CANA_tzRxdRegs.tzPSUData.AClineCurr[CANA_PSURegs.Node] * 10);

   /*****************************************************************************************/
//    CANB_tzSiteRegs.ui16Grid_Volt[canA_PSUACDCbuff.uiNodeID]=CANA_tzPSURegs_RxACDC[canA_PSUACDCbuff.uiNodeID].tzPSUDataACDC.f32Grid_Volt*10;
//    CANB_tzSiteRegs.ui16Grid_Freq[canA_PSUACDCbuff.uiNodeID]=CANA_tzPSURegs_RxACDC[canA_PSUACDCbuff.uiNodeID].tzPSUDataACDC.f32Grid_Freq*100;
//    CANB_tzSiteRegs.ui16Grid_Curr[canA_PSUACDCbuff.uiNodeID]=CANA_tzPSURegs_RxACDC[canA_PSUACDCbuff.uiNodeID].tzPSUDataACDC.f32Grid_Curr*10;
//    CANB_tzSiteRegs.ui16Grid_Curr[canA_PSUACDCbuff.uiNodeID]=CANA_tzPSURegs_RxACDC[canA_PSUACDCbuff.uiNodeID].tzPSUDataACDC.f32DCparam_VDC*10;
//    CANB_tzSiteRegs.ui16DCparam_VDC[canA_PSUACDCbuff.uiNodeID]=CANA_tzPSURegs_RxACDC[canA_PSUACDCbuff.uiNodeID].tzPSUDataACDC.f32DCparam_IDC*100;
//    CANB_tzSiteRegs.ui16DCparam_IDC[canA_PSUACDCbuff.uiNodeID]=CANA_tzPSURegs_RxACDC[canA_PSUACDCbuff.uiNodeID].tzPSUDataACDC.f32Grid_Curr*10;
//    CANB_tzSiteRegs.ui16VREF[canA_PSUDCDCbuff.uiNodeID]=CANA_tzPSURegs_RxDCDC0[canA_PSUDCDCbuff.uiNodeID].tzPSUDataDCDC.f32VREF*10;
//    CANB_tzSiteRegs.ui16IREF[canA_PSUDCDCbuff.uiNodeID]=CANA_tzPSURegs_RxDCDC0[canA_PSUDCDCbuff.uiNodeID].tzPSUDataDCDC.f32IREF*10;
//    CANB_tzSiteRegs.ui16VACT[canA_PSUDCDCbuff.uiNodeID]=CANA_tzPSURegs_RxDCDC0[canA_PSUDCDCbuff.uiNodeID].tzPSUDataDCDC.f32VACT*10;
//    CANB_tzSiteRegs.ui16IACT[canA_PSUDCDCbuff.uiNodeID]=CANA_tzPSURegs_RxDCDC0[canA_PSUDCDCbuff.uiNodeID].tzPSUDataDCDC.f32IACT*10; // to check txmsn
//    CANB_tzSiteRegs.ui16VIN[canA_PSUDCDCbuff.uiNodeID]=CANA_tzPSURegs_RxDCDC0[canA_PSUDCDCbuff.uiNodeID].tzPSUDataDCDC.f32VIN *10;
//    CANB_tzSiteRegs.ui16IIN[canA_PSUDCDCbuff.uiNodeID]=CANA_tzPSURegs_RxDCDC0[canA_PSUDCDCbuff.uiNodeID].tzPSUDataDCDC.f32IIN*10;
//    CANB_tzSiteRegs.ui16TEMP[canA_PSUDCDCbuff.uiNodeID] =CANA_tzPSURegs_RxDCDC0[canA_PSUDCDCbuff.uiNodeID].tzPSUDataDCDC.f32TEMP*100;
//    CANB_tzSiteRegs.ui16CHA_Amps[canA_PSUDCDCbuff.uiNodeID] =CANA_tzPSURegs_RxDCDC0[canA_PSUDCDCbuff.uiNodeID].tzPSUDataDCDC.f32CHA_Amps*10;
//    CANB_tzSiteRegs.ui16CHB_Amps[canA_PSUDCDCbuff.uiNodeID] =CANA_tzPSURegs_RxDCDC0[canA_PSUDCDCbuff.uiNodeID].tzPSUDataDCDC.f32CHB_Amps*10;
//    CANB_tzSiteRegs.ui16CHB_Amps[canA_PSUDCDCbuff.uiNodeID] =CANA_tzPSURegs_RxDCDC0[canA_PSUDCDCbuff.uiNodeID].tzPSUDataDCDC.f32CHC_Amps*10;
//
///*********************************************************************************************/
//    CANB_tzSiteRegs.ui16VREF_DCDC1[canA_PSUDCDCbuff.uiNodeID]=CANA_tzPSURegs_RxDCDC1[canA_PSUDCDCbuff.uiNodeID].tzPSUDataDCDC.f32VREF*10;
//    CANB_tzSiteRegs.ui16IREF_DCDC1[canA_PSUDCDCbuff.uiNodeID]=CANA_tzPSURegs_RxDCDC1[canA_PSUDCDCbuff.uiNodeID].tzPSUDataDCDC.f32IREF*10;
//    CANB_tzSiteRegs.ui16VACT_DCDC1[canA_PSUDCDCbuff.uiNodeID]=CANA_tzPSURegs_RxDCDC1[canA_PSUDCDCbuff.uiNodeID].tzPSUDataDCDC.f32VACT*10;
//    CANB_tzSiteRegs.ui16IACT_DCDC1[canA_PSUDCDCbuff.uiNodeID]=CANA_tzPSURegs_RxDCDC1[canA_PSUDCDCbuff.uiNodeID].tzPSUDataDCDC.f32IACT*10;
//    CANB_tzSiteRegs.ui16VIN_DCDC1[canA_PSUDCDCbuff.uiNodeID]=CANA_tzPSURegs_RxDCDC1[canA_PSUDCDCbuff.uiNodeID].tzPSUDataDCDC.f32VIN *10;
//    CANB_tzSiteRegs.ui16IIN_DCDC1[canA_PSUDCDCbuff.uiNodeID]=CANA_tzPSURegs_RxDCDC1[canA_PSUDCDCbuff.uiNodeID].tzPSUDataDCDC.f32IIN*10;
//    CANB_tzSiteRegs.ui16TEMP_DCDC1[canA_PSUDCDCbuff.uiNodeID] =CANA_tzPSURegs_RxDCDC1[canA_PSUDCDCbuff.uiNodeID].tzPSUDataDCDC.f32TEMP*100;
//    CANB_tzSiteRegs.ui16CHA_Amps_DCDC1[canA_PSUDCDCbuff.uiNodeID] =CANA_tzPSURegs_RxDCDC1[canA_PSUDCDCbuff.uiNodeID].tzPSUDataDCDC.f32CHA_Amps*10;
//    CANB_tzSiteRegs.ui16CHB_Amps_DCDC1[canA_PSUDCDCbuff.uiNodeID] =CANA_tzPSURegs_RxDCDC1[canA_PSUDCDCbuff.uiNodeID].tzPSUDataDCDC.f32CHB_Amps*10;
//    CANB_tzSiteRegs.ui16CHB_Amps_DCDC1[canA_PSUDCDCbuff.uiNodeID] =CANA_tzPSURegs_RxDCDC1[canA_PSUDCDCbuff.uiNodeID].tzPSUDataDCDC.f32CHC_Amps*10;
//
//
//}*/
void canb_fnParamsUpdate()
{
    static uint16_t cnt_acdc = 0;
    static uint16_t cnt_dcdc = 0;

    CANB_tzSiteRegs.ui16MeasVolt[CANA_PSURegs.Node] =
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[CANA_PSURegs.Node] * 100;
    CANB_tzSiteRegs.ui16MeasCurr[CANA_PSURegs.Node] =
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[CANA_PSURegs.Node] * 100;
    CANB_tzSiteRegs.ui16DCDCFlts[CANA_PSURegs.Node] =
            CANA_tzDCDCFaultRegs[CANA_PSURegs.Node].all;
    CANB_tzSiteRegs.ui16PFCHFlts[CANA_PSURegs.Node] =
            CANA_tzFECHFaultRegs[CANA_PSURegs.Node].all;
    CANB_tzSiteRegs.ui16PFCLFlts[CANA_PSURegs.Node] =
            CANA_tzFECLFaultRegs[CANA_PSURegs.Node].all;

    CANB_tzSiteRegs.ui16ACFreq[CANA_PSURegs.Node] =
            (CANA_tzRxdRegs.tzPSUData.ACparamFreq[CANA_PSURegs.Node] * 10);
    CANB_tzSiteRegs.ui16ACVolt[CANA_PSURegs.Node] =
            (CANA_tzRxdRegs.tzPSUData.AClineVolt[CANA_PSURegs.Node]);

    CANB_tzSiteRegs.ui16ACCurr[CANA_PSURegs.Node] =
            (CANA_tzRxdRegs.tzPSUData.AClineCurr[CANA_PSURegs.Node] * 10);

   /*****************************************************************************************/
    ++cnt_acdc;
    if (cnt_acdc >= CANA_mTOT_ACDCPSUNODE+1)
        cnt_acdc = 0;

    ++cnt_dcdc;
        if (cnt_dcdc >= CANA_mTOT_DCDCPSUNODE+1)
            cnt_dcdc = 0;

        CANB_tzSiteRegs.ui16Grid_Volt[cnt_acdc]=CANA_tzPSURegs_RxACDC[cnt_acdc].tzPSUDataACDC.f32Grid_Volt*10;
        CANB_tzSiteRegs.ui16Grid_Freq[cnt_acdc]=CANA_tzPSURegs_RxACDC[cnt_acdc].tzPSUDataACDC.f32Grid_Freq*100;
        CANB_tzSiteRegs.ui16Grid_Curr[cnt_acdc]=CANA_tzPSURegs_RxACDC[cnt_acdc].tzPSUDataACDC.f32Grid_Curr*10;
        CANB_tzSiteRegs.ui16Grid_Curr[cnt_acdc]=CANA_tzPSURegs_RxACDC[cnt_acdc].tzPSUDataACDC.f32DCparam_VDC*10;
        CANB_tzSiteRegs.ui16DCparam_VDC[cnt_acdc]=CANA_tzPSURegs_RxACDC[cnt_acdc].tzPSUDataACDC.f32DCparam_IDC*100;
        CANB_tzSiteRegs.ui16DCparam_IDC[cnt_acdc]=CANA_tzPSURegs_RxACDC[cnt_acdc].tzPSUDataACDC.f32Grid_Curr*10;

    /*********************************************************************************************/
        CANB_tzSiteRegs.ui16VREF[cnt_dcdc]=CANA_tzPSURegs_RxDCDC0[cnt_dcdc].tzPSUDataDCDC.f32VREF*10;
        CANB_tzSiteRegs.ui16IREF[cnt_dcdc]=CANA_tzPSURegs_RxDCDC0[cnt_dcdc].tzPSUDataDCDC.f32IREF*10;
        CANB_tzSiteRegs.ui16VACT[cnt_dcdc]=CANA_tzPSURegs_RxDCDC0[cnt_dcdc].tzPSUDataDCDC.f32VACT*10;
        CANB_tzSiteRegs.ui16IACT[cnt_dcdc]=CANA_tzPSURegs_RxDCDC0[cnt_dcdc].tzPSUDataDCDC.f32IACT*10; // to check txmsn
        CANB_tzSiteRegs.ui16VIN[cnt_dcdc]=CANA_tzPSURegs_RxDCDC0[cnt_dcdc].tzPSUDataDCDC.f32VIN *10;
        CANB_tzSiteRegs.ui16IIN[cnt_dcdc]=CANA_tzPSURegs_RxDCDC0[cnt_dcdc].tzPSUDataDCDC.f32IIN*10;
        CANB_tzSiteRegs.ui16TEMP[cnt_dcdc] =CANA_tzPSURegs_RxDCDC0[cnt_dcdc].tzPSUDataDCDC.f32TEMP*100;
        CANB_tzSiteRegs.ui16CHA_Amps[cnt_dcdc] =CANA_tzPSURegs_RxDCDC0[cnt_dcdc].tzPSUDataDCDC.f32CHA_Amps*10;
        CANB_tzSiteRegs.ui16CHB_Amps[cnt_dcdc] =CANA_tzPSURegs_RxDCDC0[cnt_dcdc].tzPSUDataDCDC.f32CHB_Amps*10;
        CANB_tzSiteRegs.ui16CHB_Amps[cnt_dcdc] =CANA_tzPSURegs_RxDCDC0[cnt_dcdc].tzPSUDataDCDC.f32CHC_Amps*10;

    /*********************************************************************************************/
        CANB_tzSiteRegs.ui16VREF_DCDC1[cnt_dcdc]=CANA_tzPSURegs_RxDCDC1[cnt_dcdc].tzPSUDataDCDC.f32VREF*10;
        CANB_tzSiteRegs.ui16IREF_DCDC1[cnt_dcdc]=CANA_tzPSURegs_RxDCDC1[cnt_dcdc].tzPSUDataDCDC.f32IREF*10;
        CANB_tzSiteRegs.ui16VACT_DCDC1[cnt_dcdc]=CANA_tzPSURegs_RxDCDC1[cnt_dcdc].tzPSUDataDCDC.f32VACT*10;
        CANB_tzSiteRegs.ui16IACT_DCDC1[cnt_dcdc]=CANA_tzPSURegs_RxDCDC1[cnt_dcdc].tzPSUDataDCDC.f32IACT*10;
        CANB_tzSiteRegs.ui16VIN_DCDC1[cnt_dcdc]=CANA_tzPSURegs_RxDCDC1[cnt_dcdc].tzPSUDataDCDC.f32VIN *10;
        CANB_tzSiteRegs.ui16IIN_DCDC1[cnt_dcdc]=CANA_tzPSURegs_RxDCDC1[cnt_dcdc].tzPSUDataDCDC.f32IIN*10;
        CANB_tzSiteRegs.ui16TEMP_DCDC1[cnt_dcdc] =CANA_tzPSURegs_RxDCDC1[cnt_dcdc].tzPSUDataDCDC.f32TEMP*100;
        CANB_tzSiteRegs.ui16CHA_Amps_DCDC1[cnt_dcdc] =CANA_tzPSURegs_RxDCDC1[cnt_dcdc].tzPSUDataDCDC.f32CHA_Amps*10;
        CANB_tzSiteRegs.ui16CHB_Amps_DCDC1[cnt_dcdc] =CANA_tzPSURegs_RxDCDC1[cnt_dcdc].tzPSUDataDCDC.f32CHB_Amps*10;
        CANB_tzSiteRegs.ui16CHB_Amps_DCDC1[cnt_dcdc] =CANA_tzPSURegs_RxDCDC1[cnt_dcdc].tzPSUDataDCDC.f32CHC_Amps*10;


}

/*==============================================================================
 Local Constants
 ==============================================================================*/

/*==============================================================================
 End of File
 ==============================================================================*/

