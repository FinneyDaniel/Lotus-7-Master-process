/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  cana_PSUCom.h
 @author DEEPTI.K
 @date 01-Jul-2022

 @brief Description
 ==============================================================================*/
#ifndef MODULE_APPLICATION_CANA_PSUCOM_C_INC_CANA_PSUCOM_H_
#define MODULE_APPLICATION_CANA_PSUCOM_C_INC_CANA_PSUCOM_H_

/*==============================================================================
 Includes
 ==============================================================================*/

#include "F28x_Project.h"
#include "inc/hw_can.h"
#include <stdint.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

/*==============================================================================
 Defines
 ==============================================================================*/

// General msg IDs used for msg id creation
#define CANA_mBASEID_FOR_TRANSMIT_PSU                                                                        (0x10000000U)
#define CANA_mBASEID_FOR_PRODUCT_PSU                                                                         (0x01000000U)
#define CANA_mBASEID_FOR_FAMILY_PSU                                                                          (0x00100000U)
#define CANA_mBASEID_FOR_MODEL_PSU                                                                           (0x000B0000U)
#define CANA_mBASEID_FOR_GLOBAL_TURNON_PSU                                                                   (0x00000004U)
#define CANA_mBASEID_FOR_GLOBAL_VOLTSET_PSU                                                                  (0x00000005U)
#define CANA_mBASEID_FOR_GLOBAL_CURRSET_PSU                                                                  (0x00000006U)

#define CANA_mBASEID_FOR_MEASPARAMS_PSU                                                                      (0x00000020U)
#define CANA_mBASEID_FOR_PROGPARAMS_PSU                                                                      (0x00000020U)
#define CANA_mBASEID_FOR_FAULTS_PSU                                                                          (0x00000020U)
#define CANA_mBASEID_FOR_ACPARAMS_PSU                                                                        (0x00000020U)
#define CANA_mBASEID_FOR_IMP_PSU                                                                             (0x00000015U)

#define CANA_mMSGID_GLOBAL_COMMAND_TO_TURNON_PSU                                                             (CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU +  CANA_mBASEID_FOR_GLOBAL_TURNON_PSU)
#define CANA_mMSGID_GLOBAL_COMMAND_TO_VOLTSET_PSU                                                            (CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU +  CANA_mBASEID_FOR_GLOBAL_VOLTSET_PSU)
#define CANA_mMSGID_GLOBAL_COMMAND_TO_CURRSET_PSU                                                            (CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU +  CANA_mBASEID_FOR_GLOBAL_CURRSET_PSU)

#define CANA_mMSGID_QRY_MODELSPC_MEASPARAM_PSU                                                              (CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU + CANA_mBASEID_FOR_MODEL_PSU + CANA_mBASEID_FOR_MEASPARAMS_PSU)
#define CANA_mMSGID_QRY_MODELSPC_PROGPARAM_PSU                                                              (CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU + CANA_mBASEID_FOR_MODEL_PSU + CANA_mBASEID_FOR_PROGPARAMS_PSU)
#define CANA_mMSGID_QRY_MODELSPC_FAULTS_PSU                                                                 (CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU + CANA_mBASEID_FOR_MODEL_PSU + CANA_mBASEID_FOR_FAULTS_PSU)
#define CANA_mMSGID_QRY_MODELSPC_ACPARAM_PSU                                                                (CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU + CANA_mBASEID_FOR_MODEL_PSU + CANA_mBASEID_FOR_ACPARAMS_PSU)
#define CANA_mMSGID_QRY_MODELSPC_IMPSET_PSU                                                                 (CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU + CANA_mBASEID_FOR_MODEL_PSU + CANA_mBASEID_FOR_IMP_PSU)

#define CANA_mTURNOFF_DCDC                                                                                  (0x0)
#define CANA_mTURNON_DCDC                                                                                   (0xAA)
#define CANA_mTURNON_FEC                                                                                    (0xBB)
#define CANA_mTOTAL_PSUNODE                                                                                 (27U)
#define CANA_mPSUTIMEOUT                                                                                    (150U)

#define CANA_mTURNOFF_ACDC                                                                                  (0x55)
#define CANA_mTURNON_ACDC                                                                                   (0xAA)
#define CANA_mRESETON_DCDC                                                                                  (0xBB)
#define CANA_mRESETOFF_DCDC                                                                                 (0x55)
#define CANA_mRESETON_ACDC                                                                                  (0xBB)
#define CANA_mRESETOFF_ACDC                                                                                 (0x55)

