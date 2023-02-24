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
#include "../main/ver.h"
/*==============================================================================
 Defines
 ==============================================================================*/

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/

CANA_tzIOREGS CANA_tzIORegs;
CANA_tzIOFLAGS CANA_tzIOflags;
CANA_tzTIMERIOREGS CANA_tzIOtimers;
CANA_tzMSREGS CANA_tzMSRegs;
CANA_tzMP_PSU_debugREGS CANA_tzMP_PSU_DebgRegs;
CANA_tzAI_IOREGS CANA_tzAIData_LPCIORegs[CANA_mTOTAL_LPCNODES],
        CANA_tzAIData_LHCIORegs[CANA_mTOTAL_LHCNODES];
CANA_tzDIFREQ_IOREGS CANA_tzAIDataFreq_IORegs[CANA_mTOTAL_IONODE];

CANA_tzTHERMAL_IOREGS CANA_tzThermal_IORegs[CANA_mTOTAL_IONODE];
CANA_tzAISENSOR_DATA CANA_tzAISensorData;
CANA_tzDISENSOR_DATA CANA_tzDISensorData;

CANA_tzTHERMOCOUPLE_DATA CANA_tzThermoCoupleData;
CANA_tzDO_IOREGS CANA_tzSetDO_IORegs;
can_tzAnaOPParams CANA_tzAnaOPParams;
CANA_tzDIG_OP CANA_tzDO[2][4];
CANA_tzDOREGS CANA_tzDOParams;

union CANA_tzDI_IOREGS CANA_tzLPCDI_IORegs[CANA_mTOTAL_LPCNODES],
        CANA_tzLHCDI_IORegs[CANA_mTOTAL_LHCNODES];
union CANA_tzAIFLT_IOREGS CANA_tzLPCAIFlt_IORegs[CANA_mTOTAL_IONODE],
        CANA_tzLHCAIFlt_IORegs[CANA_mTOTAL_IONODE];

union CANA_tzLPCIO1_DIFLT_IOREGS CANA_tzLPCIO1_DIFaultRegs;
union CANA_tzLPCIO2_DIFLT_IOREGS CANA_tzLPCIO2_DIFaultRegs;

union CANA_tzLHCIO1_DIFLT_IOREGS CANA_tzLHCIO1_DIFaultRegs;
union CANA_tzLHCIO2_DIFLT_IOREGS CANA_tzLHCIO2_DIFaultRegs;

union CANA_tzLPCIO1_AIFLT_IOREGS CANA_tzLPCIO1_AIFaultRegs;
union CANA_tzLPCIO2_AIFLT_IOREGS CANA_tzLPCIO2_AIFaultRegs;

union CANA_tzLHCIO1_AIFLT_IOREGS CANA_tzLHCIO1_AIFaultRegs;
union CANA_tzLHCIO2_AIFLT_IOREGS CANA_tzLHCIO2_AIFaultRegs;
union CANA_tzLHCIO3_AIFLT_IOREGS CANA_tzLHCIO3_AIFaultRegs;
union CANA_tzLHCIO4_AIFLT_IOREGS CANA_tzLHCIO4_AIFaultRegs;
union CANA_tzTHERMALFLT_IOREGS CANA_tzThermalFaultRegs;

union CANA_tzACTIVE_IONODE_REGS CANA_tzActNodeRegs_IO;
union CANA_tzACTIVE_VSNODE_REGS CANA_tzActNodeRegs_VS;
union CANA_tzACTIVE_VS1NODE_REGS CANA_tzActNodeRegs_VS1;

union CANA_tzDOMS_STATUS_REGS CANA_tzActMS_DOStRegs;

union CANA_tzMP_FAULTS_REGS CANA_tzActMS_FaultRegs;
union CANA_tzMS_LHCIO1DIFAULTS_REGS CANA_tzActMS_LHCIO1_DIFaultRegs;

/**************PSU ******************************/
CANA_tzPSUACDC canA_PSUACDCbuff;
CANA_tzPSUDCDC canA_PSUDCDCbuff;
CANA_tzRXDRegs CANA_tzRxdRegsACDC;

/*==============================================================================
 Macros
 ==============================================================================*/

CIRC_BUF_DEF(uiRxbufferLPCIO, 100);
CIRC_BUF_DEF(uiRxbufferLHCIO, 100);
CIRC_BUF_DEF(uiRxbufferMS, 50);
CIRC_BUF_DEF(uiRxbufferVSC, 100);
CIRC_BUF_DEF(uiRxbufferPSUACDC, 100);
CIRC_BUF_DEF(uiRxbufferPSUDCDC, 100);
CIRC_BUF_DEF(uiRxbufferPSUMASTER_DEBG, 100);
/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

void CANA_fnInitMBox(void);
void CANA_fnRXevent(void);
void CANA_fnTask(void);
void CANA_fnTx(void);
void CAN_fnComFailChk(void);

static void cana_fnmsgPrcsLPCIO(uint16_t uimsgID, uint16_t *msgData,
                                uint16_t uiNodeType);

static void cana_fnmsgPrcsLHCIO(uint16_t uimsgID, uint16_t *msgData,
                                uint16_t uiNodeType);

static void cana_fnmsgPrcsMS(uint16_t uiMsgtype, uint16_t *msgDataMS);
static float32_t limitAnalogSensorData(float32 SensorType);

void CANA_fnIOHrtBt();
void safeshutDown();

bool can_fnEnquedata(can_tzcirc_buff *ptr, uint16_t *data, uint32_t msgID,
                     uint16_t DLC);
bool can_fndequedata(can_tzcirc_buff *ptr, uint16_t *data, uint32_t *msgID,
                     uint16_t *DLC);

void CANA_fnMSTxCmds(uint16_t ui16CabiD, uint16_t NodeID,
                     CANA_tzDIG_OP *ptrDigOP);

void CANA_fnCmdsForAnaOPVs(uint16_t ui16unitID, uint16_t ui16cab_ID,
                           uint16_t ui16nodeID, can_tzAnaOPParams *ptrAO_V);

void CANA_fnCmdsForAnaOPIs(uint16_t ui16unitID, uint16_t ui16cab_ID,
                           uint16_t ui16nodeID, can_tzAnaOPParams *ptrAO_I);

void cana_CommisionMode();
void canA_fnversion(void);
/*==============================================================================
 Local Variables
 ==============================================================================*/

uint16_t uirxMsgLPCIO[8] = { 0 };
uint16_t uirxMsgLHCIO[8] = { 0 };

uint16_t uirxMsgMS[8] = { 0 };
uint16_t ui16txMsgDataIO[8] = { 0 };
uint16_t ui16txMsgDataversion[8] = { 0 };
uint16_t uirxPrcsMsgLPCIO[8] = { 0 };
uint16_t uirxPrcsMsgLHCIO[8] = { 0 };
uint16_t uirxPrcsMsgMS[8] = { 0 };
uint16_t uirxMsgVSC[8] = { 0 };
uint16_t uirxPrcsMsgVSC[8] = { 0 };
uint16_t uirxPrcsMsgPSUACDC[8] = { 0 };
uint16_t uirxPrcsMsgPSUDCDC[8] = { 0 };
uint16_t uirxMsgPSUACDC[8] = { 0 };
uint16_t uirxMsgPSUDCDC[8] = { 0 };
uint32_t u32msgID1 = 0, u32msgID2 = 0, u32msgID3 = 0, u32msgID4 = 0,u32msgID5 = 0,u32msgID6 = 0,u32msgID7 = 0;
uint16_t uiDataLength1 = 0, uiDataLength2 = 0, uiDataLength3 = 0,
        uiDataLength4 = 0,uiDataLength5 = 0,uiDataLength6 = 0;
uint16_t uiMsgtype = 0, uiNodeType = 0;
uint16_t uiCANtxMsgDataMS[8] = { 0 };
uint16_t ui16CabID = 0, ui16prev_value = 0;
uint16_t uiCabIDAO = 0, uiNodeIDAO = 0;
uint16_t ui16Cnt = 0;
uint16_t ui16StateTnstCnt = 0, ui16StateRstCnt = 0;
uint16_t testCabID = 0, testCabID1 = 0, testNodeID = 0, testCntVFD = 0,
        ui16ComsnCnt = 0;
float32 testEBV = 0;
uint16_t var = 0, ui16IOcnt = 0,ui16MPtoMScnt = 0;
uint16_t uirxMsgPSUMASTER_DEBG[8] = { 0 };
uint16_t uirxPrcsMsgPSUMASTER_DEBG[8] = { 0 },ui16CANATxCntVer = 0;
/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @Function name : void can_fnEnquedata(void)
 @brief  function to add data to Circular Buffer queue - Used to queue Received CAN Messages.
 @param void
 @return void
 ============================================================================ */

