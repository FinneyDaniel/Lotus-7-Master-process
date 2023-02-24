/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================

 @file  oi_can.h
 @author DEEPTI.K
 @date 06-Sep-2021

 @brief Description of CANA module defs that has CAN FLASHING DATA, MASTER Safety, IO CARD and VS CARD DATA
 ==============================================================================*/

#ifndef MODULE_CANA_COM_INC_CANA_DEFS_H_
#define MODULE_CANA_COM_INC_CANA_DEFS_H_

/*==============================================================================
 Includes
 ==============================================================================*/

#include <module/application/cana_com/inc/cana_defs.h>
#include "F28x_Project.h"
#include "inc/hw_can.h"
#include <stdint.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

/*==============================================================================
 Defines
 ==============================================================================*/

// CAN Flashing Messages

#define CANA_mFLASH_WRITE_FAIL                                     (0x02U)

#define CANA_mFLASH_ERRORBYTE                                      (0xF0U)
#define CANA_mAPP_FLASH_LEN                                        (0x012000)
#define CANA_mRX_BUFFER_SIZE                                       (8U)

#define CAN_mFLASH_ERR_COMM_TIMEOUT                                      (0x0001)
#define CAN_mFLASH_WRITE_FAIL                                            (0x0002)
#define CAN_mFLASH_CHKSUM_MISMATCH                                       (0x0003)
#define CAN_mBYTES                                                       (7U)
#define CAN_mERROR_BYTE                                                  (0xF0)
#define CAN_mBUFFER_SIZE                                                 (224U)

#define CAN_mPOLYNOMIAL                                                 (0x04C11DB7L)
#define APPROX_1_SECOND_DELAY                                           (5000000UL)


// CAN Data length

#define CANA_mONE_BYTE                                             (1U)
#define CANA_mTWO_BYTE                                             (2U)
#define CANA_mTHREE_BYTE                                           (3U)
#define CANA_mFOUR_BYTE                                            (4U)
#define CANA_mFIVE_BYTE                                            (5U)
#define CANA_mSIX_BYTE                                             (6U)
#define CANA_mSEVEN_BYTE                                           (7U)
#define CANA_mEIGHT_BYTE                                           (8U)

// Number of IO Nodes and Voltage Sensing Card Nodes

#define CANA_mTOTAL_IONODE                                              (4U)
#define CANA_mTOTAL_VSNODE                                              (12U)

//

#define CANA_mNODEID_MASKPSU                                             (0x0000FF00U)
#define CANA_mNODEID_MASKIO                                             (0x0000FF00U)
#define CANA_mNODEID_MASKVS                                             (0x0000FF00U)

#define CANA_mIOTIMEOUT                                                 (150U)


#define CANA_mRX_LPCMSGID1                                                 (0x10042130)
#define CANA_mRX_LHCMSGID1                                                 (0x10042110)
#define CANA_mRX_VSCMSGID1                                                 (0x10041110)

#define CANA_mTX_MSMSGID1                                                 (0x10043000)
#define CANA_mTX_MSMSGID2                                                 (0x10043001)

#define CANA_mTX_IOMSGID1                                                 (0x11142000)  // Digital Output
#define CANA_mTX_IOMSGID2                                                 (0x11242000)  // Analog Output Voltage
#define CANA_mTX_IOMSGID3                                                 (0x11342000)  // Analog Output Current

#define CANA_mRX_MSMSGID1                                                 (0x11043000)


#define CANA_mTX_VSC_CELL_CNT                                             (0x11141110)