#define CANA_mTURNON_FEC                                                                                    (0xBB)
#define CANA_mTOTAL_PSUNODE                                                                                 (27U)
#define CANA_mPSUTIMEOUT                                                                                    (150U)
#define CANA_mTOT_ACDCPSUNODE                                                                               (9U)
#define CANA_mTOT_DCDCPSUNODE                                                                               (9U)
#define CANA_mNo_Q                                                                                          (0x0)
#define CANA_mPercent_Q                                                                                     (0x1)
#define CANA_mPF_Q                                                                                          (0x2)
#define CANA_mLAG                                                                                           (0x0)
#define CANA_mLEAD                                                                                          (0x1)

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/
typedef struct CANA_zPSUDATA
{
    uint16_t uiStart_cmd;
    float32_t f32SetVolt;
    uint16_t uiVoltInt[CANA_mTOTAL_PSUNODE + 1];
    uint16_t uiVoltFrac[CANA_mTOTAL_PSUNODE + 1];
    float32 f32OutVolt[CANA_mTOTAL_PSUNODE + 1];
    uint16_t uiCurrInt[CANA_mTOTAL_PSUNODE + 1];
    uint16_t uiCurrFrac[CANA_mTOTAL_PSUNODE + 1];
    float32 f32OutCurr[CANA_mTOTAL_PSUNODE + 1];
    uint16_t uiProgVoltInt[CANA_mTOTAL_PSUNODE + 1];
    uint16_t uiProgVoltFrac[CANA_mTOTAL_PSUNODE + 1];
    float32 f32ProgVolt[CANA_mTOTAL_PSUNODE + 1];
    uint16_t uiProgCurrInt[CANA_mTOTAL_PSUNODE + 1];
    uint16_t uiProgCurrFrac[CANA_mTOTAL_PSUNODE + 1];
    float32_t f32ProgCurr[CANA_mTOTAL_PSUNODE + 1];
    float32_t ACparamFreq[CANA_mTOTAL_PSUNODE + 1];
    float32 AClineVolt[CANA_mTOTAL_PSUNODE + 1];
    float32 AClineCurr[CANA_mTOTAL_PSUNODE + 1];

} CANA_tzPSUDATA;

typedef struct CAN_zRXDRegs
{
    CANA_tzPSUDATA tzPSUData;

} CANA_tzRXDRegs;

typedef struct CANA_zPSUTXDATA
{
    float32_t VoltageSet;
    float32_t CurrentSet;
    float32_t TotalCurrentSet;
    float32_t TotalISetTemp;

} CANA_tzPSUTXDATA;

typedef struct CANA_zTXDRegs
{
    CANA_tzPSUTXDATA tzPSUData;

} CANA_tzTXDRegs;

typedef struct CANA_zPSURegs
{

    uint16_t MsgID1;
    uint16_t MsgID2;
    uint16_t MsgID3;
    uint16_t MsgID4;

    uint16_t MBox5;
    uint16_t MBox6;
    uint16_t MBox7;
    uint16_t MBox8;
    uint16_t MBox21;
    uint16_t MBox22;
    uint16_t MBox23;

    uint16_t Node;
    uint16_t HiNode;

} CANA_tzPSURegs;

// ENUMS
typedef enum
{
    IDLE_PSU,
    SET_VOLT,
    SET_CURR,
    QUERY_PROGPARAM,
    TURN_ON,
    TURN_ONFEC,
    TURN_OFF,
    QUERY_OP_PARAM,
    QUERY_FLTS,
    QUERY_ACPARAMS,
    SAVE_SETTINGS,
    IMPEDANCE_MODE,
    LOCAL_REMOTE

} CANA_tzPSUQUERY;

typedef struct
{
    CANA_tzPSUQUERY PSU;

} CANA_tzQUERYTYPE;

// Active Node Register
struct CANA_tzACTNODE_BITS
{
    uint16_t bt_rsvd :1;        // Bit 0
    uint16_t bt_node1 :1;       // Bit 1
    uint16_t bt_node2 :1;       // Bit 2
    uint16_t bt_node3 :1;       // Bit 3
    uint16_t bt_node4 :1;       // Bit 4
    uint16_t bt_node5 :1;       // Bit 5
    uint16_t bt_node6 :1;       // Bit 6
    uint16_t bt_node7 :1;       // Bit 7
    uint16_t bt_node8 :1;       // Bit 8
    uint16_t bt_node9 :1;       // Bit 9
    uint16_t bt_node10 :1;       // Bit 10
    uint16_t bt_node11 :1;       // Bit 11
    uint16_t bt_node12 :1;       // Bit 12
    uint16_t bt_node13 :1;       // Bit 13
    uint16_t bt_node14 :1;       // Bit 14
    uint16_t bt_node15 :1;       // Bit 15
};