bool can_fnEnquedata(can_tzcirc_buff *ptr, uint16_t *data, uint32_t msgID,
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

    memcpy(ptr->canA_tzRxC_buffer[ptr->i_head].uiDataBuffer, data, 8);
    ptr->canA_tzRxC_buffer[ptr->i_head].u32_msgID = msgID;
    ptr->canA_tzRxC_buffer[ptr->i_head].i_DataLength = (uint16_t) DLC;
    ptr->i_head = i_count;

    return true;
}

/*=============================================================================
 @Function name : void can_fndequedata(void)
 @brief  function to delete data from Circular Buffer queue - Used to queue Received CAN Messages.
 @param void
 @return void
 ============================================================================ */
bool can_fndequedata(can_tzcirc_buff *ptr, uint16_t *data, uint32_t *msgID,
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

    memcpy(data, ptr->canA_tzRxC_buffer[ptr->i_tail].uiDataBuffer, 8);
    *msgID = ptr->canA_tzRxC_buffer[ptr->i_tail].u32_msgID;
    *DLC = ptr->canA_tzRxC_buffer[ptr->i_tail].i_DataLength;
    ptr->i_tail = i_count;

    return true;

}

/*=============================================================================
 @Function name : void CAN_fnRXevent(void)
 @Brief function to receive messages over CAN-A from Master Safety
 @Voltage Sensing, IO cards of LPC - 2 Numbers and LHC - 2 Numbers
 @param void
 @return void
 ============================================================================ */
void CANA_fnRXevent(void)
{

    //LPCIO receive event

    if (CAN_IsMessageReceived(CANA_BASE, CAN_mMAILBOX_3))
    {
        CAN_readMessage(CANA_BASE, CAN_mMAILBOX_3, uirxMsgLPCIO);

        // Enqueue the Received Messages from LPC IO into a circular Buffer

        can_fnEnquedata(&uiRxbufferLPCIO, uirxMsgLPCIO,
                        CanaRegs.CAN_IF2ARB.bit.ID,
                        CanaRegs.CAN_IF2MCTL.bit.DLC);

    }

    //LHCIO receive event

    if (CAN_IsMessageReceived(CANA_BASE, CAN_mMAILBOX_4))
    {
        CAN_readMessage(CANA_BASE, CAN_mMAILBOX_4, uirxMsgLHCIO);

        // Enqueue the Received Messages from LHC IO into a circular Buffer

        can_fnEnquedata(&uiRxbufferLHCIO, uirxMsgLHCIO,
                        CanaRegs.CAN_IF2ARB.bit.ID,
                        CanaRegs.CAN_IF2MCTL.bit.DLC);

    }

    //MS receive event

    if (CAN_IsMessageReceived(CANA_BASE, CAN_mMAILBOX_11))
    {
        CAN_readMessage(CANA_BASE, CAN_mMAILBOX_11, uirxMsgMS);

        // Enqueue the Received Messages from MS into a circular Buffer

        can_fnEnquedata(&uiRxbufferMS, uirxMsgMS, CanaRegs.CAN_IF2ARB.bit.ID,
                        CanaRegs.CAN_IF2MCTL.bit.DLC);

    }

    //VSC receive event

    if (CAN_IsMessageReceived(CANA_BASE, CAN_mMAILBOX_12))
    {
        CAN_readMessage(CANA_BASE, CAN_mMAILBOX_12, uirxMsgVSC);

        // Enqueue the Received Messages from VSC into a circular Buffer

        can_fnEnquedata(&uiRxbufferVSC, uirxMsgVSC, CanaRegs.CAN_IF2ARB.bit.ID,
                        CanaRegs.CAN_IF2MCTL.bit.DLC);
        ui32CANAVSFailCnt1 = 0;

    }
    /********************************************************************************/
    //PSU - ACDC receive event

    if (CAN_IsMessageReceived(CANA_BASE, CAN_mMAILBOX_21))
    {
        CAN_readMessage(CANA_BASE, CAN_mMAILBOX_21, uirxMsgPSUACDC);

        // Enqueue the Received Messages from PSU-ACDC into a circular Buffer

        can_fnEnquedata(&uiRxbufferPSUACDC, uirxMsgPSUACDC, CanaRegs.CAN_IF2ARB.bit.ID,
                        CanaRegs.CAN_IF2MCTL.bit.DLC);
    }
    //PSU - DCDC receive event

    if (CAN_IsMessageReceived(CANA_BASE, CAN_mMAILBOX_22))
    {
        CAN_readMessage(CANA_BASE, CAN_mMAILBOX_22, uirxMsgPSUDCDC);

        // Enqueue the Received Messages from PSU-ACDC into a circular Buffer

        can_fnEnquedata(&uiRxbufferPSUDCDC, uirxMsgPSUDCDC, CanaRegs.CAN_IF2ARB.bit.ID,
                        CanaRegs.CAN_IF2MCTL.bit.DLC);
    }
    /********************************************************************************/
    //PSU - for MASTER commands from PCAN

    if (CAN_IsMessageReceived(CANA_BASE, CAN_mMAILBOX_15))
    {
        CAN_readMessage(CANA_BASE, CAN_mMAILBOX_15, uirxMsgPSUMASTER_DEBG);

        // Enqueue the Received Messages from PSU-ACDC into a circular Buffer

        can_fnEnquedata(&uiRxbufferPSUMASTER_DEBG, uirxMsgPSUMASTER_DEBG, CanaRegs.CAN_IF2ARB.bit.ID,
                        CanaRegs.CAN_IF2MCTL.bit.DLC);
    }

}

/*=============================================================================
 @Function name : void CANA_fnTask(void)
 @brief  function to Process(Extract UnitId,NodeId, CabinetId,
 @received messages over CAN-A Bus from IO Cards, VS Cards,
 @Master Process
 @param void
 @return void
 ============================================================================ */
void CANA_fnTask(void)
{

    uint32_t ui32temp;

    // Dequeuing Messages of LPCIO Cards

    while (can_fndequedata(&uiRxbufferLPCIO, uirxPrcsMsgLPCIO, &u32msgID1,
                           &uiDataLength1))
    {

        //extracting msgIDs, UnitID, NodeID for individual messages of LPC

        ui32temp = (u32msgID1 & 0x00F00F0F);
        CANA_tzIORegs.uiMsgtypeLPCIO =
                (uint16_t) ((ui32temp & 0x00F00000) >> 20);
        CANA_tzIORegs.uiUnitID = (uint16_t) ((ui32temp & 0x00000F00) >> 8);

        CANA_tzIORegs.uiNodeLPCIO = (uint16_t) (ui32temp & 0x0F);

        //processing received messages of LPC

        cana_fnmsgPrcsLPCIO(CANA_tzIORegs.uiMsgtypeLPCIO, uirxPrcsMsgLPCIO,
                            CANA_tzIORegs.uiNodeLPCIO);
    }

    // Dequeuing Messages of LHCIO Cards

    while (can_fndequedata(&uiRxbufferLHCIO, uirxPrcsMsgLHCIO, &u32msgID2,
                           &uiDataLength2))

    {

        //extracting msgIDs, UnitID, NodeID for individual messages of LHC

        ui32temp = (u32msgID2 & 0x00F0000F);
        CANA_tzIORegs.uiMsgtypeLHCIO = (uint16_t) (ui32temp >> 20);
        CANA_tzIORegs.uiNodeLHCIO = (uint16_t) (ui32temp & 0x0F);

        //processing received messages of LHC

        cana_fnmsgPrcsLHCIO(CANA_tzIORegs.uiMsgtypeLHCIO, uirxPrcsMsgLHCIO,
                            CANA_tzIORegs.uiNodeLHCIO);
    }

    // Dequeuing Messages of Master Safety Card(MS)

    while (can_fndequedata(&uiRxbufferMS, uirxPrcsMsgMS, &u32msgID3,
                           &uiDataLength3))
    {

        //extracting Message Types from Master Safety(MS)

        ui32temp = (u32msgID3 & 0x00F00000);
        CANA_tzMSRegs.uiMsgtype = (uint16_t) (ui32temp >> 20);

        //processing received messages of MS

        cana_fnmsgPrcsMS(CANA_tzMSRegs.uiMsgtype, uirxPrcsMsgMS);
    }

    // Dequeuing Messages of Voltage Sensing Cards(VS)

    while (can_fndequedata(&uiRxbufferVSC, uirxPrcsMsgVSC, &u32msgID4,
                           &uiDataLength4))
    {

        //extracting Message Types from Voltage Sensing(VS)

        canA_VSCbuff.uiNodeID = u32msgID4 & 0xF;
        ui32temp = u32msgID4 & 0x00F00000;
        canA_VSCbuff.uiMsgType = (uint16_t) (ui32temp >> 20);

        cana_fnmsgPrcsVSC(canA_VSCbuff.uiNodeID, canA_VSCbuff.uiMsgType,
                          uirxPrcsMsgVSC);
    }
        /*********************************FOR MK2 ***ACDC********************************/
    while (can_fndequedata(&uiRxbufferPSUACDC, uirxPrcsMsgPSUACDC, &u32msgID5,
                           &uiDataLength4))
    {

        canA_PSUACDCbuff.uiNodeID = u32msgID5 & 0xF;
        ui32temp = u32msgID5 & 0x00F0000F;
        canA_PSUACDCbuff.uiMsgType = (uint16_t) (ui32temp >> 20);
        //canA_PSUACDCbuff.uiMsgIDL = (uint16_t) (ui32temp >> 24);
        cana_fnmsgPrcsACDC(canA_PSUACDCbuff.uiNodeID, canA_PSUACDCbuff.uiMsgType,
                          uirxPrcsMsgPSUACDC);
    }
    /*********************************FOR MK2 ******DCDC*****************************/
    while (can_fndequedata(&uiRxbufferPSUDCDC, uirxPrcsMsgPSUDCDC, &u32msgID6,
                           &uiDataLength5))
    {


        ui32temp = (u32msgID6 & 0x00F00F0F);//u32msgID6 & 0x00F00000;
        canA_PSUDCDCbuff.uiNodeID = u32msgID6 & 0xF;
        canA_PSUDCDCbuff.uiMsgType = (uint16_t) ((ui32temp & 0x00F00000) >> 20);
        canA_PSUDCDCbuff.uiUnitID = (uint16_t) ((ui32temp & 0x00000F00) >> 8);
       // canA_PSUDCDCbuff.uiMsgIDL = (uint16_t) ((ui32temp & 0x0F000000) >> 24);

        cana_fnmsgPrcsDCDC(canA_PSUDCDCbuff.uiNodeID, canA_PSUDCDCbuff.uiUnitID,canA_PSUDCDCbuff.uiMsgType,
                          uirxPrcsMsgPSUDCDC);
    }
    /*********************************FOR MASTER - PSU DEBUG****************************/
     while (can_fndequedata(&uiRxbufferPSUMASTER_DEBG, uirxPrcsMsgPSUMASTER_DEBG, &u32msgID7,
                            &uiDataLength6))
     {


         ui32temp = (u32msgID7 & 0x00F00F0F);//u32msgID6 & 0x00F00000;
         CANA_tzMP_PSU_DebgRegs.uiMsgtype = (uint16_t) ((ui32temp & 0x00F00000) >> 20);

         cana_fnmsgPrcsMASTERDEBG(uirxPrcsMsgPSUMASTER_DEBG);
     }

}