#define CAN_mMAILBOX_1                                                  (1U)
#define CAN_mMAILBOX_2                                                  (2U)
#define CAN_mMAILBOX_3                                                  (3U)
#define CAN_mMAILBOX_4                                                  (4U)
#define CAN_mMAILBOX_5                                                  (5U)
#define CAN_mMAILBOX_6                                                  (6U)
#define CAN_mMAILBOX_7                                                  (7U)
#define CAN_mMAILBOX_8                                                  (8U)
#define CAN_mMAILBOX_9                                                  (9U)
#define CAN_mMAILBOX_10                                                 (10U)
#define CAN_mMAILBOX_11                                                 (11U)
#define CAN_mMAILBOX_12                                                 (12U)
#define CAN_mMAILBOX_13                                                 (13U)
#define CAN_mMAILBOX_14                                                 (14U)
#define CAN_mMAILBOX_15                                                 (15U)
#define CAN_mMAILBOX_16                                                 (16U)
#define CAN_mMAILBOX_17                                                 (17U)
#define CAN_mMAILBOX_18                                                 (18U)
#define CAN_mMAILBOX_19                                                 (19U)
#define CAN_mMAILBOX_20                                                 (20U)
#define CAN_mMAILBOX_21                                                 (21U)
#define CAN_mMAILBOX_22                                                 (22U)
#define CAN_mMAILBOX_23                                                 (23U)
#define CAN_mMAILBOX_24                                                 (24U)
#define CAN_mMAILBOX_25                                                 (25U)
#define CAN_mMAILBOX_26                                                 (26U)
#define CAN_mMAILBOX_27                                                 (27U)
#define CAN_mMAILBOX_28                                                 (28U)
#define CAN_mMAILBOX_29                                                 (29U)
#define CAN_mMAILBOX_30                                                 (30U)
#define CAN_mMAILBOX_31                                                 (31U)
#define CAN_mMAILBOX_32                                                 (32U)

#define CAN_mLEN1                                                       (1U)
#define CAN_mLEN2                                                       (2U)
#define CAN_mLEN3                                                       (3U)
#define CAN_mLEN4                                                       (4U)
#define CAN_mLEN5                                                       (5U)
#define CAN_mLEN6                                                       (6U)
#define CAN_mLEN7                                                       (7U)
#define CAN_mLEN8                                                       (8U)

// General msg IDs used for msg id creation

#define CANA_mLPC30_IO                                                  (0U)
#define CANA_mLPC31_IO                                                  (1U)
#define CANA_mLHC10_IO                                                  (0U)
#define CANA_mLHC11_IO                                                  (1U)
#define CANA_mLHC12_IO                                                  (2U)
#define CANA_mLHC13_IO                                                  (3U)

#define CANA_mLPC_CABID                                                   (3U)
#define CANA_mLHC_CABID                                                   (1U)



#define    IO_DI_MSGID                          (0U)
#define    IO_AIBLK1_MSGID                      (1U)
#define    IO_AIBLK2_MSGID                      (2U)
#define    IO_THERMAL_MSGID                     (3U)
#define    IO_DI1_FREQ_MSGID                    (4U)
#define    IO_DI2_FREQ_MSGID                    (5U)

#define LPC_30                                 (0U)
#define LPC_31                                 (1U)
#define LHC_10                                 (0U)
#define LHC_11                                 (1U)
#define LHC_12                                 (2U)
#define LHC_13                                 (3U)
#define CANA_mDIG0_SET  (0x01U)
#define CANA_mDIG1_SET  (0x02U)
#define CANA_mDIG2_SET  (0x04U)
#define CANA_mDIG3_SET  (0x80U)
#define CANA_mDIG4_SET  (0x10U)
#define CANA_mDIG5_SET  (0x20U)
#define CANA_mDIG6_SET  (0x40U)
#define CANA_mDIG7_SET  (0x80U)


#define CANA_mDIG0_CLEAR  (0U)
#define CANA_mDIG1_CLEAR  (0U)
#define CANA_mDIG2_CLEAR  (0U)
#define CANA_mDIG3_CLEAR  (0U)
#define CANA_mDIG4_CLEAR  (0U)
#define CANA_mDIG5_CLEAR  (0U)
#define CANA_mDIG6_CLEAR  (0U)
#define CANA_mDIG7_CLEAR  (0U)

#define CANA_mTOTAL_LPCNODES                                                                                   (2U)
#define CANA_mTOTAL_LHCNODES                                                                                   (4U)

#define CANA_mRX_mPSUACDCMSGID                                               (0x11055030)
#define CANA_mRX_mPSUDCDCMSGID                                               (0x11054030)