struct CANA_tzACTNODE1_BITS
{
    uint16_t bt_rsvd1 :1;       // Bit 16
    uint16_t bt_node16 :1;       // Bit 17
    uint16_t bt_node17 :1;       // Bit 18
    uint16_t bt_node18 :1;       // Bit 19
    uint16_t bt_node19 :1;       // Bit 20
    uint16_t bt_node20 :1;       // Bit 21
    uint16_t bt_node21 :1;       // Bit 22
    uint16_t bt_node22 :1;       // Bit 23
    uint16_t bt_node23 :1;       // Bit 24
    uint16_t bt_node24 :1;       // Bit 25
    uint16_t bt_node25 :1;       // Bit 26
    uint16_t bt_node26 :1;       // Bit 27
    uint16_t bt_node27 :1;       // Bit 28
    uint16_t bt_node28 :1;       // Bit 29
    Uint16 bt_node29 :1;       // Bit 30
    Uint16 bt_node30 :1;       // Bit 31

};

union CANA_tzACTNODE_REGS
{
    uint16_t all;
    struct CANA_tzACTNODE_BITS bit;
};

union CANA_tzACTNODE1_REGS
{
    uint16_t all;
    struct CANA_tzACTNODE1_BITS bit;
};

// Fault RegisterDCDC
struct CANA_tzFAULT_DCDCBITS
{
    uint16_t bt_AC_Fault :1;                           // Bit 0
    uint16_t bt_Otp_Fault :1;                          // Bit 1
    uint16_t bt_Fold_Fault :1;                         // Bit 2
    uint16_t bt_Ovp_Fault :1;                          // Bit 3
    uint16_t bt_ShutOff_Fault :1;                      // Bit 4
    uint16_t bt_Out_Fault :1;                          // Bit 5
    uint16_t bt_Interlock_Fault :1;                    // Bit 6
    uint16_t bt_Fan_Fault :1;                          // Bit 7
    uint16_t bt_Reserved :1;                           // Bit 8
    uint16_t bt_PFC_DC_Bus_Fault :1;                   // Bit 9
    uint16_t bt_Internal_Communication_Fault :1;       // Bit 10   I2C, SPI
    uint16_t bt_Eeprom_Checksum_Failure :1;            // Bit 11
    uint16_t bt_On_Board_Adc_Failure :1;               // Bit 12
    uint16_t bt_Illegal_Math_Performed :1;             // Bit 13
    uint16_t bt_Reserved_2 :1;                         // Bit 14
    uint16_t bt_Reserved_3 :1;                         // Bit 15
};

union CANA_tzFAULT_DCDCREGS
{
    Uint16 all;
    struct CANA_tzFAULT_DCDCBITS bit;
};

/**
 * @brief structure to store fault register-PFC
 */
struct CANA_tzFAULT_FECHBITS
{
    uint32_t dc_bus_overcurrent :1;  // bit8
    uint32_t grid_current_violation :1; // bit9
    uint32_t inst_grid_rstcurrent_violation :1; // bit10
    uint32_t system_fault :1; // bit11
    uint32_t over_temp_voilation :1; // bit12
    uint32_t freq_out_of_Range :1; // bit13
    uint32_t inputphase_missing :1; // bit14
    uint32_t healthStatus :1;      // bit15
};

union CANA_tzFAULT_FECHREGS
{
    Uint16 all;
    struct CANA_tzFAULT_FECHBITS bit;
};

struct CANA_tzFAULT_FECLBITS
{
    uint32_t grid_ac_undervolt :1;    // bit0
    uint32_t hardware_fault :1;       // bit1
    uint32_t grid_ac_overvolt :1;     // bit2
    uint32_t pll_nosync :1;  // bit3
    uint32_t dc_bus_overvolt :1;      // bit4
    uint32_t dc_bus_rstovervolt :1;   // bit5
    uint32_t dc_bus_undervolt :1;     // bit6
    uint32_t dc_bus_undervolt1 :1;   // bit7
};