/*=============================================================================
 @Function name : void can_fnmsgProcess(void)
 @brief  function to Process LPCIO Cards messages over CAN-A Bus

 @param void
 @return void
 ============================================================================ */
static void cana_fnmsgPrcsLPCIO(uint16_t uiMsgtype, uint16_t *msgDataIO,
                                uint16_t uiNodeType)

{

    // Six Types of Messages are being received from each IO card of LPC

    switch (uiMsgtype)
    {

    // 1st Message (Case 0) - Digital Inputs Status received from IO Cards.

    // This acts as the Heart beat Message from each IO Card.
    // IO Card sends Heart Beat Count which gets incremented on every message.Count gets rolled off at 255(Separate Counts for LPC and LHCIO)
    // Before Processing every message, MP has to check, present Count Not equal to Previous Count.
    // Upon Communication failure, present count is equal to previous Count.
    // This is monitored for some time, can Communication failure is announced from that particular IO Card.

    case IO_DI_MSGID:

        switch (uiNodeType)
        {

        case LPC_30:

            if (CANA_tzIOtimers.RxCntLPC30 != msgDataIO[0]) // Heart Beat of LPCIO_30
            {
                CANA_tzIOtimers.RxCntLPC30 = msgDataIO[0];

                if (msgDataIO[1] == msgDataIO[2])
                {
                    CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].all = msgDataIO[1];
                    CANA_tzLPCAIFlt_IORegs[CANA_mLPC30_IO].all = msgDataIO[5];

                    CANA_tzIOflags.btLPC30CommnStart = true;
                    CANA_tzIOtimers.LPC30ComFailCnt = 0; // CAN failure Count of LPCIO_30 is made 0 if present Count != Previous Count
                    CANA_tzActNodeRegs_IO.bit.bt_LPC30 = 1;
                }
            }
            break;

        case LPC_31:

            if (CANA_tzIOtimers.RxCntLPC31 != msgDataIO[0]) // Heart Beat of LPCIO_31
            {
                CANA_tzIOtimers.RxCntLPC31 = msgDataIO[0];

                if (msgDataIO[1] == msgDataIO[2])
                {
                    CANA_tzLPCDI_IORegs[CANA_mLPC31_IO].all = msgDataIO[1];
                    CANA_tzLPCAIFlt_IORegs[CANA_mLPC31_IO].all = msgDataIO[5];

                    CANA_tzIOflags.btLPC31CommnStart = true;
                    CANA_tzIOtimers.LPC31ComFailCnt = 0; // CAN failure Count of LPCIO_31 is made 0 if present Count != Previous Count
                    CANA_tzActNodeRegs_IO.bit.bt_LPC31 = 1;

                }
            }
            break;
        }
        break;

        // 2nd Message (Case 1) - Analog Inputs Status received from IO Cards.

    case IO_AIBLK1_MSGID:

        switch (uiNodeType)
        {

        case LPC_30:

            CANA_tzAIData_LPCIORegs[CANA_mLPC30_IO].AI0_Data = ((msgDataIO[0]
                    << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC30_IO].AI1_Data = ((msgDataIO[2]
                    << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC30_IO].AI2_Data = ((msgDataIO[4]
                    << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC30_IO].AI3_Data = ((msgDataIO[6]
                    << 8) | (msgDataIO[7])) * 0.001;
            break;

        case LPC_31:

            CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI0_Data = ((msgDataIO[0]
                    << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI1_Data = ((msgDataIO[2]
                    << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI2_Data = ((msgDataIO[4]
                    << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI3_Data = ((msgDataIO[6]
                    << 8) | (msgDataIO[7])) * 0.001;

            // Updating received values to Analog sensor Structure

            CANA_tzAISensorData.HYS_401 =
                    CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI3_Data;

            break;
        default:
            break;
        }
        break;

        // 3rd Message (Case 2) - Analog Inputs Status received from IO Cards.

    case IO_AIBLK2_MSGID:

        switch (uiNodeType)
        {

        case LPC_30:

            CANA_tzAIData_LPCIORegs[CANA_mLPC30_IO].AI4_Data = ((msgDataIO[0]
                    << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC30_IO].AI5_Data = ((msgDataIO[2]
                    << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC30_IO].AI6_Data = ((msgDataIO[4]
                    << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC30_IO].AI7_Data = ((msgDataIO[6]
                    << 8) | (msgDataIO[7])) * 0.001;
            break;

        case LPC_31:

            CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI4_Data = ((msgDataIO[0]
                    << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI5_Data = ((msgDataIO[2]
                    << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI6_Data = ((msgDataIO[4]
                    << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI7_Data = ((msgDataIO[6]
                    << 8) | (msgDataIO[7])) * 0.001;

            // Updating received values to Analog sensor Structure

            CANA_tzAISensorData.HYS_101 =
                    CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI4_Data;
            CANA_tzAISensorData.HYS_102 =
                    CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI5_Data;
            CANA_tzAISensorData.OXS_101 =
                    CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI6_Data;
            CANA_tzAISensorData.HYS_501 =
                    CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI7_Data;

            break;
        default:
            break;
        }
        break;

        // 4th Message (Case 3) - Analog Inputs Status received from IO Cards.

    case IO_THERMAL_MSGID:

        break;

        // 5th Message (Case 4) - Digital Inputs Status received from IO Cards.


    case IO_DI1_FREQ_MSGID:

        switch (uiNodeType)
        {
        case LPC_31:

            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI0_FreqData =
                    ((msgDataIO[0] << 8) | (msgDataIO[1])) * 0.01;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI1_FreqData =
                    ((msgDataIO[2] << 8) | (msgDataIO[3])) * 0.01;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI2_FreqData =
                    ((msgDataIO[4] << 8) | (msgDataIO[5])) * 0.01;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI3_FreqData =
                    ((msgDataIO[6] << 8) | (msgDataIO[7])) * 0.01;

            // Updating received values to Digital sensor Structure

            CANA_tzDISensorData.PURGE101 =
                    CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI0_FreqData;
            CANA_tzDISensorData.PURGE102 =
                    CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI1_FreqData;
            CANA_tzDISensorData.PURGE501 =
                    CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI2_FreqData;
            CANA_tzDISensorData.PURGE502 =
                    CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI3_FreqData;

            break;
        default:
            break;
        }
        break;

        // 6th Message (Case 5) - Digital Inputs Status received from IO Cards.

    case IO_DI2_FREQ_MSGID:

        switch (uiNodeType)
        {
        case LPC_31:

            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI4_FreqData =
                    ((msgDataIO[0] << 8) | (msgDataIO[1])) * 0.01;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI5_FreqData =
                    ((msgDataIO[2] << 8) | (msgDataIO[3])) * 0.01;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI6_FreqData =
                    ((msgDataIO[4] << 8) | (msgDataIO[5])) * 0.01;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI7_FreqData =
                    ((msgDataIO[6] << 8) | (msgDataIO[7])) * 0.01;

            CANA_tzDISensorData.PURGE401 =
                    CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI4_FreqData;
            CANA_tzDISensorData.PURGE402 =
                    CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI5_FreqData;

            break;
        default:
            break;
        }
        break;

    }

    CANA_tzAISensorData.HYS_101 = limitAnalogSensorData(
            CANA_tzAISensorData.HYS_101);
    CANA_tzAISensorData.HYS_102 = limitAnalogSensorData(
            CANA_tzAISensorData.HYS_102);
    CANA_tzAISensorData.HYS_501 = limitAnalogSensorData(
            CANA_tzAISensorData.HYS_501);
    CANA_tzAISensorData.HYS_401 = limitAnalogSensorData(
            CANA_tzAISensorData.HYS_401);
    CANA_tzAISensorData.OXS_101 = limitAnalogSensorData(
            CANA_tzAISensorData.OXS_101);
}

/*=============================================================================
 @Function name : void can_fnmsgProcess(void)
 @brief  function to Process LHCIO messages over CAN

 @param void
 @return void
 ============================================================================ */
static void cana_fnmsgPrcsLHCIO(uint16_t uiMsgtype, uint16_t *msgDataIO,
                                uint16_t uiNodeType)

{
    // Six Types of Messages are being received from each IO card of LPC

    switch (uiMsgtype)
    {
    case IO_DI_MSGID:

        switch (uiNodeType)
        {

        // 1st Message (Case 0) - Digital Inputs Status received from IO Cards.

        // This acts as the Heart beat Message from each IO Card.
        // IO Card sends Heart Beat Count which gets incremented on every message.Count gets rolled off at 255(Separate Counts for LPC and LHCIO)
        // Before Processing every message, MP has to check, present Count Not equal to Previous Count.
        // Upon Communication failure, present count is equal to previous Count.
        // This is monitored for some time, can Communication failure is announced from that particular IO Card.

        case LHC_10:

            if (CANA_tzIOtimers.RxCntLHC10 != msgDataIO[0])
            {
                CANA_tzIOtimers.RxCntLHC10 = msgDataIO[0];

                if (msgDataIO[1] == msgDataIO[2])
                {
                    CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].all = msgDataIO[1];
                    CANA_tzIORegs.CJC[CANA_mLHC10_IO] = ((msgDataIO[3] << 8)
                            | (msgDataIO[4])) * 0.01;
                    CANA_tzLHCAIFlt_IORegs[CANA_mLHC10_IO].all = msgDataIO[5];

                    CANA_tzIOflags.btLHC10CommnStart = true;
                    CANA_tzIOtimers.LHC10ComFailCnt = 0;
                    CANA_tzActNodeRegs_IO.bit.bt_LHC10 = 1;

                }
            }
            break;

        case LHC_11:

            if (CANA_tzIOtimers.RxCntLHC11 != msgDataIO[0])
            {
                CANA_tzIOtimers.RxCntLHC11 = msgDataIO[0];

                if (msgDataIO[1] == msgDataIO[2])
                {
                    CANA_tzLHCDI_IORegs[CANA_mLHC11_IO].all = msgDataIO[1];
                    CANA_tzLHCAIFlt_IORegs[CANA_mLHC11_IO].all = msgDataIO[5];
                    CANA_tzIOflags.btLHC11CommnStart = true;
                    CANA_tzIOtimers.LHC11ComFailCnt = 0;
                    CANA_tzActNodeRegs_IO.bit.bt_LHC11 = 1;
                }
            }
            break;
        case LHC_12:

            if (CANA_tzIOtimers.RxCntLHC12 != msgDataIO[0])
            {
                CANA_tzIOtimers.RxCntLHC12 = msgDataIO[0];

                if (msgDataIO[1] == msgDataIO[2])
                {
                    CANA_tzLHCDI_IORegs[CANA_mLHC12_IO].all = msgDataIO[1];
                    CANA_tzLHCAIFlt_IORegs[CANA_mLHC12_IO].all = msgDataIO[5];
                    CANA_tzIOflags.btLHC12CommnStart = true;
                    CANA_tzIOtimers.LHC12ComFailCnt = 0;
                    CANA_tzActNodeRegs_IO.bit.bt_LHC12 = 1;
                }
            }
            break;
        case LHC_13:

            if (CANA_tzIOtimers.RxCntLHC13 != msgDataIO[0])
            {
                CANA_tzIOtimers.RxCntLHC13 = msgDataIO[0];

                if (msgDataIO[1] == msgDataIO[2])
                {
                    CANA_tzLHCDI_IORegs[CANA_mLHC13_IO].all = msgDataIO[1];
                    CANA_tzLHCAIFlt_IORegs[CANA_mLHC13_IO].all = msgDataIO[5];
                    CANA_tzIOflags.btLHC13CommnStart = true;
                    CANA_tzIOtimers.LHC13ComFailCnt = 0;
                    CANA_tzActNodeRegs_IO.bit.bt_LHC13 = 1;
                }
            }
            break;
        }
        break;

        // 2nd Message (Case 1) - Analog Inputs Status received from IO Cards.

    case IO_AIBLK1_MSGID:

        switch (uiNodeType)
        {

        case LHC_10:

            CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI0_Data = ((msgDataIO[0]
                    << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI1_Data = ((msgDataIO[2]
                    << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI2_Data = ((msgDataIO[4]
                    << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI3_Data = ((msgDataIO[6]
                    << 8) | (msgDataIO[7])) * 0.001;
            break;

        case LHC_11:

            CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI0_Data = ((msgDataIO[0]
                    << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI1_Data = ((msgDataIO[2]
                    << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI2_Data = ((msgDataIO[4]
                    << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI3_Data = ((msgDataIO[6]
                    << 8) | (msgDataIO[7])) * 0.001;

//            CANA_tzAISensorData.PRT_402 =
//                    CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI3_Data;

            break;
        case LHC_12:

            CANA_tzAIData_LHCIORegs[CANA_mLHC12_IO].AI0_Data = ((msgDataIO[0]    //MFM-402
                    << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC12_IO].AI1_Data = ((msgDataIO[2]     //MFM-401
                    << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC12_IO].AI2_Data = ((msgDataIO[4]    //PRT-407
                    << 8) | (msgDataIO[5])) * 0.001;
//            CANA_tzAIData_LHCIORegs[CANA_mLHC12_IO].AI3_Data = ((msgDataIO[6]   //PRT-406
//                    << 8) | (msgDataIO[7])) * 0.001;

            CANA_tzAISensorData.MFM_401 =
                                CANA_tzAIData_LHCIORegs[CANA_mLHC12_IO].AI0_Data;
            CANA_tzAISensorData.MFM_402 =
                                CANA_tzAIData_LHCIORegs[CANA_mLHC12_IO].AI1_Data;
            CANA_tzAISensorData.PRT_406 =
                    CANA_tzAIData_LHCIORegs[CANA_mLHC12_IO].AI2_Data;
//            CANA_tzAISensorData.PRT_406 =
//                    CANA_tzAIData_LHCIORegs[CANA_mLHC12_IO].AI3_Data;

            break;
        case LHC_13:

                   CANA_tzAIData_LHCIORegs[CANA_mLHC13_IO].AI3_Data = ((msgDataIO[6]   //PRT-406
                           << 8) | (msgDataIO[7])) * 0.001;

                   CANA_tzAISensorData.PRT_405 =
                           CANA_tzAIData_LHCIORegs[CANA_mLHC13_IO].AI3_Data;

                   break;


        default:
            break;
        }
        break;

        // 3rd Message (Case 2) - Analog Inputs Status received from IO Cards.

    case IO_AIBLK2_MSGID:

        switch (uiNodeType)
        {

        case LHC_10:

            CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI4_Data = ((msgDataIO[0]
                    << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI5_Data = ((msgDataIO[2]
                    << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI6_Data = ((msgDataIO[4]
                    << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI7_Data = ((msgDataIO[6]
                    << 8) | (msgDataIO[7])) * 0.001;

            CANA_tzAISensorData.LVL_101 =
                    CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI4_Data;
            CANA_tzAISensorData.PRT_101 =
                    CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI5_Data;
            CANA_tzAISensorData.PRT_102 =
                    CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI6_Data;
            CANA_tzAISensorData.COS_101 =
                    CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI7_Data;

            break;

        case LHC_11:

            CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI4_Data = ((msgDataIO[0]
                    << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI5_Data = ((msgDataIO[2]
                    << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI6_Data = ((msgDataIO[4]
                    << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI7_Data = ((msgDataIO[6]
                    << 8) | (msgDataIO[7])) * 0.001;

            CANA_tzAISensorData.PRT_401 =
                    CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI5_Data;
            CANA_tzAISensorData.TE_401 =
                    CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI6_Data;
            CANA_tzAISensorData.DPT_401 =
                    CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI7_Data;

            break;
        case LHC_12:

            CANA_tzAIData_LHCIORegs[CANA_mLHC12_IO].AI4_Data = ((msgDataIO[0]  //PRT 402
                    << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC12_IO].AI5_Data = ((msgDataIO[2]  //PRT 403
                    << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC12_IO].AI6_Data = ((msgDataIO[4]  //PRT 404
                    << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC12_IO].AI7_Data = ((msgDataIO[6]  //PRT 405
                    << 8) | (msgDataIO[7])) * 0.001;

            CANA_tzAISensorData.PRT1 =
                    CANA_tzAIData_LHCIORegs[CANA_mLHC12_IO].AI4_Data;
            CANA_tzAISensorData.PRT_402 =
                                CANA_tzAIData_LHCIORegs[CANA_mLHC12_IO].AI5_Data;
            CANA_tzAISensorData.PRT_403 =
                                CANA_tzAIData_LHCIORegs[CANA_mLHC12_IO].AI6_Data;
            CANA_tzAISensorData.PRT_404 =
                                CANA_tzAIData_LHCIORegs[CANA_mLHC12_IO].AI7_Data;

            break;

        case LHC_13:

            CANA_tzAIData_LHCIORegs[CANA_mLHC13_IO].AI4_Data = ((msgDataIO[0]  //RTD 401
                    << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC13_IO].AI5_Data = ((msgDataIO[2]  //RTD 402
                    << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC13_IO].AI6_Data = ((msgDataIO[4]  //RTD 403
                    << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC13_IO].AI7_Data = ((msgDataIO[6]  //RTD 404
                    << 8) | (msgDataIO[7])) * 0.001;

            CANA_tzAISensorData.RTD_401 =
                    CANA_tzAIData_LHCIORegs[CANA_mLHC13_IO].AI4_Data;
            CANA_tzAISensorData.RTD_402 =
                    CANA_tzAIData_LHCIORegs[CANA_mLHC13_IO].AI5_Data;
            CANA_tzAISensorData.RTD_403 =
                                CANA_tzAIData_LHCIORegs[CANA_mLHC13_IO].AI6_Data;
            CANA_tzAISensorData.RTD_404 =
                                CANA_tzAIData_LHCIORegs[CANA_mLHC13_IO].AI7_Data;

            break;
     /****************************************************************************************/

        default:
            break;
        }
        break;

        // 4th Message (Case 3) - Analog Inputs Status received from IO Cards.

    case IO_THERMAL_MSGID:

        switch (uiNodeType)
        {
        case LHC_10:

            CANA_tzThermal_IORegs[CANA_mLHC10_IO].T0_Data = ((msgDataIO[0] << 8)
                    | (msgDataIO[1])) * 0.001;
            CANA_tzThermal_IORegs[CANA_mLHC10_IO].T1_Data = ((msgDataIO[2] << 8)
                    | (msgDataIO[3])) * 0.001;
            CANA_tzThermal_IORegs[CANA_mLHC10_IO].T2_Data = ((msgDataIO[4] << 8)
                    | (msgDataIO[5])) * 0.001;
            CANA_tzThermal_IORegs[CANA_mLHC10_IO].T3_Data = ((msgDataIO[6] << 8)
                    | (msgDataIO[7])) * 0.001;

            CANA_tzThermoCoupleData.TTC_101 =
                    CANA_tzThermal_IORegs[CANA_mLHC10_IO].T0_Data;
            CANA_tzThermoCoupleData.TTC_102 =
                    CANA_tzThermal_IORegs[CANA_mLHC10_IO].T1_Data;
            CANA_tzThermoCoupleData.TTC_301 =
                    CANA_tzThermal_IORegs[CANA_mLHC10_IO].T2_Data;
            CANA_tzThermoCoupleData.KTC_401 =
                    CANA_tzThermal_IORegs[CANA_mLHC10_IO].T3_Data;

            break;
        default:
            break;
        }
        break;

        // 5th Message (Case 4) - Digital Inputs Status received from IO Cards.

    case IO_DI1_FREQ_MSGID:
        break;

        // 6th Message (Case 5) - Digital Inputs Status received from IO Cards.


    case IO_DI2_FREQ_MSGID:
        break;

    default:
        break;

    }

    CANA_tzAISensorData.LVL_101 = limitAnalogSensorData(
            CANA_tzAISensorData.LVL_101);
    CANA_tzAISensorData.PRT_101 = limitAnalogSensorData(
            CANA_tzAISensorData.PRT_101);
    CANA_tzAISensorData.PRT_102 = limitAnalogSensorData(
            CANA_tzAISensorData.PRT_102);
    CANA_tzAISensorData.PRT_401 = limitAnalogSensorData(
            CANA_tzAISensorData.PRT_401);
    CANA_tzAISensorData.PRT1 = limitAnalogSensorData(
            CANA_tzAISensorData.PRT1);
    CANA_tzAISensorData.TE_401 = limitAnalogSensorData(
            CANA_tzAISensorData.TE_401);
    CANA_tzAISensorData.COS_101 = limitAnalogSensorData(
            CANA_tzAISensorData.COS_101);
    CANA_tzAISensorData.OXS_101 = limitAnalogSensorData(
            CANA_tzAISensorData.OXS_101);
    CANA_tzAISensorData.DPT_401 = limitAnalogSensorData(
            CANA_tzAISensorData.DPT_401);
    /**********************************************************/
    CANA_tzAISensorData.PRT_402 = limitAnalogSensorData(
            CANA_tzAISensorData.PRT_402);
    CANA_tzAISensorData.PRT_403 = limitAnalogSensorData(
            CANA_tzAISensorData.PRT_403);
    CANA_tzAISensorData.PRT_404 = limitAnalogSensorData(
            CANA_tzAISensorData.PRT_404);
    CANA_tzAISensorData.PRT_405 = limitAnalogSensorData(
            CANA_tzAISensorData.PRT_405);
    CANA_tzAISensorData.PRT_406 = limitAnalogSensorData(
            CANA_tzAISensorData.PRT_406);

    CANA_tzAISensorData.RTD_401 = limitAnalogSensorData(
            CANA_tzAISensorData.RTD_401);
    CANA_tzAISensorData.RTD_402 = limitAnalogSensorData(
            CANA_tzAISensorData.RTD_402);
    CANA_tzAISensorData.RTD_403 = limitAnalogSensorData(
            CANA_tzAISensorData.RTD_403);
    CANA_tzAISensorData.RTD_404 = limitAnalogSensorData(
            CANA_tzAISensorData.RTD_404);
}

/*=============================================================================
 @Function name : void can_fnmsgProcess(void)
 @brief  function to Process Master Safety messages over CAN

 @param void
 @return void
 ============================================================================ */
static void cana_fnmsgPrcsMS(uint16_t uiMsgtype, uint16_t *msgDataMS)
{

    // Messages from Master safety Controller are being processed here

    switch (uiMsgtype)
    {

    // 1st Message (Case 0) -   ,
    //
    case 0:

        if (CANA_tzMSRegs.RxCntMS != msgDataMS[0])
        {
            CANA_tzMSRegs.RxCntMS = msgDataMS[0];       // Heart Beat Signal for Communication Check
            CANA_tzMSRegs.StartCmd = msgDataMS[1];      // Start/Stop Command
            CANA_tzMSRegs.PresentStMS = msgDataMS[2];   // Status MS
            CANA_tzMSRegs.AOCmd = msgDataMS[3];         // Analog Voltages Enable/Disable Command
            CANA_tzActMS_FaultRegs.all = msgDataMS[4];  // Faults of MS
            CANA_tzMSRegs.AOVFan101 = msgDataMS[5];     // Analog Voltages to be applied to purge Fans are being received here.
            CANA_tzMSRegs.AOVFan501 = msgDataMS[6];
            CANA_tzMSRegs.AOVFan401 = msgDataMS[7];
            CANA_tzMSRegs.btMSComStart = true;
            CANA_tzMSRegs.MSComFailCnt = 0;

        }

        if ((CANA_tzMSRegs.AOCmd & 0x01) == 0x01)
        {
            CANA_tzMSRegs.TurnONPurge101 = 1;
        }
        else
        {
            CANA_tzMSRegs.TurnONPurge101 = 0;
        }

        if ((CANA_tzMSRegs.AOCmd & 0x02) == 0x02)
        {
            CANA_tzMSRegs.TurnONPurge501 = 1;
        }
        else
        {
            CANA_tzMSRegs.TurnONPurge501 = 0;
        }

        if ((CANA_tzMSRegs.AOCmd & 0x0004) == 0x0004)
        {
            CANA_tzMSRegs.TurnONPurge401 = 1;
        }
        else
        {
            CANA_tzMSRegs.TurnONPurge401 = 0;
        }
        break;

    case 1:

        CANA_tzActMS_DOStRegs.all = msgDataMS[0];   // Digital Output Status
CANA_tzActMS_LHCIO1_DIFaultRegs.all = msgDataMS[1];   // Digital Output Status
        break;

    default:
        break;

    }

    // Updating the status of Master Safety

    switch (CANA_tzMSRegs.PresentStMS)
    {

    case 0:
        STAT_tzStateMacMS.Present_st = MS_STANDBY;
        break;

    case 1:
        STAT_tzStateMacMS.Present_st = MS_PURGE;

        break;
    case 2:
        STAT_tzStateMacMS.Present_st = MS_IOPOWER;
        break;
    case 3:
        STAT_tzStateMacMS.Present_st = MS_ARMED_POWER;
        break;
    case 4:
        STAT_tzStateMacMS.Present_st = MS_FAULT;
        break;
    case 5:
        STAT_tzStateMacMS.Present_st = MS_SHUTDOWN;
        break;
    default:
        break;
    }

}


/*=============================================================================
 @Function name : void CANA_fnComFailChk(void)
 @brief  function to Detect CAN failure from different IO Cards/ Voltage Sensing Cards

 @param void
 @return void
 ============================================================================ */

void CANA_fnComFailChk()
{


    int i;
    uint16_t ui16temp;

    CANA_fnPSUComFailChk();    //PSU CAN FAIL

    // Master Safety CAN Fail after 30sec of Communication break

    CANA_tzMSRegs.MSComFailCnt++;
    if (CANA_tzMSRegs.MSComFailCnt >= 3000)
    {
        CANA_tzMSRegs.btMSComStart = false;
        CANA_tzMSRegs.MSComFailCnt = 3001;
    }

    // LHCIO-1 CAN Fail after 30sec of Communication break


    CANA_tzIOtimers.LHC10ComFailCnt++;
    if (CANA_tzIOtimers.LHC10ComFailCnt >= 3000)
    {
        CANA_tzIOtimers.LHC10ComFailCnt = 3001;
        CANA_tzIOflags.btLHC10CommnStart = false;
        CANA_tzActNodeRegs_IO.bit.bt_LHC10 = 0;

    }

    // LHCIO-2 CAN Fail after 30sec of Communication break


    CANA_tzIOtimers.LHC11ComFailCnt++;
    if (CANA_tzIOtimers.LHC11ComFailCnt >= 3000)
    {
        CANA_tzIOtimers.LHC11ComFailCnt = 3001;
        CANA_tzIOflags.btLHC11CommnStart = false;
        CANA_tzActNodeRegs_IO.bit.bt_LHC11 = 0;

    }


    CANA_tzIOtimers.LHC12ComFailCnt++;
    if (CANA_tzIOtimers.LHC12ComFailCnt >= 3000)
    {
        CANA_tzIOtimers.LHC12ComFailCnt = 3001;
        CANA_tzIOflags.btLHC12CommnStart = false;
        CANA_tzActNodeRegs_IO.bit.bt_LHC12 = 0;

    }

    CANA_tzIOtimers.LHC13ComFailCnt++;
    if (CANA_tzIOtimers.LHC13ComFailCnt >= 3000)
    {
        CANA_tzIOtimers.LHC13ComFailCnt = 3001;
        CANA_tzIOflags.btLHC13CommnStart = false;
        CANA_tzActNodeRegs_IO.bit.bt_LHC13 = 0;

    }

    // LPCIO-1 CAN Fail after 30sec of Communication break


    CANA_tzIOtimers.LPC30ComFailCnt++;
    if (CANA_tzIOtimers.LPC30ComFailCnt >= 3000)
    {
        CANA_tzIOtimers.LPC30ComFailCnt = 3001;
        CANA_tzActNodeRegs_IO.bit.bt_LPC30 = 0;
    }


    // LPCIO-2 CAN Fail after 30sec of Communication break


    CANA_tzIOtimers.LPC31ComFailCnt++;
    if (CANA_tzIOtimers.LPC31ComFailCnt >= 3000)
    {
        CANA_tzIOtimers.LPC31ComFailCnt = 3001;
        CANA_tzActNodeRegs_IO.bit.bt_LPC31 = 0;
    }

    if (ui16CANAVSFailTrig1 == 1)
    {
       // canA_VSCbuff.uiNodeID = 0; //11843020
    }

    // VS-1-12 CAN Fail after 20sec of Communication break

     for (i = 1; i <= 16; i++)
    {
        if (canA_tzVSC_info[i].isReceived)
        {
            canA_tzVSC_info[i].isReceived = 0;
            canA_tzVSC_info[i].CANfailCnt = 0;
            //CANA_tzActNodeRegs_VS1.bit.bt_VS16 = 1;
        }
        else
        {
            if (++canA_tzVSC_info[i].CANfailCnt >= 2000)
            {
                canA_tzVSC_info[i].CANfailCnt = 2000;
                //CANA_tzActNodeRegs_VS1.bit.bt_VS16 = 0;
                for (ui16temp = 1; ui16temp <= 16; ui16temp++)
                {
                    canA_tzVSC_info[i].f32Cellvolt[ui16temp] = 0;
                }
            }
        }
    }
}

/*=============================================================================
 @Function name : void CANA_fnTx(void)
 @brief  function to transmit CAN Messages over CAN-A bus to IO Cards

 @param void
 @return void
 ============================================================================ */

void CANA_fnTx()
{
   CANA_fnPSUTx();

   canA_fnversion();
    // Common Messages Irrespective of States

    // Below Commands Can be used for Manual Testing Mode


    CANA_tzMSRegs.HBCntMS++;
    if (CANA_tzMSRegs.HBCntMS >= 50)
    {
//        CANA_fnMSTxCmds(testCabID, testNodeID,
//                        &CANA_tzDO[testCabID][testNodeID]); //Heartbeat

        CANA_fnMSTxCmds(testCabID, testNodeID,
                        &CANA_tzDO[testCabID1][testNodeID]); //Heartbeat

        CANA_tzMSRegs.HBCntMS = 0;
    }

    if (STAT_tzStateMac.Present_st == COMMISSION)
  //  if (STAT_tzStateMac.Present_st == STAND_BY)
    {
        ui16manualTesting = 1;
    }
    else
    {
        ui16manualTesting = 0;
    }

//    // Below 2 Messages can be removed after manual testing
//
//    CANA_tzMSRegs.ManualTestCntMS++;
//    if (CANA_tzMSRegs.ManualTestCntMS == 25)
//    {
//        CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV4 = (testCntVFD * 100);
//
//        CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
//                              &CANA_tzAnaOPParams); // Control Speed of Purge fan1
//    }
//    else if ((CANA_tzMSRegs.ManualTestCntMS == 50))
//    {
//        CANA_tzAnaOPParams.CANA_tzAOI[1][0].AOI1 = (testEBV * 10);
//
//        CANA_fnCmdsForAnaOPIs(CANA_tzIORegs.uiUnitID, 1, 0,
//                              &CANA_tzAnaOPParams);
//
//    }
//
//    else if (CANA_tzMSRegs.ManualTestCntMS > 100)
//    {
//        CANA_tzMSRegs.ManualTestCntMS = 0;
//
//    }


    // Controlling the speed of Purge Fans irrespective of states. Command to IO cards will be sent from MP
    // Analog voltages/ AO Enable Signals are received from Master Safety

    ui16Cnt++;

    if (ui16Cnt >= 25)
    {

        if (CANA_tzMSRegs.TurnONPurge101 == 1)
        {
            CANA_tzMSRegs.fan101ResetCnt = 0;
            CANA_tzMSRegs.fan101SetCnt++;
            if (CANA_tzMSRegs.fan101SetCnt == 2)
            {
                CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV1 =
                        (CANA_tzMSRegs.AOVFan101 * 100);

                CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
                                      &CANA_tzAnaOPParams); // Control Speed of Purge fan101
                CANA_tzMSRegs.fan101SetCnt = 0;
            }

        }
        else
        {

            CANA_tzMSRegs.fan101SetCnt = 0;
            CANA_tzMSRegs.fan101ResetCnt++;
            if (CANA_tzMSRegs.fan101ResetCnt == 2)
            {
                CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV1 = (0);

                CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
                                      &CANA_tzAnaOPParams); // Control Speed of Purge fan101
                CANA_tzMSRegs.fan101ResetCnt = 0;
            }
        }

        if (CANA_tzMSRegs.TurnONPurge501 == 1)
        {
            CANA_tzMSRegs.fan501ResetCnt = 0;
            CANA_tzMSRegs.fan501SetCnt++;
            if (CANA_tzMSRegs.fan501SetCnt == 2)
            {
                CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV2 =
                        (CANA_tzMSRegs.AOVFan501 * 100);

                CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
                                      &CANA_tzAnaOPParams); // Control Speed of Purge fan 501
                CANA_tzMSRegs.fan501SetCnt = 0;
            }

        }
        else
        {

            CANA_tzMSRegs.fan501SetCnt = 0;
            CANA_tzMSRegs.fan501ResetCnt++;
            if (CANA_tzMSRegs.fan501ResetCnt == 2)
            {
                CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV2 = (0);

                CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
                                      &CANA_tzAnaOPParams); // Control Speed of Purge fan
                CANA_tzMSRegs.fan501ResetCnt = 0;
            }
        }

        if (CANA_tzMSRegs.TurnONPurge401 == 1)
        {
            CANA_tzMSRegs.fan401ResetCnt = 0;
            CANA_tzMSRegs.fan401SetCnt++;
            if (CANA_tzMSRegs.fan401SetCnt == 2)
            {
                CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV3 =
                        (CANA_tzMSRegs.AOVFan401 * 100);

                CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
                                      &CANA_tzAnaOPParams); // Control Speed of Purge fan 401
                CANA_tzMSRegs.fan401SetCnt = 0;
            }

        }
        else
        {

            CANA_tzMSRegs.fan401SetCnt = 0;
            CANA_tzMSRegs.fan401ResetCnt++;
            if (CANA_tzMSRegs.fan401ResetCnt == 2)
            {
                CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV3 = (0);

                CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
                                      &CANA_tzAnaOPParams); // Control Speed of Purge fan 401
                CANA_tzMSRegs.fan401ResetCnt = 0;
            }
        }

        ui16Cnt = 0;
    }


    // Messages to be sent to Different IO's based on Different states.

    switch (STAT_tzStateMac.Present_st)
    {

    // Stand By States - TURN OFF all Digital IO's and Analog Voltage for VFD

    case STAND_BY:

        ui16IOcnt++;

        switch (ui16IOcnt)
        {
        case 0:
            CANA_tzDO[0][0].all = 0x0;
            CANA_fnMSTxCmds(3, 1, &CANA_tzDO[0][0]);

            CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV4 = 0;

            CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
                                  &CANA_tzAnaOPParams); // Turn OFF Analog Output for PMP101 VFD

            break;

        case 10:
            CANA_tzDO[1][3].all = 0x0;
            CANA_fnMSTxCmds(1, 3, &CANA_tzDO[1][3]);    // Turn OFF all Digital IOs of LHCIO Card-3
            break;

        case 20:
            CANA_tzDO[1][0].all = 0x0;
            CANA_fnMSTxCmds(1, 0, &CANA_tzDO[1][0]);    // Turn OFF all Digital IOs of LHCIO Card-1
            break;

        case 30:
            CANA_tzDO[1][1].all = 0x0;
            CANA_fnMSTxCmds(1, 1, &CANA_tzDO[1][1]);   // Turn OFF all Digital IOs of LHCIO Card-2

            break;

        default:
            break;

        }

        if (ui16IOcnt >= 40)
        {
            ui16IOcnt = 40;
        }
        control_waterloop_stopped();
        break;

        // Run Only Water loop

    case READY:

        if (++ui16IOcnt >= 20)
        {
            ui16IOcnt = 0;

            CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC10_IO, &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC10_IO]);
        }
        control_waterloop();

        break;

        // Check for PRT102 value and Cell Short . If everything is fine after 1 minute goto Stack Power

    case STACK_CHECK:

        if (++ui16IOcnt >= 20)
        {
            ui16IOcnt = 0;
            CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC10_IO, &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC10_IO]);
        }

        control_waterloop();

        if (CANA_tzLHCIO1_AIFaultRegs.bit.PRT_102 == 0) // Check Cell Short Also LAter
        {
            ui16StateRstCnt = 0;
            ui16StateTnstCnt++;
            if (ui16StateTnstCnt > 5000)
            {
                ui16StateTnstCnt = 5000;

                CANB_tzSiteRxRegs.Start_cmd = 3;
                CANA_tzIOflags.faultAtStChkSt = 0;
            }
        }
        else
        {
            ui16StateTnstCnt = 0;
            ui16StateRstCnt++;
            if (ui16StateRstCnt > 1000)
            {
                ui16StateRstCnt = 1000;

                CANB_tzSiteRxRegs.Start_cmd = 2; // Remains in STACK CHECK Only
                CANA_tzIOflags.faultAtStChkSt = 1;

                // To be changed to fault State Later
            }
        }

        break;

        // Remain in Stack Power without Ramping of Current References until SP gives command to Start
        // LATER CAN BE REMOVED _ JUST TO BE SAFE DURING INITIAL TESTING IT IS DONE

    case STACK_POWER:
        ui16IOcnt++;
        if (ui16IOcnt >= 30)
        {
            ui16IOcnt = 0;
            CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC10_IO, &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC10_IO]);
        }
        if(ui16IOcnt == 10)
        {
            CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC11_IO,
                             &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);
        }
        if(ui16IOcnt == 20)
        {
            CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC13_IO,
                             &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC13_IO]);
        }

        control_waterloop();

        break;

    case FAULT:
        ui16IOcnt = 0;

        safeshutDown();
        control_waterloop_stopped();

        break;

    case COMMISSION:
        ui16IOcnt = 0;

        ui16ComsnCnt++;
        if (ui16ComsnCnt >= 10)
        {
            cana_CommisionMode();
            ui16ComsnCnt = 0;
        }

        break;

    default:
        break;
    }

}
/*********************************************************************************/
void CANA_fnMSTxCmds(uint16_t ui16CabiD, uint16_t NodeID,
                     CANA_tzDIG_OP *ptrDigOP)
{

    ui16MPtoMScnt++;
    if(ui16MPtoMScnt>=3)
    {
        ui16MPtoMScnt=0;
    }
    // Master Process to Master Safety
    switch (ui16MPtoMScnt)
    {
    case 1:
            CAN_setupMessageObject(
            CANA_BASE,
                                   CAN_mMAILBOX_17,
                                   CANA_mTX_MSMSGID1,
                                   CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
                                   CAN_MSG_OBJ_NO_FLAGS,
                                   CAN_mLEN8);

            CANA_tzMSRegs.TxCntMS++;

            if (CANA_tzMSRegs.TxCntMS >= 255)
            {
                CANA_tzMSRegs.TxCntMS = 0;
            }

            uiCANtxMsgDataMS[0] = CANA_tzMSRegs.TxCntMS;

            uiCANtxMsgDataMS[1] = STAT_tzStateMac.Present_st;

            uiCANtxMsgDataMS[2] = ptrDigOP->all;

            uiCANtxMsgDataMS[3] = ui16CabiD;
            uiCANtxMsgDataMS[4] = NodeID;

            uiCANtxMsgDataMS[5] = CONTROLtzFaultRegs.bit.LPCCurHealthy;

            uiCANtxMsgDataMS[6] = CANA_tzLHCIO1_AIFaultRegs.all;
            uiCANtxMsgDataMS[7] = CANA_tzLHCIO2_AIFaultRegs.all;

            CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_17, CAN_mLEN8, uiCANtxMsgDataMS);
    break;

  case 2:
            CAN_setupMessageObject(
            CANA_BASE,
                                   CAN_mMAILBOX_17,
                                   CANA_mTX_MSMSGID2,
                                   CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
                                   CAN_MSG_OBJ_NO_FLAGS,
                                   CAN_mLEN8);


            uiCANtxMsgDataMS[0] = CANA_tzLHCIO3_AIFaultRegs.all;;

            uiCANtxMsgDataMS[1] = CANA_tzLHCIO4_AIFaultRegs.all;;

            uiCANtxMsgDataMS[2] = ptrDigOP->all;

            uiCANtxMsgDataMS[3] = ui16CabiD;
            uiCANtxMsgDataMS[4] = NodeID;

            uiCANtxMsgDataMS[5] = 0;

            uiCANtxMsgDataMS[6] = 0;
            uiCANtxMsgDataMS[7] = 0;

            CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_17, CAN_mLEN8, uiCANtxMsgDataMS);
    break;
  default:
      break;
 }
}
/******************************************************************************/
void CANA_fnCmdsForAnaOPVs(uint16_t ui16unitID, uint16_t ui16cab_ID,
                           uint16_t ui16nodeID, can_tzAnaOPParams *ptrAOV)
{

    CAN_setupMessageObject(
            CANA_BASE,
            CAN_mMAILBOX_8,
            (ui16unitID << 8) | (ui16cab_ID << 4) | (ui16nodeID)
                    | CANA_mTX_IOMSGID3,
            CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
            CAN_MSG_OBJ_NO_FLAGS,
            CAN_mLEN8);

    if (ui16cab_ID == 3)
    {
        ui16CabID = 0; // filling LPC Cabinet array
    }
    else if (ui16cab_ID == 1)
    {
        ui16CabID = 1; // filling LHC Cabinet array
    }

    ui16txMsgDataIO[0] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV1) >> 8;
    ui16txMsgDataIO[1] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV1);
    ui16txMsgDataIO[2] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV2) >> 8;
    ui16txMsgDataIO[3] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV2);
    ui16txMsgDataIO[4] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV3) >> 8;
    ui16txMsgDataIO[5] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV3);
    ui16txMsgDataIO[6] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV4) >> 8;
    ui16txMsgDataIO[7] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV4);

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_8, CAN_mLEN8, ui16txMsgDataIO);
}

void CANA_fnCmdsForAnaOPIs(uint16_t ui16unitID, uint16_t ui16cab_ID,
                           uint16_t ui16nodeID, can_tzAnaOPParams *ptrAOI)
{

    CAN_setupMessageObject(
            CANA_BASE,
            CAN_mMAILBOX_9,
            (ui16unitID << 8) | (ui16cab_ID << 4) | (ui16nodeID)
                    | CANA_mTX_IOMSGID2,
            CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
            CAN_MSG_OBJ_NO_FLAGS,
            CAN_mLEN8);

    if (ui16cab_ID == 3)
    {
        ui16CabID = 0; // filling LPC Cabinet array
    }
    else if (ui16cab_ID == 1)
    {
        ui16CabID = 1; // filling LHC Cabinet array
    }

    ui16txMsgDataIO[0] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI1) >> 8;
    ui16txMsgDataIO[1] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI1);
    ui16txMsgDataIO[2] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI2) >> 8;
    ui16txMsgDataIO[3] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI2);
    ui16txMsgDataIO[4] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI3) >> 8;
    ui16txMsgDataIO[5] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI3);
    ui16txMsgDataIO[6] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI4) >> 8;
    ui16txMsgDataIO[7] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI4);

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_9, CAN_mLEN8, ui16txMsgDataIO);
}

void CANA_fnIOHrtBt()
{
//    CANA_tzIOtimers.HrtbtCntIOCom++;
//
//    switch (CANA_tzIOtimers.HrtbtCntIOCom)
//    {
//    case 1:
//
//        CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 3, 0, &CANA_tzDO[0][0]); //LPCIO1
//        break;
//
//    case 2:
//
//        CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 3, 1, &CANA_tzDO[0][1]); //LPCIO2
//        break;
//
//    case 3:
//
//        CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1, 0, &CANA_tzDO[1][0]); //LHCIO1
//        break;
//
//    case 4:
//
//        CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1, 1, &CANA_tzDO[1][1]); //LHCIO2
//        CANA_tzIOtimers.HrtbtCntIOCom = 0;
//
//        break;
//
//    default:
//        break;
//    }
}

void safeshutDown()
{

    CANA_tzMSRegs.ResetIOsInflt++;
    if ((CANA_tzMSRegs.ResetIOsInflt >= 50)
            && (CANA_tzMSRegs.ResetIOsInflt <= 52))
    {

        // Turn Off CTR302 in Fault Mode

        CANA_tzDO[0][1].bit.DO2 = 0x1;  // To be turn Off Later - JTAG gets disconnected
        CANA_tzDO[0][1].bit.DO3 = 0x1;  // To be turn Off Later

        CANA_fnMSTxCmds(3, 1, &CANA_tzDO[0][1]); //Turn ON CTR302
    }

    else if ((CANA_tzMSRegs.ResetIOsInflt >= 53)
            && (CANA_tzMSRegs.ResetIOsInflt <= 54))
    {

        // Turn OFF WSV101
        CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC10_IO].bit.DO0 = 0x0;

        CANB_tzSiteRegs.WaterDemand = 0x0;

        CANA_fnMSTxCmds(1, 0, &CANA_tzDO[1][0]); //Turn OFF WSV101
    }
    else if (CANA_tzMSRegs.ResetIOsInflt >= 55)
    {
        CANA_tzMSRegs.ResetIOsInflt = 0;

    }

    CANA_tzMSRegs.ResetAOsInflt++;
    if (CANA_tzMSRegs.ResetAOsInflt == 50)
    {
        CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV4 = (testCntVFD * 100);

        CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
                              &CANA_tzAnaOPParams); // Control Speed of Purge fan1
    }
    else if (CANA_tzMSRegs.ResetAOsInflt == 100)
    {
        CANA_tzAnaOPParams.CANA_tzAOI[1][0].AOI1 = (testEBV * 10);

        CANA_fnCmdsForAnaOPIs(CANA_tzIORegs.uiUnitID, 1, 0,
                              &CANA_tzAnaOPParams);

    }
    else if (CANA_tzMSRegs.ResetAOsInflt > 100)
    {
        CANA_tzMSRegs.ResetAOsInflt = 0;

    }
}

static float32_t limitAnalogSensorData(float32_t SensorType)
{
    if (SensorType > 21.0f)
    {
        SensorType = 21.0f;
    }
    if (SensorType < 3.0f)
    {
        SensorType = 3.0f;
    }
    return (float32_t) (SensorType);
}

void cana_CommisionMode()
{

    if (CANA_tzQueryType.PSU == QUERY_PROGPARAM)
    {
        CANA_tzQueryType.PSU = SET_VOLT;
    }
    if (ui16manualTesting == 1)
    {

        CANA_tzMSRegs.ManualTestCntMS++;

        if (CANA_tzMSRegs.ManualTestCntMS == 1)
        {

            CANA_fnMSTxCmds(testCabID, testNodeID,
                            &CANA_tzDO[testCabID1][testNodeID]); //Heartbeat
        }
        if (CANA_tzMSRegs.ManualTestCntMS == 10)
        {
            CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV4 = (testCntVFD * 100);

            CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
                                  &CANA_tzAnaOPParams); // Control Speed of Purge fan1
        }
        else if ((CANA_tzMSRegs.ManualTestCntMS == 20))
        {
            CANA_tzAnaOPParams.CANA_tzAOI[1][0].AOI1 = (testEBV * 100);

            CANA_fnCmdsForAnaOPIs(CANA_tzIORegs.uiUnitID, 1, 0,
                                  &CANA_tzAnaOPParams);

        }

        else if (CANA_tzMSRegs.ManualTestCntMS > 21)
        {
            CANA_tzMSRegs.ManualTestCntMS = 0;

        }

    }
}

/************************************************************/
void canA_fnversion(void)
{
    // Version data

    ui16CANATxCntVer++;

    if (ui16CANATxCntVer > 10)
    {
        ui16CANATxCntVer = 1;
    }

    CAN_setupMessageObject(CANA_BASE, CAN_mMAILBOX_32,
                               (0x11F43020 | (CANA_tzIORegs.uiUnitID << 8)),
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN8);
    switch (ui16CANATxCntVer)
    {
        case 1:
            ui16txMsgDataversion[0] = (ui16CANATxCntVer >> 8) & 0xFF;
            ui16txMsgDataversion[1] = ui16CANATxCntVer & 0xFF;
            ui16txMsgDataversion[2] = MAJOR;
            ui16txMsgDataversion[3] = MINOR;
            ui16txMsgDataversion[4] = (BUILD >> 8) & 0xFF;
            ui16txMsgDataversion[5] = BUILD & 0xFF;
            ui16txMsgDataversion[6] = 0;
            ui16txMsgDataversion[7] = 0;

            CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_32, CAN_mLEN8, ui16txMsgDataversion);
            break;
    }

}
/*==============================================================================
 End of File
 ==============================================================================*/