#define CANA_mTX_mPSUACDCMSGID1                                               (0x10155030)
#define CANA_mTX_mPSUACDCMSGID2                                               (0x10255030)
#define CANA_mTX_mPSUACDCMSGID3                                               (0x10355030)
#define CANA_mTX_mPSUDCDC0MSGID1                                               (0x10154030)
#define CANA_mTX_mPSUDCDC0MSGID2                                               (0x10254030)
#define CANA_mTX_mPSUDCDC0MSGID3                                               (0x10354030)
#define CANA_mTX_mPSUDCDC1MSGID1                                               (0x10154130)
#define CANA_mTX_mPSUDCDC1MSGID2                                               (0x10254130)
#define CANA_mTX_mPSUDCDC1MSGID3                                               (0x10354130)

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/

typedef struct CAN_zRX_CIRCULAR_BUF
{
    uint16_t uiDataBuffer[CANA_mRX_BUFFER_SIZE];
    uint32_t u32_msgID;
    int16_t i_DataLength;

} CAN_tzRX_CIRCULAR_BUF;

typedef struct can_zcirc_buff
{
    CAN_tzRX_CIRCULAR_BUF canA_tzRxC_buffer[100];
    int16_t i_head;
    int16_t i_tail;
    const int16_t i_maxlen;
} can_tzcirc_buff;

typedef struct CANA_zAO_VOLT
{
    uint16_t AOV1;
    uint16_t AOV2;
    uint16_t AOV3;
    uint16_t AOV4;

} CANA_tzAO_VOLT;

typedef struct CANA_zAO_CURR
{
    uint16_t AOI1;
    uint16_t AOI2;
    uint16_t AOI3;
    uint16_t AOI4;

} CANA_tzAO_CURR;

typedef struct can_zAnaOPParams
{
    CANA_tzAO_VOLT CANA_tzAOV[2][2];
    CANA_tzAO_CURR CANA_tzAOI[2][2];


} can_tzAnaOPParams;


 struct IO_tzDIG_OP
{
    uint16_t DO0:1;
    uint16_t DO1:1;
    uint16_t DO2:1;
    uint16_t DO3:1;
    uint16_t DO4:1;
    uint16_t DO5:1;
    uint16_t DO6:1;
    uint16_t DO7:1;

} ;

typedef union
{
    uint16_t all;
   struct IO_tzDIG_OP bit;
}CANA_tzDIG_OP;


struct IO_tzDO
{
   uint16_t DO0:1;
   uint16_t DO1:1;
   uint16_t DO2:1;
   uint16_t DO3:1;
   uint16_t DO4:1;
   uint16_t DO5:1;
   uint16_t DO6:1;
   uint16_t DO7:1;

} ;

typedef union
{
   uint16_t all;
  struct IO_tzDO bit;

}CANA_tzDOs;


typedef struct can_tzDOs
{
    CANA_tzDOs CANA_tzDO[2][4];

} CANA_tzDOREGS;






typedef struct CANA_zAI_IOREGS
{
    float32_t AI0_Data;
    float32_t AI1_Data;
    float32_t AI2_Data;
    float32_t AI3_Data;
    float32_t AI4_Data;
    float32_t AI5_Data;
    float32_t AI6_Data;
    float32_t AI7_Data;
} CANA_tzAI_IOREGS;

typedef struct CANA_zIOREGS
{

    uint16_t Node;

    uint16_t uiMsgtypeLPCIO;
    uint16_t uiUnitID;

    uint16_t uiMsgtypeLHCIO;
    uint16_t uiNodeLPCIO;
    uint16_t uiNodeLHCIO;
    float32_t CJC[CANA_mTOTAL_IONODE];

} CANA_tzIOREGS;

typedef struct CANA_zTIMERIOREGS
{

    uint32_t RxCntLPC30;
    uint32_t RxCntLPC31;
    uint32_t RxCntLHC10;
    uint32_t RxCntLHC11;
    uint32_t RxCntLHC12;
    uint32_t RxCntLHC13;
    uint32_t LPC30ComFailCnt;
    uint32_t LPC31ComFailCnt;

    uint32_t LHC10ComFailCnt;
    uint32_t LHC11ComFailCnt;
    uint32_t LHC12ComFailCnt;
    uint32_t LHC13ComFailCnt;
    uint16_t TxCntIOCom[2][2];
    uint16_t TxCntWaterloop;
    uint16_t HrtbtCntIOCom;



} CANA_tzTIMERIOREGS;