union CANA_tzFAULT_FECLREGS
{
    Uint16 all;
    struct CANA_tzFAULT_FECLBITS bit;
};

typedef struct CANA_zPSUTIMER
{
    uint16_t secondCnt;
    uint16_t TxCount;
    uint16_t TxManCount;

    uint16_t TxCntPSUCmds;
    uint16_t RxCntPSUCmds;
    uint16_t CANfailCnt[CANA_mTOTAL_PSUNODE + 1];
    uint16_t CurRampUpcnt;
    uint16_t CurRampDowncnt;
    uint16_t InstShutDowncnt;
    uint16_t SiteCommandRUT_ACK;
    uint16_t ComsnCnt;


} CANA_tzPSUTIMER;

typedef struct CANA_zTIMERRegs
{
    CANA_tzPSUTIMER tzPSU;

} CANA_tzTIMERRegs;
/*********************************FOR MK2 ***************************/
 typedef struct CANA_zPSUACDC
{
   uint16_t uiNodeID;
   uint16_t uiMsgType;
   uint16_t uiMsgIDL;

}CANA_tzPSUACDC;

typedef struct CANA_zPSUDCDC
{
  uint16_t uiNodeID;
  uint16_t uiMsgType;
  uint16_t uiMsgIDL;
  uint16_t uiUnitID;
}CANA_tzPSUDCDC;


struct CANA_tzACDC_FLTOthBITS
{
    uint16_t bt0_OF1_flt:1;
    uint16_t bt1_OF2_flt:1;
    uint16_t bt2_OF3_flt:1;
    uint16_t bt3_OF4_flt:1;
    uint16_t bt4_OF5_flt:1;
    uint16_t bt5_OF6_flt:1;
    uint16_t bt6_OF7_flt:1;
    uint16_t bt7_OF8_flt:1;
};

union CANA_tzACDC_FLTOthREGS
{
    Uint16 all;
    struct CANA_tzACDC_FLTOthBITS bit;
};

struct CANA_tzDCDC_FLTOthLBITS
{
    uint16_t bt0_OF1_flt:1;
    uint16_t bt1_OF2_flt:1;
    uint16_t bt2_OF3_flt:1;
    uint16_t bt3_OF4_flt:1;
    uint16_t bt4_OF5_flt:1;
    uint16_t bt5_OF6_flt:1;
    uint16_t bt6_OF7_flt:1;
    uint16_t bt7_OF8_flt:1;
};

union CANA_tzDCDC_FLTOthLREGS
{
    Uint16 all;
    struct CANA_tzDCDC_FLTOthLBITS bit;
};
struct CANA_tzDCDC_FLTOthHBITS
{
    uint16_t bt0_OF1_flt:1;
    uint16_t bt1_OF2_flt:1;
    uint16_t bt2_OF3_flt:1;
    uint16_t bt3_OF4_flt:1;
    uint16_t bt4_OF5_flt:1;
    uint16_t bt5_OF6_flt:1;
    uint16_t bt6_OF7_flt:1;
    uint16_t bt7_OF8_flt:1;
};

union CANA_tzDCDC_FLTOthHREGS
{
    Uint16 all;
    struct CANA_tzDCDC_FLTOthHBITS bit;
};
struct CANA_tzACDC_FLTHWBITS
{
    uint16_t bt0_PrechargeError:1;
    uint16_t bt1_ACContactorError:1;
    uint16_t bt2_GateDriverError:1;
    uint16_t bt3_HWGridCurrViolation:1;
    uint16_t bt4_HWDCCurrViolation:1;
    uint16_t bt5_HWDCVoltViolation:1;
    uint16_t bt6_Reserved:1;
    uint16_t bt7_Reserved:1;
};

union CANA_tzACDC_FLTHWREGS
{
    Uint16 all;
    struct CANA_tzACDC_FLTHWBITS bit;
};
struct CANA_tzACDC_FLTSWLBITS
{
    uint16_t bt0_GridUVFault:1;
    uint16_t bt1_GridOVFault:1;
    uint16_t bt2_GridFreqOutOfRange:1;
    uint16_t bt3_PLLSyncError:1;
    uint16_t bt4_GridVoltUnbalance:1;
    uint16_t bt5_DCBusUVFault:1;
    uint16_t bt6_DCBusOVFault:1;
    uint16_t bt7_DCBusOCFault:1;
};

