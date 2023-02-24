/*=============================================================================
Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
All trademarks are owned by Enarka India Private Limited
============================================================================ */

/*==============================================================================
 @file  cana_vsc.c
 @author OHMLAP0107   
 @date 14-Jul-2022

 @brief Description
==============================================================================*/

/*==============================================================================
 Includes
==============================================================================*/

#include "F28x_Project.h"
#include <stdint.h>
#include "inc/hw_can.h"
#include "can.h"

#include "cana_vsc.h"
#include "cana_defs.h"
#include "canb_defs.h"

/*==============================================================================
 Defines
==============================================================================*/

/*==============================================================================
 Enums
==============================================================================*/

/*==============================================================================
 Structures
==============================================================================*/
CANA_tzVSC canA_VSCbuff;
CANA_tzVSC_CELL_INFO canA_tzVSC_info[17];
uint16_t ui16CANVSTX_cnt=0;
uint16_t ui16IsTxCellCount=0;
/*==============================================================================
 Macros
==============================================================================*/

/*==============================================================================
 Local Function Prototypes
==============================================================================*/
static void VSC_fntxCellcnt(uint16_t uiNodeid, uint16_t uiCellcount);
void cana_fnVSCTx(void);


/*==============================================================================
 Local Variables
==============================================================================*/
uint16_t uiMaxCards_VSC = 0;
uint16_t uiMaxCellsLastCard_VSC = 0;
uint16_t uiCellCount = 0,uiNoOfStackSets = 0;
/*==============================================================================
 Local Constants
==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
//TODO: to be called in stack check
void cana_fnVSCTx(void)
{
    //TODO: uicellcount to be read from CAN-B
    static uint16_t cnt = 0;
    ui16CANVSTX_cnt++;

    if (ui16CANVSTX_cnt > 100)
    {
        ui16CANVSTX_cnt = 0;
        if (ui16IsTxCellCount)
        {
            switch (cnt)
            {
                case 1:
                    VSC_fntxCellcnt(0, 8);
                    break;

                case 2:
                    VSC_fntxCellcnt(4, 8);
                    break;

                case 3:
                    VSC_fntxCellcnt(8, 8);
                    break;

                case 4:
                    VSC_fntxCellcnt(12, 8);
                    break;

                default:
                    cnt = 0;
                    break;

            }
        }
        cnt++;
#if 0
        uiMaxCards_VSC = CANB_tzSiteRegs.numofCells >> 4;
        uiMaxCellsLastCard_VSC = CANB_tzSiteRegs.numofCells & 0xF;

                if (uiMaxCellsLastCard_VSC)
                {
                    VSC_fntxCellcnt(uiMaxCards_VSC + 1, uiMaxCellsLastCard_VSC);
                    uiMaxCards_VSC = uiMaxCards_VSC + 1;
                }
#endif
    }

}
/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
static void VSC_fntxCellcnt(uint16_t uiNodeid, uint16_t uiCellcount)
{
    uint16_t uiVSCtxbuf[8] = { 0 };

    CAN_setupMessageObject(CANA_BASE, CAN_mMAILBOX_20,
                           CANA_mTX_VSC_CELL_CNT | uiNodeid, CAN_MSG_FRAME_EXT,
                           CAN_MSG_OBJ_TYPE_TX, 0, CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    uiVSCtxbuf[0] = 0x22;
    uiVSCtxbuf[1] = 0x00;
    uiVSCtxbuf[2] = 0x00;
    uiVSCtxbuf[3] = 0x00;
    uiVSCtxbuf[4] = 0x00;
    uiVSCtxbuf[5] = uiCellcount;//uiCellcount;//CANB_tzSiteRegs.numofCells & 0xFF;
    uiVSCtxbuf[6] = 0x00;
    uiVSCtxbuf[7] = 0x00;

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_20, CAN_mLEN8, uiVSCtxbuf);

}

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
void cana_fnmsgPrcsVSC(uint16_t nodeID, uint16_t msgtype, uint16_t *msgbuf)
{
    uint16_t ui16temp;
    uint16_t uiCellNum = 0;
    uint32_t u32temp = 0x0000FFFF;
    uint32_t u32temp1 = 0x0000FFFF;

    // to receive data from active VSC only
    uiMaxCards_VSC = 16;
    if (nodeID <= (uiMaxCards_VSC))
    {
        canA_tzVSC_info[nodeID].isReceived = 1;
        switch (msgtype)
        {
        case 1:
        {
            if (nodeID == 4 || nodeID == 8)
                for (ui16temp = 0; ui16temp < 8; ui16temp++)
                {
                    msgbuf[ui16temp] = 0;
                }
            canA_tzVSC_info[nodeID].f32Cellvolt[1] = (float32_t) (msgbuf[0] << 8
                    | msgbuf[1] << 0) * 0.001;
            canA_tzVSC_info[nodeID].f32Cellvolt[2] = (float32_t) (msgbuf[2] << 8
                    | msgbuf[3] << 0) * 0.001;
            canA_tzVSC_info[nodeID].f32Cellvolt[3] = (float32_t) (msgbuf[4] << 8
                    | msgbuf[5] << 0) * 0.001;
            canA_tzVSC_info[nodeID].f32Cellvolt[4] = (float32_t) (msgbuf[6] << 8
                    | msgbuf[7] << 0) * 0.001;

        }
            break;

        case 2:
        {
            if (nodeID == 4 || nodeID == 8)
                for (ui16temp = 0; ui16temp < 8; ui16temp++)
                {
                    msgbuf[ui16temp] = 0;
                }
            canA_tzVSC_info[nodeID].f32Cellvolt[5] = (float32_t) (msgbuf[0] << 8
                    | msgbuf[1] << 0) * 0.001;
            canA_tzVSC_info[nodeID].f32Cellvolt[6] = (float32_t) (msgbuf[2] << 8
                    | msgbuf[3] << 0) * 0.001;
            canA_tzVSC_info[nodeID].f32Cellvolt[7] = (float32_t) (msgbuf[4] << 8
                    | msgbuf[5] << 0) * 0.001;
            canA_tzVSC_info[nodeID].f32Cellvolt[8] = (float32_t) (msgbuf[6] << 8
                    | msgbuf[7] << 0) * 0.001;
            for (ui16temp = 0; ui16temp < 8; ui16temp++)
                                    {
                msgbuf[ui16temp] = 0;
                                    }
        }
            break;

        case 3:
        {
            if (nodeID == 4 || nodeID == 8)
                for (ui16temp = 0; ui16temp < 8; ui16temp++)
                {
                    msgbuf[ui16temp] = 0;
                }
            canA_tzVSC_info[nodeID].f32Cellvolt[9] = (float32_t) (msgbuf[0] << 8
                    | msgbuf[1] << 0) * 0.001;
            canA_tzVSC_info[nodeID].f32Cellvolt[10] = (float32_t) (msgbuf[2]
                    << 8 | msgbuf[3] << 0) * 0.001;
            canA_tzVSC_info[nodeID].f32Cellvolt[11] = (float32_t) (msgbuf[4]
                    << 8 | msgbuf[5] << 0) * 0.001;
            canA_tzVSC_info[nodeID].f32Cellvolt[12] = (float32_t) (msgbuf[6]
                    << 8 | msgbuf[7] << 0) * 0.001;
            canA_tzVSC_info[0].f32Cellvolt[9] = 0.0;
            canA_tzVSC_info[0].f32Cellvolt[10] = 0.0;
            canA_tzVSC_info[0].f32Cellvolt[11] = 0.0;
            canA_tzVSC_info[0].f32Cellvolt[12] = 0.0;
            canA_tzVSC_info[4].f32Cellvolt[9] = 0.0;
            canA_tzVSC_info[4].f32Cellvolt[10] = 0.0;
            canA_tzVSC_info[4].f32Cellvolt[11] = 0.0;
            canA_tzVSC_info[4].f32Cellvolt[12] = 0.0;
            canA_tzVSC_info[8].f32Cellvolt[9] = 0.0;
             canA_tzVSC_info[8].f32Cellvolt[10] = 0.0;
             canA_tzVSC_info[8].f32Cellvolt[11] = 0.0;
             canA_tzVSC_info[8].f32Cellvolt[12] = 0.0;
             canA_tzVSC_info[12].f32Cellvolt[9] = 0.0;
             canA_tzVSC_info[12].f32Cellvolt[10] = 0.0;
             canA_tzVSC_info[12].f32Cellvolt[11] = 0.0;
             canA_tzVSC_info[12].f32Cellvolt[12] = 0.0;

             for (ui16temp = 0; ui16temp < 8; ui16temp++)
                                     {
                                         msgbuf[ui16temp] = 0;
                                     }


        }
            break;

        case 4:
        {
            if (nodeID == 4 || nodeID == 8)
                for (ui16temp = 0; ui16temp < 8; ui16temp++)
                {
                    msgbuf[ui16temp] = 0;
                }
            canA_tzVSC_info[nodeID].f32Cellvolt[13] = (float32_t) (msgbuf[0]
                    << 8 | msgbuf[1] << 0) * 0.001;
            canA_tzVSC_info[nodeID].f32Cellvolt[14] = (float32_t) (msgbuf[2]
                    << 8 | msgbuf[3] << 0) * 0.001;
            canA_tzVSC_info[nodeID].f32Cellvolt[15] = (float32_t) (msgbuf[4]
                    << 8 | msgbuf[5] << 0) * 0.001;
            canA_tzVSC_info[nodeID].f32Cellvolt[16] = (float32_t) (msgbuf[6]
                    << 8 | msgbuf[7] << 0) * 0.001;

            canA_tzVSC_info[0].f32Cellvolt[13] = 0.0;
            canA_tzVSC_info[0].f32Cellvolt[14] = 0.0;
            canA_tzVSC_info[0].f32Cellvolt[15] = 0.0;
            canA_tzVSC_info[0].f32Cellvolt[16] = 0.0;

            canA_tzVSC_info[4].f32Cellvolt[13] = 0.0;
            canA_tzVSC_info[4].f32Cellvolt[14] = 0.0;
            canA_tzVSC_info[4].f32Cellvolt[15] = 0.0;
            canA_tzVSC_info[4].f32Cellvolt[16] = 0.0;

            canA_tzVSC_info[8].f32Cellvolt[13] = 0.0;
            canA_tzVSC_info[8].f32Cellvolt[14] = 0.0;
            canA_tzVSC_info[8].f32Cellvolt[15] = 0.0;
            canA_tzVSC_info[8].f32Cellvolt[16] = 0.0;

            canA_tzVSC_info[12].f32Cellvolt[13] = 0.0;
            canA_tzVSC_info[12].f32Cellvolt[14] = 0.0;
            canA_tzVSC_info[12].f32Cellvolt[15] = 0.0;
            canA_tzVSC_info[12].f32Cellvolt[16] = 0.0;

            for (ui16temp = 0; ui16temp < 8; ui16temp++)
                                     {
                                         msgbuf[ui16temp] = 0;
                                     }

        }
            break;

        case 5:
        {
            if (nodeID == 4 || nodeID == 8) // Added by Jayanth Sir.
                for (ui16temp = 0; ui16temp < 8; ui16temp++)
                {
                    msgbuf[ui16temp] = 0;
                }
            canA_tzVSC_info[nodeID].f32MaxCellVolt[0] = (float32_t) (msgbuf[0]
                    << 8 | msgbuf[1] << 0) * 0.001;
            canA_tzVSC_info[nodeID].uiMaxcellNum[0] = msgbuf[2] & 0xFF;
            canA_tzVSC_info[nodeID].f32MaxCellVolt[1] = (float32_t) (msgbuf[3]
                    << 8 | msgbuf[4] << 0) * 0.001;
            canA_tzVSC_info[nodeID].uiMaxcellNum[1] = msgbuf[5] & 0xFF;
            for (ui16temp = 0; ui16temp < 8; ui16temp++)
                                    {
                                        msgbuf[ui16temp] = 0;
                                    }
        }
            break;

        case 6:
        {
            if (nodeID == 4 || nodeID == 8)
                for (ui16temp = 0; ui16temp < 8; ui16temp++)
                {
                    msgbuf[ui16temp] = 0;
                }
            canA_tzVSC_info[nodeID].f32MinCellVolt[0] = (float32_t) (msgbuf[0]
                    << 8 | msgbuf[1] << 0) * 0.001;
            canA_tzVSC_info[nodeID].uiMincellNum[0] = msgbuf[2] & 0xFF;
            canA_tzVSC_info[nodeID].f32MinCellVolt[1] = (float32_t) (msgbuf[3]
                    << 8 | msgbuf[4] << 0) * 0.001;
            canA_tzVSC_info[nodeID].uiMincellNum[1] = msgbuf[5] & 0xFF;
            for (ui16temp = 0; ui16temp < 8; ui16temp++)
                                    {
                                        msgbuf[ui16temp] = 0;
                                    }
        }
            break;

        case 7:
        {
            // impedance values
            uiCellNum = msgbuf[0];

            u32temp = (u32temp & msgbuf[1]) << 16;

            canA_tzVSC_info[nodeID].f32Z_Real[uiCellNum] = (int32_t) (u32temp
                    | (int32_t) (((msgbuf[2] << 8) | ((msgbuf[3]) & 0x00FF))
                            & 0x0000FFFF)) * 0.0001;

            u32temp1 = (u32temp1 & msgbuf[4]) << 16;

            canA_tzVSC_info[nodeID].f32Z_Img[uiCellNum] = (int32_t) (u32temp1
                    | (int32_t) (((msgbuf[5] << 8) | ((msgbuf[6]) & 0x00FF))
                            & 0x0000FFFF)) * 0.0001;
            for (ui16temp = 0; ui16temp < 8; ui16temp++)
                                    {
                                        msgbuf[ui16temp] = 0;
                                    }
        }
            break;
        default:
            break;
        }


//            canA_tzVSC_info[0].f32Cellvolt[0][9] = 0.0;
//            canA_tzVSC_info[0].f32Cellvolt[0][10] = 0.0;
//            canA_tzVSC_info[0].f32Cellvolt[0][11] = 0.0;
//            canA_tzVSC_info[0].f32Cellvolt[0][12] = 0.0;
//            canA_tzVSC_info[0].f32Cellvolt[0][13] = 0.0;
//            canA_tzVSC_info[0].f32Cellvolt[0][14] = 0.0;
//            canA_tzVSC_info[0].f32Cellvolt[0][15] = 0.0;
//            canA_tzVSC_info[0].f32Cellvolt[0][16] = 0.0;


    }





//     if(CANA_tzVSRegs.Node!= 2)
//     {
//         if(CANA_tzTimerRegs.tzVS.CANfailCnt[2]++ >= 200)
//         {
//             CANA_tzTimerRegs.tzVS.CANfailCnt[2] = 200;
//             for(ui16temp = 1;ui16temp<=16;ui16temp++)
//             {
//                 CANA_tzRxdRegs.tzVSData.CellVolt[2][ui16temp] = 0;
//             }
//         }
//     }
//     else
//     {
//         CANA_tzTimerRegs.tzVS.CANfailCnt[2] = 0;
//     }
//
//
//     if(CANA_tzVSRegs.Node!= 3)
//     {
//         if(CANA_tzTimerRegs.tzVS.CANfailCnt[3]++ >= 200)
//         {
//             CANA_tzTimerRegs.tzVS.CANfailCnt[3] = 200;
//             for(ui16temp = 1;ui16temp<=16;ui16temp++)
//             {
//                 CANA_tzRxdRegs.tzVSData.CellVolt[3][ui16temp] = 0;
//             }
//         }
//     }
//     else
//     {
//         CANA_tzTimerRegs.tzVS.CANfailCnt[3] = 0;
//     }
//
//
//
//
//     if(CANA_tzVSRegs.Node!= 4)
//     {
//         if(CANA_tzTimerRegs.tzVS.CANfailCnt[4]++ >= 200)
//         {
//             CANA_tzTimerRegs.tzVS.CANfailCnt[4] = 200;
//             for(ui16temp = 1;ui16temp<=16;ui16temp++)
//             {
//                 CANA_tzRxdRegs.tzVSData.CellVolt[4][ui16temp] = 0;
//             }
//         }
//     }
//     else
//     {
//         CANA_tzTimerRegs.tzVS.CANfailCnt[4] = 0;
//     }
//
//
//     if(CANA_tzVSRegs.Node!= 5)
//     {
//         if(CANA_tzTimerRegs.tzVS.CANfailCnt[5]++ >= 200)
//         {
//             CANA_tzTimerRegs.tzVS.CANfailCnt[5] = 200;
//             for(ui16temp = 1;ui16temp<=16;ui16temp++)
//             {
//                 CANA_tzRxdRegs.tzVSData.CellVolt[5][ui16temp] = 0;
//             }
//         }
//     }
//     else
//     {
//         CANA_tzTimerRegs.tzVS.CANfailCnt[5] = 0;
//     }
//
//
//     if(CANA_tzVSRegs.Node!= 6)
//     {
//         if(CANA_tzTimerRegs.tzVS.CANfailCnt[6]++ >= 200)
//         {
//             CANA_tzTimerRegs.tzVS.CANfailCnt[6] = 200;
//             for(ui16temp = 1;ui16temp<=16;ui16temp++)
//             {
//                 CANA_tzRxdRegs.tzVSData.CellVolt[6][ui16temp] = 0;
//             }
//         }
//     }
//     else
//     {
//         CANA_tzTimerRegs.tzVS.CANfailCnt[6] = 0;
//     }
//
//
//
//     if(CANA_tzVSRegs.Node!= 7)
//     {
//         if(CANA_tzTimerRegs.tzVS.CANfailCnt[7]++ >= 200)
//         {
//             CANA_tzTimerRegs.tzVS.CANfailCnt[7] = 200;
//             for(ui16temp = 1;ui16temp<=16;ui16temp++)
//             {
//                 CANA_tzRxdRegs.tzVSData.CellVolt[7][ui16temp] = 0;
//             }
//         }
//     }
//     else
//     {
//         CANA_tzTimerRegs.tzVS.CANfailCnt[7] = 0;
//     }
//
//
//     if(CANA_tzVSRegs.Node!= 8)
//     {
//         if(CANA_tzTimerRegs.tzVS.CANfailCnt[8]++ >= 200)
//         {
//             CANA_tzTimerRegs.tzVS.CANfailCnt[8] = 200;
//             for(ui16temp = 1;ui16temp<=16;ui16temp++)
//             {
//                 CANA_tzRxdRegs.tzVSData.CellVolt[8][ui16temp] = 0;
//             }
//         }
//     }
//     else
//     {
//         CANA_tzTimerRegs.tzVS.CANfailCnt[8] = 0;
//     }
//
//
//     if(CANA_tzVSRegs.Node!= 9)
//     {
//         if(CANA_tzTimerRegs.tzVS.CANfailCnt[9]++ >= 200)
//         {
//             CANA_tzTimerRegs.tzVS.CANfailCnt[9] = 200;
//             for(ui16temp = 1;ui16temp<=16;ui16temp++)
//             {
//                 CANA_tzRxdRegs.tzVSData.CellVolt[9][ui16temp] = 0;
//             }
//         }
//     }
//     else
//     {
//         CANA_tzTimerRegs.tzVS.CANfailCnt[9] = 0;
//     }
//
//
//
//
//     if(CANA_tzVSRegs.Node!= 10)
//     {
//         if(CANA_tzTimerRegs.tzVS.CANfailCnt[10]++ >= 200)
//         {
//             CANA_tzTimerRegs.tzVS.CANfailCnt[10] = 200;
//             for(ui16temp = 1;ui16temp<=16;ui16temp++)
//             {
//                 CANA_tzRxdRegs.tzVSData.CellVolt[10][ui16temp] = 0;
//             }
//         }
//     }
//     else
//     {
//         CANA_tzTimerRegs.tzVS.CANfailCnt[10] = 0;
//     }
//
//
//     if(CANA_tzVSRegs.Node!= 11)
//     {
//         if(CANA_tzTimerRegs.tzVS.CANfailCnt[11]++ >= 200)
//         {
//             CANA_tzTimerRegs.tzVS.CANfailCnt[11] = 200;
//             for(ui16temp = 1;ui16temp<=16;ui16temp++)
//             {
//                 CANA_tzRxdRegs.tzVSData.CellVolt[11][ui16temp] = 0;
//             }
//         }
//     }
//     else
//     {
//         CANA_tzTimerRegs.tzVS.CANfailCnt[11] = 0;
//     }
//
//
//     if(CANA_tzVSRegs.Node!= 12)
//     {
//         if(CANA_tzTimerRegs.tzVS.CANfailCnt[12]++ >= 200)
//         {
//             CANA_tzTimerRegs.tzVS.CANfailCnt[12] = 200;
//             for(ui16temp = 1;ui16temp<=16;ui16temp++)
//             {
//                 CANA_tzRxdRegs.tzVSData.CellVolt[12][ui16temp] = 0;
//             }
//         }
//     }
//     else
//     {
//         CANA_tzTimerRegs.tzVS.CANfailCnt[12] = 0;
//     }
//
//     if(CANA_tzVSRegs.Node!= 13)
//     {
//         if(CANA_tzTimerRegs.tzVS.CANfailCnt[13]++ >= 200)
//         {
//             CANA_tzTimerRegs.tzVS.CANfailCnt[13] = 200;
//             for(ui16temp = 1;ui16temp<=16;ui16temp++)
//             {
//                 CANA_tzRxdRegs.tzVSData.CellVolt[13][ui16temp] = 0;
//             }
//         }
//     }
//     else
//     {
//         CANA_tzTimerRegs.tzVS.CANfailCnt[13] = 0;
//     }

}

/*==============================================================================
 End of File
==============================================================================*/