typedef struct CANA_zMSREGS
{

    uint32_t TxCntMS;
    uint32_t HBCntMS;
    uint32_t ManualTestCntMS;


    uint32_t RxCntMS;
    uint16_t uiMsgtype;

    uint16_t fan101SetCnt;
    uint16_t fan101ResetCnt;
    uint16_t fan501SetCnt;
    uint16_t fan501ResetCnt;
    uint16_t fan401SetCnt;
    uint16_t fan401ResetCnt;

    uint16_t PresentStMS;
    uint16_t StartCmd;
    bool btMSComStart;
    uint16_t AOCmd;
    uint32_t MSComFailCnt;
    bool TurnONPurge101;
    bool TurnONPurge501;
    bool TurnONPurge401;
    uint16_t AOVFan101;
    uint16_t AOVFan501;
    uint16_t AOVFan401;

    uint32_t ResetIOsInflt;
    uint32_t ResetAOsInflt;



} CANA_tzMSREGS;

typedef struct CANA_zIOFLAGS
{

    bool btLPC30CommnStart;
    bool btLPC31CommnStart;
    bool btLHC10CommnStart;
    bool btLHC11CommnStart;
    bool btLHC12CommnStart;
    bool btLHC13CommnStart;
    bool faultAtStChkSt;

} CANA_tzIOFLAGS;

struct CANA_tzDI_IO_BITS
{
    uint16_t DI_bit0 :1;
    uint16_t DI_bit1 :1;
    uint16_t DI_bit2 :1;
    uint16_t DI_bit3 :1;
    uint16_t DI_bit4 :1;
    uint16_t DI_bit5 :1;
    uint16_t DI_bit6 :1;
    uint16_t DI_bit7 :1;
};

union CANA_tzDI_IOREGS
{
    uint16_t all;
    struct CANA_tzDI_IO_BITS bit;
};

struct CANA_tzAIFLT_IO_BITS
{
    uint16_t AIFlt_bit0 :1;
    uint16_t AIFlt_bit1 :1;
    uint16_t AIFlt_bit2 :1;
    uint16_t AIFlt_bit3 :1;
    uint16_t AIFlt_bit4 :1;
    uint16_t AIFlt_bit5 :1;
    uint16_t AIFlt_bit6 :1;
    uint16_t AIFlt_bit7 :1;
};

union CANA_tzAIFLT_IOREGS
{
    uint16_t all;
    struct CANA_tzAIFLT_IO_BITS bit;
};

typedef struct CANA_zTHERMAL_IOREGS
{
    float32_t T0_Data;
    float32_t T1_Data;
    float32_t T2_Data;
    float32_t T3_Data;
} CANA_tzTHERMAL_IOREGS;

typedef struct CANA_zAISENSOR_DATA
{
    float32_t HYS_101;
    float32_t HYS_501;
    float32_t HYS_401;
    float32_t HYS_102;
    float32_t OXS_101;
    float32_t LVL_101;
    float32_t PRT_101;
    float32_t PRT_102;
    float32_t COS_101;
    float32_t PRT_401;
    float32_t PRT1;
    float32_t PRT_402;
    float32_t PRT_403;
    float32_t PRT_404;
    float32_t PRT_405;
    float32_t PRT_406;
    float32_t PRT_407;
    float32_t MFM_401;
    float32_t MFM_402;

    float32_t TE_401;
    float32_t DPT_401;
    float32_t RTD_401;
    float32_t RTD_402;
    float32_t RTD_403;
    float32_t RTD_404;

} CANA_tzAISENSOR_DATA;

typedef struct CANA_zDISENSOR_DATA
{
    float32_t PURGE101;
    float32_t PURGE102;
    float32_t PURGE501;
    float32_t PURGE502;
    float32_t PURGE401;
    float32_t PURGE402;

} CANA_tzDISENSOR_DATA;

typedef struct CANA_zTHERMOCOUPLE_DATA
{
    float32_t TTC_101;
    float32_t TTC_102;
    float32_t TTC_301;

    float32_t KTC_401;
    float32_t KTC_402;
    float32_t KTC_801;
    float32_t CJC_Temp;

} CANA_tzTHERMOCOUPLE_DATA;