union CANA_tzACDC_FLTSWLREGS
{
    Uint16 all;
    struct CANA_tzACDC_FLTSWLBITS bit;
};

struct CANA_tzACDC_FLTSWHBITS
{
    uint16_t bt0_IGBTOverTempFault:1;
    uint16_t bt1_AmbOverTempFault:1;
    uint16_t bt2_FWGridCurrViolation:1;
    uint16_t bt3_FanFailFault:1;
    uint16_t bt4_CANCommError:1;
    uint16_t bt5_I2CCommError:1;
    uint16_t bt6_SystemFault:1;
    uint16_t bt7_LVRTTimeError:1;
};

union CANA_tzACDC_FLTSWHREGS
{
    Uint16 all;
    struct CANA_tzACDC_FLTSWHBITS bit;
};

typedef struct CANA_zPSUDATA_ACDC
{
    float32 f32Grid_Volt;
    float32 f32Grid_Curr;
    float32 f32Grid_Freq;
    uint16_t ui16Grid_Power_watt;
    float32 f32DCparam_VDC;
    float32 f32DCparam_IDC;
    uint16_t ui16DCparam_Power_watt;
    uint16_t uiDCparam_status;
    uint16_t uiDCparam_version;
    uint16_t uiTemp_P1;
    uint16_t uiTemp_P2;
    uint16_t uiTemp_P3;
    uint16_t uiTemp_A;
    uint16_t uiDerating_fact;

} CANA_tzPSUDATA_ACDC;

typedef struct CAN_zRXDRegsPSU_ACDC
{
    CANA_tzPSUDATA_ACDC tzPSUDataACDC;
    uint16_t isReceived;
    uint16_t CANfailCnt;


} CANA_tzRXDRegsPSU_ACDC;
/****************** PSU ACDC TX CMDS ****************************/
typedef struct CANA_zTxPSUDATA_ACDC
{
    uint16_t uiON_cmd[CANA_mTOT_ACDCPSUNODE + 1];
    uint16_t uiReset_cmd[CANA_mTOT_ACDCPSUNODE + 1];
    uint32_t ui32Vdc_RefSet[CANA_mTOT_ACDCPSUNODE + 1];
    uint16_t uiQ_cmd[CANA_mTOT_ACDCPSUNODE + 1];
    uint16_t uiQ_dir[CANA_mTOT_ACDCPSUNODE + 1];
    uint32_t ui32Q_percent[CANA_mTOT_ACDCPSUNODE + 1];
    uint32_t ui32PF[CANA_mTOT_ACDCPSUNODE + 1];
} CANA_tzTxPSUDATA_ACDC;

typedef struct CAN_zTXDRegsPSU_ACDC
{
    CANA_tzTxPSUDATA_ACDC tzPSUTxDataACDC;

} CANA_tzTXDRegsPSU_ACDC;
/****************** PSU DCDC TX CMDS ****************************/
typedef struct CANA_zTxPSUDATA_DCDC
{
    uint16_t uiON_cmd[CANA_mTOT_DCDCPSUNODE + 1];
    uint16_t uiReset_cmd[CANA_mTOT_DCDCPSUNODE + 1];
    uint32_t ui32Vdc_RefSet[CANA_mTOT_DCDCPSUNODE + 1];
    float32 f32Idc_RefSet[CANA_mTOT_DCDCPSUNODE + 1];
    uint16_t uiImpedanceSpc_mode[CANA_mTOT_DCDCPSUNODE + 1];
    uint32_t ui32FREQ[CANA_mTOT_DCDCPSUNODE + 1];
    uint32_t ui32Magnitude[CANA_mTOT_DCDCPSUNODE + 1];
} CANA_tzTxPSUDATA_DCDC;

typedef struct CAN_zTXDRegsPSU_DCDC
{
    CANA_tzTxPSUDATA_DCDC tzPSUTxDataDCDC;

} CANA_tzTXDRegsPSU_DCDC;
/****************************** PSU DCDC ******************************************/
typedef struct CANA_zPSUDATA_DCDC
{

    float32 f32VREF;
    float32 f32IREF;
    float32 f32VACT;
    float32 f32IACT;
    float32 f32VIN;
    float32 f32IIN;
    float32 f32TEMP;
    uint16_t uiDCparam_status;
    uint16_t uiDCparam_version;
    float32 f32CHA_Amps;
    float32 f32CHB_Amps;
    float32 f32CHC_Amps;
//    uint16_t uiDeb1;
//    uint16_t uiDeb2;
//    uint16_t uiDeb3;
    uint16_t uiDerating_fact;

} CANA_tzPSUDATA_DCDC;

