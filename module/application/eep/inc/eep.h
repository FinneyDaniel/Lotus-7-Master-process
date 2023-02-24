/*=============================================================================
Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
All trademarks are owned by Enarka India Private Limited
============================================================================ */

/*==============================================================================
 @file  eep.h
 @author OHMLAP0042   
 @date 06-Jun-2022

 @brief Description
==============================================================================*/

#ifndef MODULE_APPLICATION_EEP_INC_EEP_H_
#define MODULE_APPLICATION_EEP_INC_EEP_H_

/*==============================================================================
 Includes
==============================================================================*/

#include <stdint.h>
#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "F2837xS_device.h"
/*==============================================================================
 Defines
==============================================================================*/
#define EEP_mADC_A                             (1U)
#define EEP_mADC_B                             (2U)
#define EEP_mCJC_mV                            (3U)

#define EEP_mRESET_ALL                           (0U)
#define EEP_mQUERY                               (1U)
#define EEP_mRESET                               (0xAAU)

#define EEP_mWRITE                             (1U)
#define EEP_mREAD                              (2U)
#define EEP_mIDLE                              (0U)

#define EEP_mWRITE_MODE                        (0x10)
#define EEP_mREAD_MODE                         (0x01)
#define EEP_mIDLE_MODE                         (0x11)

#define EEP_mSUCCESS                           (1U)
#define MAX_BUFFER_SIZE                        (14U)

#define EEP_mCAN_FLASH_ADDR                                   (0x120U)
#define EEP_mCAN_ERROR_ADDR                                   (0x122U)
#define EEP_mCAN_LENGTH_ADDR                                  (0x124U)
#define EEP_mCAN_CRC32_ADDR                                   (0x128U)
#define EEP_mCAN_DRYER_ADDR                                   (0x0U)//(0x12AU)


#define EEP_mCHANNEL_ID1                            (0x01)
#define EEP_mCHANNEL_ID2                            (0x02)
#define EEP_mCHANNEL_ID3                            (0x03)
#define EEP_mCHANNEL_ID4                            (0x04)
#define EEP_mCHANNEL_ID5                            (0x05)
#define EEP_mCHANNEL_ID6                            (0x06)
#define EEP_mCHANNEL_ID7                            (0x07)
#define EEP_mCHANNEL_ID8                            (0x08)

/*==============================================================================
 Enums
==============================================================================*/

/*==============================================================================
 Structures
==============================================================================*/
typedef struct EEP_zMSG_BUFF
{
    uint16_t msgStatus;
    uint16_t slaveAddr;                  // Slave address tied to the message.
    uint16_t numBytes;                   // Number of valid bytes in message.
    uint16_t memoryHighAddr;             // EEPROM address of data associated
                                         // with message (high byte).
    uint16_t memoryLowAddr;              // EEPROM address of data associated
                                         // with message (low byte).
    uint16_t *msgBuffer; // Array holding message data.
}EEP_tzMSG_BUFF;

typedef struct EEP_zFLAGS
{
    bool bt_busyflag;
    bool bt_eepsuccess_flag;
    bool bt_eepfail_flag;
    bool bt_loaddata;
    bool bt_ResetAll;
    bool bt_eepReset;
    bool bt_eepWrite;
}EEP_tzFLAGS;


/*==============================================================================
 Macros
==============================================================================*/

/*==============================================================================
 Extern/Public Function Prototypes
==============================================================================*/


/*==============================================================================
 Extern/Public Variables
==============================================================================*/

extern uint16_t uieepData[8];
extern uint16_t uiRxMsgBuffer[10];
extern uint16_t uiRxMsgBuffer1[5];
extern uint16_t uiTxMsgBuffer[5];
extern uint16_t uiTxMsgBuffer1[5];
extern EEP_tzMSG_BUFF transaction_I2CMsg;
extern void eep_fnTask(void);
extern bool EEP_fnWrite(EEP_tzMSG_BUFF  *pTx_msg, uint16_t ui_addr, uint16_t *pdata, uint16_t uilen);
extern bool EEP_fnRead(EEP_tzMSG_BUFF  *pRx_msg, uint16_t ui_addr, uint16_t *pdata, uint16_t uilen);


/*==============================================================================
 Extern/Public Constants
==============================================================================*/


/*==============================================================================
 Extern/Public Function Prototypes
==============================================================================*/

extern bool eep_fnPollwrite(uint16_t address, uint16_t *data, uint16_t bytes);
extern bool eep_fnPollRead(uint16_t address, uint16_t *data, uint16_t bytes);
extern void eep_fnInit(void);
extern void eep_fnidle(void);
extern EEP_tzFLAGS eep_tzflag;

/*==============================================================================
 Extern/Public Variables
==============================================================================*/

/*==============================================================================
 Extern/Public Constants
==============================================================================*/


#endif /* MODULE_APPLICATION_EEP_INC_EEP_H_ */