typedef struct CANA_zDIFREQ_IOREGS
{
    float32_t DI0_FreqData;
    float32_t DI1_FreqData;
    float32_t DI2_FreqData;
    float32_t DI3_FreqData;
    float32_t DI4_FreqData;
    float32_t DI5_FreqData;
    float32_t DI6_FreqData;
    float32_t DI7_FreqData;
} CANA_tzDIFREQ_IOREGS;

struct CANA_tzLPCIO1_DIFLT_IOBITS
{
    uint16_t WLS_101 :1;
    uint16_t WLS_102 :1;
    uint16_t WLS_103 :1;
    uint16_t DOS_301 :1;
    uint16_t DOS_303 :1;
    uint16_t Rsvd4 :1;
};

union CANA_tzLPCIO1_DIFLT_IOREGS
{
    uint16_t all;
    struct CANA_tzLPCIO1_DIFLT_IOBITS bit;
};

struct CANA_tzLPCIO2_DIFLT_IOBITS
{

    uint16_t Rsvd1 :1;
};

union CANA_tzLPCIO2_DIFLT_IOREGS
{
    uint16_t all;
    struct CANA_tzLPCIO2_DIFLT_IOBITS bit;
};


struct CANA_tzLHCIO1_DIFLT_IOBITS
{
    uint16_t DOS101_103 :1;
    uint16_t LGT101_402 :1;
    uint16_t Rsvd3 :1;
    uint16_t Rsvd4 :1;
    uint16_t Rsvd5 :1;
    uint16_t Rsvd6 :1;
    uint16_t Rsvd7 :1;
    uint16_t Rsvd8 :1;
};

union CANA_tzLHCIO1_DIFLT_IOREGS
{
    uint16_t all;
    struct CANA_tzLHCIO1_DIFLT_IOBITS bit;
};

struct CANA_tzLHCIO2_DIFLT_IOBITS
{
    uint16_t Rsvd1 :1;
    uint16_t Rsvd2 :1;
    uint16_t Rsvd3 :1;
    uint16_t Rsvd4 :1;
    uint16_t Rsvd5 :1;
    uint16_t Rsvd6 :1;
    uint16_t Rsvd7 :1;
    uint16_t Rsvd8 :1;
};

union CANA_tzLHCIO2_DIFLT_IOREGS
{
    uint16_t all;
    struct CANA_tzLHCIO2_DIFLT_IOBITS bit;
};


typedef struct CANA_zDO_IOREGS
{
    uint16_t DO_bit0;
    uint16_t DO_bit1;
    uint16_t DO_bit2;
    uint16_t DO_bit3;
    uint16_t DO_bit4;
    uint16_t DO_bit5;
    uint16_t DO_bit6;
    uint16_t DO_bit7;

}CANA_tzDO_IOREGS;




struct CANA_tzAO_IO_BITS

{

};


struct CANA_tzLPCIO1_AIFLT_IOBITS
{
    uint16_t Rsvd1 :1;
};

union CANA_tzLPCIO1_AIFLT_IOREGS
{
    Uint16 all;
    struct CANA_tzLPCIO1_AIFLT_IOBITS bit;
};

struct CANA_tzLPCIO2_AIFLT_IOBITS
{
    uint16_t HYS_101 :1;
    uint16_t HYS_501 :1;
    uint16_t HYS_401 :1;
    uint16_t HYS_102_ShtDwn :1;
    uint16_t OXS_101_ShtDwn :1;
    uint16_t HYS_102_RmpDwn :1;
    uint16_t OXS_101_RmpDwn :1;
    uint16_t HYS_NotOk :1;
};

union CANA_tzLPCIO2_AIFLT_IOREGS
{
    Uint16 all;
    struct CANA_tzLPCIO2_AIFLT_IOBITS bit;
};

struct CANA_tzLHCIO1_AIFLT_IOBITS
{
    uint16_t LVL_101 :1;
    uint16_t PRT_101 :1;
    uint16_t PRT_102 :1;
    uint16_t COS_101 :1;
    uint16_t Rsvd1 :1;
    uint16_t Rsvd2 :1;
    uint16_t Rsvd3 :1;
    uint16_t Rsvd4 :1;
};