typedef struct CAN_zRXDRegsPSU_DCDC
{
    CANA_tzPSUDATA_DCDC tzPSUDataDCDC;
    uint16_t isReceived;
    uint16_t CANfailCnt;

} CANA_tzRXDRegsPSU_DCDC;

struct CANA_tzDCDC_FLTHWBITS
{
    uint16_t bt0_ip_voltage_ov_uv:1;
    uint16_t bt1_ip_current_oc:1;
    uint16_t bt2_op_voltage_ov:1;
    uint16_t bt3_primary_otp:1;
    uint16_t bt4_hw_system_fault:1;
    uint16_t bt5_Reserved:1;
    uint16_t bt6_Reserved:1;
    uint16_t bt7_Reserved:1;
};

union CANA_tzDCDC_FLTHWREGS
{
    Uint16 all;
    struct CANA_tzDCDC_FLTHWBITS bit;
};
struct CANA_tzDCDC_FLTSWBITS
{
    uint16_t bt0_input_overvoltage:1;
    uint16_t bt1_input_undervoltage:1;
    uint16_t bt2_input_overcurrent:1;
    uint16_t bt3_output_overvoltage:1;
    uint16_t bt4_output_overcurrent:1;
    uint16_t bt5_sec_otp:1;
    uint16_t bt6_btFanFailFault:1;
    uint16_t bt7_CanCommErr:1;
};

union CANA_tzDCDC_FLTSWREGS
{
    Uint16 all;
    struct CANA_tzDCDC_FLTSWBITS bit;
};
struct CANA_tzPSUACTNODE_BITS
{
    uint16_t bt_node1 :1;       // Bit 18
    uint16_t bt_node2 :1;       // Bit 19
    uint16_t bt_node3 :1;       // Bit 20
    uint16_t bt_node4 :1;       // Bit 21
    uint16_t bt_node5 :1;       // Bit 22
    uint16_t bt_node6 :1;       // Bit 23
    uint16_t bt_node7 :1;       // Bit 24
    uint16_t bt_node8 :1;       // Bit 25
    uint16_t bt_node9 :1;       // Bit 26


};
union CANA_tzACTNODEs_REGS
{
    Uint16 all;
    struct CANA_tzPSUACTNODE_BITS bit;
};
union CANA_tzACDCACTNODE_REGS
{
    uint16_t all;
  //  struct CANA_tzACDCACTNODE_BITS bit;
    uint16_t CanFail;
    uint16_t Node_Present;
};

typedef struct CANA_zACTNODE_PSU
{
    bool uiNode_prsnt[9+1];
    bool uiAct_nodePrsnt[CANA_mTOT_DCDCPSUNODE + 1];


} CANA_tzACTNODE_PSU;

/*============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Extern/Public Function Prototypes
 ==============================================================================*/

extern void CANA_fnPSUTX_Event(void);
extern void CANA_fnPSUComFailChk();
extern void CANA_fnPSUTx();
extern void cana_fnmsgPrcsACDC(uint16_t nodeIDACDC, uint16_t msgtypeACDC,
                               uint16_t *msgbufACDC);
extern void cana_fnmsgPrcsDCDC(uint16_t nodeIDDCDC, uint16_t unitIDDCDC,
                               uint16_t msgtypeDCDC, uint16_t *msgbufDCDC);

extern void cana_fnmsgPrcsMASTERDEBG(uint16_t *msgbufMASTER_DEBUG);
/*==============================================================================
 Extern/Public Variables
 ==============================================================================*/

// PSU related Structures
extern CANA_tzRXDRegs CANA_tzRxdRegs;
extern CANA_tzTXDRegs CANA_tzTxdRegs;
extern CANA_tzTIMERRegs CANA_tzTimerRegs;
extern CANA_tzQUERYTYPE CANA_tzQueryType;
extern CANA_tzPSURegs CANA_PSURegs;