union CANA_tzLHCIO1_AIFLT_IOREGS
{
    Uint16 all;
    struct CANA_tzLHCIO1_AIFLT_IOBITS bit;
};


struct CANA_tzLHCIO2_AIFLT_IOBITS
{
    uint16_t PRT_401 :1;
    uint16_t Rsvd0 :1;
    uint16_t TE_401 :1;
    uint16_t DPT_401 :1;
    uint16_t Rsvd1 :1;
    uint16_t Rsvd2 :1;
    uint16_t Rsvd3 :1;
    uint16_t Rsvd4 :1;
};

union CANA_tzLHCIO2_AIFLT_IOREGS
{
    Uint16 all;
    struct CANA_tzLHCIO2_AIFLT_IOBITS bit;
};
/*--------------------------------------*/
struct CANA_tzLHCIO3_AIFLT_IOBITS
{
    uint16_t PRT_402 :1;
    uint16_t PRT_403 :1;
    uint16_t PRT_404 :1;
    uint16_t PRT_405 :1;
    uint16_t PRT_406 :1;
    uint16_t PRT_407 :1;
    uint16_t MFM_401 :1;
    uint16_t MFM_402 :1;
};

union CANA_tzLHCIO3_AIFLT_IOREGS
{
    Uint16 all;
    struct CANA_tzLHCIO3_AIFLT_IOBITS bit;
};

struct CANA_tzLHCIO4_AIFLT_IOBITS
{
    uint16_t RTD_401 :1;
    uint16_t RTD_402 :1;
    uint16_t RTD_403 :1;
    uint16_t RTD_404 :1;
    uint16_t Rsvd1 :1;
    uint16_t Rsvd2 :1;
    uint16_t Rsvd3 :1;
    uint16_t Rsvd4 :1;
};

union CANA_tzLHCIO4_AIFLT_IOREGS
{
    Uint16 all;
    struct CANA_tzLHCIO4_AIFLT_IOBITS bit;
};
struct CANA_tzTHERMALFLT_BITS
{
    uint16_t TTC_101 :1;
    uint16_t TTC_102 :1;
    uint16_t TTC_301 :1;
    uint16_t KTC_401 :1;
    uint16_t Rsvd1 :1;
    uint16_t Rsvd2 :1;
    uint16_t Rsvd3 :1;
    uint16_t Rsvd4 :1;

};

union CANA_tzTHERMALFLT_IOREGS
{
    uint16_t all;
    struct CANA_tzTHERMALFLT_BITS bit;
};

struct CANA_tzACTIVE_IONODE_BITS
{
    uint16_t bt_LPC30 :1;
    uint16_t bt_LPC31 :1;
    uint16_t bt_LHC10 :1;
    uint16_t bt_LHC11 :1;
    uint16_t bt_LHC12 :1;
    uint16_t bt_LHC13 :1;
    uint16_t bt_MS :1;
    uint16_t CellFault :1;

};

union CANA_tzACTIVE_IONODE_REGS
{
    Uint16 all;
    struct CANA_tzACTIVE_IONODE_BITS bit;
};

struct CANA_tzACTIVE_VSNODE_BITS
{
    uint16_t bt_VS1 :1;
    uint16_t bt_VS2 :1;
    uint16_t bt_VS3 :1;
    uint16_t bt_VS4 :1;
    uint16_t bt_VS5 :1;
    uint16_t bt_VS6 :1;
    uint16_t bt_VS7 :1;
    uint16_t bt_VS8 :1;
};

union CANA_tzACTIVE_VSNODE_REGS
{
    Uint16 all;
    struct CANA_tzACTIVE_VSNODE_BITS bit;
};

struct CANA_tzACTIVE_VS1NODE_BITS
{
    uint16_t bt_VS9 :1;
    uint16_t bt_VS10 :1;
    uint16_t bt_VS11 :1;
    uint16_t bt_VS12 :1;
    uint16_t bt_VS13 :1;
    uint16_t bt_VS14 :1;
    uint16_t bt_VS15 :1;
    uint16_t bt_VS16 :1;
};

union CANA_tzACTIVE_VS1NODE_REGS
{
    Uint16 all;
    struct CANA_tzACTIVE_VS1NODE_BITS bit;
};

struct CANA_tzMSDO_STATUS_BITS
{
    uint16_t fan101_102 :1;
    uint16_t purg501_502 :1;
    uint16_t purg401_402 :1;
    uint16_t LHC_dc :1;
    uint16_t LHC_ac :1;
    uint16_t PSU_CONT :1;
 };

union CANA_tzDOMS_STATUS_REGS
{
    uint16_t all;
    struct CANA_tzMSDO_STATUS_BITS bit;
};


struct CANA_tzMPFLT_BITS
{
    uint16_t fan101_102 :1;
       uint16_t fan501_502 :1;
       uint16_t fan401_402 :1;
       uint16_t DOS101_103 :1;
       uint16_t DOS301_303 :1;
       uint16_t LVL_101 :1;
       uint16_t PRT_102 :1;
       uint16_t PRT_401 :1;
 };

union CANA_tzMP_FAULTS_REGS
{
    uint16_t all;
    struct CANA_tzMPFLT_BITS bit;
};
struct CANA_tzMSLHCIO1_DIFLT_BITS
{
    uint16_t DOS101_103 :1;
    uint16_t LGT101_402 :1;
    uint16_t Rsvd3 :1;
    uint16_t Rsvd4 :1;
    uint16_t Rsvd5 :1;
    uint16_t Rsvd6 :1;
    uint16_t Rsvd7 :1;
    uint16_t Rsvd8 :1;
 };

union CANA_tzMS_LHCIO1DIFAULTS_REGS
{
    uint16_t all;
    struct CANA_tzMSLHCIO1_DIFLT_BITS bit;
};

/************** cmds from PCAN to test PSU **********************/
typedef struct CANA_zMP_PSU_debugREGS
{

      uint16_t uiMsgtype;
      uint16_t ACDC_on_cmd;
      uint16_t ACDC_node_no;
      uint16_t ACDC_reset_cmd;


} CANA_tzMP_PSU_debugREGS;

/*============================================================================
 Macros
 ==============================================================================*/

#define CIRC_BUF_DEF(x,y)                \
    can_tzcirc_buff x = {                 \
        .i_head = 0,                      \
        .i_tail = 0,                      \
        .i_maxlen = y                     \
    }



/*==============================================================================
 Extern/Public Function Prototypes
 ==============================================================================*/

extern void CANA_FlashEvent(void);
extern void CANA_fnInitMBox(void);
extern void CANA_fnRXevent(void);
extern void CANA_fnTask(void);
extern void faultCheck(void);
extern void CANA_fnTx(void);




/*==============================================================================
 Extern/Public Variables
 ==============================================================================*/

extern uint16_t ui16StateTnstCnt,ui16StateRstCnt;


extern CANA_tzIOREGS CANA_tzIORegs;
extern CANA_tzTIMERIOREGS CANA_tzIOtimers;
extern CANA_tzIOFLAGS CANA_tzIOflags;

extern CANA_tzMSREGS CANA_tzMSRegs;
extern CANA_tzMP_PSU_debugREGS CANA_tzMP_PSU_DebgRegs;


extern CANA_tzTHERMAL_IOREGS CANA_tzThermal_IORegs[CANA_mTOTAL_IONODE];
extern CANA_tzAISENSOR_DATA CANA_tzAISensorData;
extern CANA_tzDISENSOR_DATA CANA_tzDISensorData;

extern CANA_tzTHERMOCOUPLE_DATA CANA_tzThermoCoupleData;
extern CANA_tzDIFREQ_IOREGS CANA_tzAIDataFreq_IORegs[CANA_mTOTAL_IONODE];
extern can_tzAnaOPParams CANA_tzAnaOPParams;
extern CANA_tzDO_IOREGS CANA_tzSetDO_IORegs;
extern CANA_tzDIG_OP CANA_tzDO[2][4];

extern CANA_tzDOREGS CANA_tzDOParams;