extern union CANA_tzACTNODEs_REGS CANA_ActnodeRegs_ACDC,CANA_ActnodeRegs_DCDC0,CANA_ActnodeRegs_DCDC1;
extern CANA_tzACTNODE_PSU CANA_ACTNODE_ACDC;
extern union CANA_tzACTNODE_REGS CANA_tzActNodeRegs_PSU;
extern union CANA_tzACTNODE1_REGS CANA_tzActNode1Regs_PSU;
extern union CANA_tzFAULT_DCDCREGS CANA_tzDCDCFaultRegs[29];
extern union CANA_tzFAULT_FECHREGS CANA_tzFECHFaultRegs[29];
extern union CANA_tzFAULT_FECLREGS CANA_tzFECLFaultRegs[29];
extern CANA_tzRXDRegsPSU_ACDC CANA_tzPSURegs_RxACDC[CANA_mTOT_ACDCPSUNODE + 1];
extern CANA_tzRXDRegsPSU_DCDC CANA_tzPSURegs_RxDCDC0[CANA_mTOT_DCDCPSUNODE + 1],
CANA_tzPSURegs_RxDCDC1[CANA_mTOT_DCDCPSUNODE + 1];
extern CANA_tzTXDRegsPSU_ACDC CANA_tzPSURegs_TxACDC;
extern CANA_tzTXDRegsPSU_DCDC CANA_tzPSURegs_TxDCDC0,CANA_tzPSURegs_TxDCDC1;
/****************MK2 **************************************/
extern CANA_tzPSUACDC canA_PSUACDCbuff;
extern CANA_tzPSUDCDC canA_PSUDCDCbuff;
extern union CANA_tzACDC_FLTOthREGS CANA_tzACDC_FLTOth[CANA_mTOT_ACDCPSUNODE + 1];
extern union CANA_tzACDC_FLTHWREGS CANA_tzACDC_HWFLT[CANA_mTOT_ACDCPSUNODE + 1];
extern union CANA_tzACDC_FLTSWLREGS CANA_tzACDC_SWLFLT[CANA_mTOT_ACDCPSUNODE + 1];
extern union CANA_tzACDC_FLTSWHREGS CANA_tzACDC_SWHFLT[CANA_mTOT_ACDCPSUNODE + 1];
extern union CANA_tzDCDC_FLTOthLREGS CANA_tzDCDC0_FLTOthL[CANA_mTOT_DCDCPSUNODE + 1];
extern union CANA_tzDCDC_FLTOthHREGS CANA_tzDCDC0_FLTOthH[CANA_mTOT_DCDCPSUNODE + 1];

extern union CANA_tzDCDC_FLTHWREGS CANA_tzDCDC_HWFLT[CANA_mTOT_DCDCPSUNODE + 1];
extern union CANA_tzDCDC_FLTSWREGS CANA_tzDCDC_SWFLT[CANA_mTOT_DCDCPSUNODE + 1];

extern union CANA_tzDCDC_FLTOthLREGS CANA_tzDCDC1_FLTOthL[CANA_mTOT_DCDCPSUNODE + 1];
extern union CANA_tzDCDC_FLTOthHREGS CANA_tzDCDC1_FLTOthH[CANA_mTOT_DCDCPSUNODE + 1];
extern union CANA_tzACDCACTNODE_REGS CANA_tzACDC_ACTnode[CANA_mTOT_ACDCPSUNODE+1];
extern union CANA_tzDCDC_FLTHWREGS CANA_tzDCDC1_HWFLT[CANA_mTOT_DCDCPSUNODE + 1];
extern union CANA_tzDCDC_FLTSWREGS CANA_tzDCDC1_SWFLT[CANA_mTOT_DCDCPSUNODE + 1];
extern union CANA_tzDCDC_FLTHWREGS CANA_tzDCDC0_HWFLT[CANA_mTOT_DCDCPSUNODE + 1];
extern union CANA_tzDCDC_FLTSWREGS CANA_tzDCDC0_SWFLT[CANA_mTOT_DCDCPSUNODE + 1];

// PSU related Variables
extern uint16_t ui16txMsgDataPSU[8];
extern uint16_t ui16RxMsgDataPSU[8];
extern uint16_t ui16Rx1MsgDataPSU[8];
extern uint16_t ui16Rx2MsgDataPSU[8];
extern uint16_t ui16Rx3MsgDataPSU[8];
extern uint16_t ui16CANAPSUFailTrig1;
extern uint32_t ui32CANAPSUFailCnt1;
extern uint16_t PSUCommand,ui16manualTesting;

/*==============================================================================
 Extern/Public Constants
 ==============================================================================*/

#endif /* MODULE_APPLICATION_CANA_PSUCOM_C_INC_CANA_PSUCOM_H_ */