extern union CANA_tzDI_IOREGS CANA_tzLPCDI_IORegs[CANA_mTOTAL_LPCNODES],
        CANA_tzLHCDI_IORegs[CANA_mTOTAL_LHCNODES];
extern union CANA_tzAIFLT_IOREGS CANA_tzLPCAIFlt_IORegs[CANA_mTOTAL_IONODE],
        CANA_tzLHCAIFlt_IORegs[CANA_mTOTAL_IONODE];

extern union CANA_tzLPCIO1_DIFLT_IOREGS CANA_tzLPCIO1_DIFaultRegs;
extern union CANA_tzLPCIO2_DIFLT_IOREGS CANA_tzLPCIO2_DIFaultRegs;
extern union CANA_tzLHCIO1_DIFLT_IOREGS CANA_tzLHCIO1_DIFaultRegs;
extern union CANA_tzLHCIO2_DIFLT_IOREGS CANA_tzLHCIO2_DIFaultRegs;


extern union CANA_tzLPCIO1_AIFLT_IOREGS CANA_tzLPCIO1_AIFaultRegs;
extern union CANA_tzLPCIO2_AIFLT_IOREGS CANA_tzLPCIO2_AIFaultRegs;
extern union CANA_tzLHCIO1_AIFLT_IOREGS CANA_tzLHCIO1_AIFaultRegs;
extern union CANA_tzLHCIO2_AIFLT_IOREGS CANA_tzLHCIO2_AIFaultRegs;
extern union CANA_tzLHCIO3_AIFLT_IOREGS CANA_tzLHCIO3_AIFaultRegs;
extern union CANA_tzLHCIO4_AIFLT_IOREGS CANA_tzLHCIO4_AIFaultRegs;
extern union CANA_tzTHERMALFLT_IOREGS CANA_tzThermalFaultRegs;
extern union CANA_tzACTIVE_IONODE_REGS CANA_tzActNodeRegs_IO;

extern union CANA_tzACTIVE_VSNODE_REGS CANA_tzActNodeRegs_VS;
extern union CANA_tzACTIVE_VS1NODE_REGS CANA_tzActNodeRegs_VS1;


extern union CANA_tzDOMS_STATUS_REGS CANA_tzActMS_DOStRegs;

extern union CANA_tzMP_FAULTS_REGS CANA_tzActMS_FaultRegs;

extern union CANA_tzLPCAIFLT_IOREGS CANA_tzLPCAIFaultRegs[CANA_mTOTAL_LPCNODES];
extern union CANA_tzLHCAIFLT_IOREGS CANA_tzLHCAIFaultRegs[CANA_mTOTAL_LHCNODES];

extern union CANA_tzMS_LHCIO1DIFAULTS_REGS CANA_tzActMS_LHCIO1_DIFaultRegs;



extern bool can_fnEnquedata(can_tzcirc_buff *ptr, uint16_t *data, uint32_t msgID,
                     uint16_t DLC);
extern bool can_fndequedata(can_tzcirc_buff *ptr, uint16_t *data, uint32_t *msgID,
                     uint16_t *DLC);



extern void CANA_fnMSTxCmds(uint16_t ui16CabiD, uint16_t NodeID, CANA_tzDIG_OP  *ptrDigOP);


extern void CANA_fnCmdsForAnaOPVs(uint16_t ui16unitID, uint16_t ui16cabinetID, uint16_t ui16nodeID,
                           can_tzAnaOPParams *ptrAO_V);

extern void CANA_fnCmdsForAnaOPIs(uint16_t ui16unitID, uint16_t ui16cabinetID, uint16_t ui16nodeID,
                           can_tzAnaOPParams *ptrAO_I);

extern void CANA_fnIOHrtBt();

extern void cana_fnVSCTx(void);
extern void CANA_fnComFailChk(void);

extern CANA_tzAI_IOREGS CANA_tzAIData_LPCIORegs[CANA_mTOTAL_LPCNODES],CANA_tzAIData_LHCIORegs[CANA_mTOTAL_LHCNODES];
extern void CANA_fnPSU_ACDC_TxCmds();
/*==============================================================================
 Extern/Public Constants
 ==============================================================================*/

#endif /* MODULE_CANA_COM_INC_CANA_DEFS_H_ */
