/*=============================================================================
Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
All trademarks are owned by Enarka India Private Limited
============================================================================ */

/*==============================================================================
 @file  oi_eep.c
 @author Swathi BK
 @date 19-Apr-2022

 @brief Description
==============================================================================*/

/*==============================================================================
 Includes
==============================================================================*/

#include "eep.h"
#include "F28x_Project.h"              // Standard libraries headers
#include "F2837xS_Device.h"
#include "math.h"
#include <stdint.h>

/*==============================================================================
 Defines
==============================================================================*/

/*==============================================================================
 Enums
==============================================================================*/


typedef enum EEP_eSTATE
{
      eNULL = 0,
      eIDLE,
      WRITE_MODE,
      SET_ADDRESS,
      WRITE_DATA,
      TIME_OUT,
      READ_MODE,
      CHECK_FOR_NAK,
      CHECK_FOR_SCD,
      READ_DATA,
}EEP_teSTATE;

EEP_tzFLAGS eep_tzflag =
{
   .bt_busyflag = false,
   .bt_eepsuccess_flag = false,
   .bt_eepfail_flag = false,
   .bt_loaddata = false,
   .bt_ResetAll = false,
   .bt_eepReset = false,
   .bt_eepWrite = false,
};

EEP_teSTATE eep_teState;
EEP_tzMSG_BUFF transaction_I2CMsg;
/*==============================================================================
 Structures
==============================================================================*/
EEP_tzMSG_BUFF transaction_I2CMsg;
EEP_tzMSG_BUFF *currentMsgPtr;

EEP_tzMSG_BUFF *currentMsgPtr;

/*==============================================================================
 Macros
==============================================================================*/
volatile Uint16 timeout_count;
#define I2C_DELAY           5000
#define I2C_WAIT_UNTIL(x)   { \
                                timeout_count = 0; \
                                while((x)) \
                                { \
                                    timeout_count++; \
                                    if (timeout_count >= I2C_DELAY) \
                                        { \
                                        return 0; \
                                        } \
                                } \
                            } \
/*==============================================================================
 Local Function Prototypes
==============================================================================*/

static void (*eep_fnEvent)(void);
static void (*eep_fnEvent_cmp)(void);

bool eep_fnPollwrite(uint16_t address, uint16_t *data, uint16_t bytes);
bool eep_fnPollRead(uint16_t address, uint16_t *data, uint16_t bytes);

void eep_fnInit(void);

/*==============================================================================
 Local Variables
==============================================================================*/
bool bt_busyflag;
uint16_t uiRW_eep = 0;
uint16_t uiTxMsgBuffer[5] = {0};
uint16_t uiTxMsgBuffer1[5] = {0};

uint16_t uieepData[8] = { 0 };

/*==============================================================================
 Local Constants
==============================================================================*/

/*=============================================================================
 @Function Name : void eep_fnWritetask(void)
 @brief write task for EEPROM

 @param void
 @return void
 ============================================================================ */
void eep_fnInit(void)
{
    eep_fnEvent = eep_fnidle;
    eep_fnEvent_cmp = eep_fnidle;
    eep_tzflag.bt_eepsuccess_flag = true;


}

/*=============================================================================
 @Function Name : void eep_fnWritetask(void)
 @brief write task for EEPROM

 @param void
 @return void
 ============================================================================ */
bool eep_fnPollwrite(uint16_t address, uint16_t *data, uint16_t bytes)
{
    uint16_t i = 0;

    I2C_WAIT_UNTIL(I2cbRegs.I2CMDR.bit.STP == 1);

    I2cbRegs.I2CSAR.all = 0x50;

    //
    // Check if bus busy
    //
    I2C_WAIT_UNTIL(I2cbRegs.I2CSTR.bit.BB == 1);

    I2cbRegs.I2CCNT = bytes + 2;

    I2cbRegs.I2CDXR.all = (address >> 8) & 0xFF;
    I2cbRegs.I2CDXR.all =  (address >> 0) & 0xFF;

    for (i = 0; i < bytes; i++)
    {
        I2cbRegs.I2CDXR.all = data[i];
    }

    //
    // Send start as master transmitter
    //
    I2cbRegs.I2CMDR.all = 0x6E20;
    DELAY_US(5000);

    I2C_WAIT_UNTIL(I2cbRegs.I2CSTR.bit.SCD == 0);

    return true;
}

/*=============================================================================
 @Function Name : void eep_fnWritetask(void)
 @brief write task for EEPROM

 @param void
 @return void
 ============================================================================ */
bool eep_fnPollRead(uint16_t address, uint16_t *data, uint16_t bytes)
{
    uint16_t i = 0;

    I2C_WAIT_UNTIL(I2cbRegs.I2CMDR.bit.STP == 1);

    I2cbRegs.I2CSAR.all = 0x50;

    // If we are in the the address setup phase, send the address without a
    // stop condition.
    //
    // Check if bus busy
    I2C_WAIT_UNTIL(I2cbRegs.I2CSTR.bit.BB == 1);

    I2cbRegs.I2CCNT = 2;

    I2cbRegs.I2CDXR.all = (address >> 8) & 0xFF;
    I2cbRegs.I2CDXR.all =  (address >> 0) & 0xFF;

    I2cbRegs.I2CMDR.all = 0x6620;

    I2C_WAIT_UNTIL(I2cbRegs.I2CSTR.bit.ARDY != 1);

    if (I2cbRegs.I2CSTR.bit.NACK == 1)
    {
        I2cbRegs.I2CMDR.bit.STP = 1;
        I2cbRegs.I2CSTR.all = (1U << 1);
        return false;
    }

    I2cbRegs.I2CCNT = bytes;

    I2cbRegs.I2CMDR.all = 0x6C20;

    if (I2cbRegs.I2CSTR.bit.NACK == 1)
    {
        I2cbRegs.I2CMDR.bit.STP = 1;
        I2cbRegs.I2CSTR.all = (1U << 1);
        return false;
    }

    I2C_WAIT_UNTIL(I2cbRegs.I2CSTR.bit.SCD == 0);

    DELAY_US(5000);

    for (i = 0; i < bytes; i++)
    {
        data[i] = I2cbRegs.I2CDRR.all;
    }

    return true;
}



/*=============================================================================
 @Function Name : void eep_fnWritetask(void)
 @brief write task for EEPROM

 @param void
 @return void
 ============================================================================ */
void eep_fnwritetask(void)
{

    switch (eep_teState)
    {
    case WRITE_MODE:
    {
        //check for stp bit
        if ((!I2cbRegs.I2CMDR.bit.STP)
                && (currentMsgPtr->msgStatus == EEP_mWRITE_MODE))
        {
            eep_teState = SET_ADDRESS;

        }
        else
        {
            eep_teState = TIME_OUT;
        }
    }
        break;

    case SET_ADDRESS:
    {
        I2cbRegs.I2CSAR.all = currentMsgPtr->slaveAddr;
        if (!I2cbRegs.I2CSTR.bit.BB)
        {
            I2cbRegs.I2CCNT = currentMsgPtr->numBytes + 2;

            I2cbRegs.I2CDXR.all = currentMsgPtr->memoryHighAddr;
            I2cbRegs.I2CDXR.all = currentMsgPtr->memoryLowAddr;

            eep_teState = WRITE_DATA;
        }
        else
        {
            eep_teState = TIME_OUT;
        }
    }
        break;

    case WRITE_DATA:
    {

        if (currentMsgPtr->numBytes > 0)
        {
            I2cbRegs.I2CDXR.all = *(currentMsgPtr->msgBuffer);
            currentMsgPtr->msgBuffer++;
            currentMsgPtr->numBytes--;
        }
        else
        {
            eep_teState = CHECK_FOR_SCD;
            I2cbRegs.I2CMDR.all = 0x6E20;
        }

    }
        break;

    case CHECK_FOR_SCD:
    {

        if (I2cbRegs.I2CSTR.bit.SCD)
        {
            eep_fnEvent = eep_fnidle;
            eep_fnEvent_cmp = eep_fnidle;
            eep_tzflag.bt_eepsuccess_flag = true;
        }
        else
        {
            eep_teState = TIME_OUT;
        }
    }
        break;

    case TIME_OUT:
    {
        eep_fnEvent = eep_fnidle;
        eep_fnEvent_cmp = eep_fnidle;
        eep_tzflag.bt_eepfail_flag = true;
    }
        break;

    }

}

/*=============================================================================
 @Function Name : void eep_fnWritetask(void)
 @brief write task for EEPROM

 @param void
 @return void
 ============================================================================ */
void eep_fnreadtask(void)
{
   // eep_teState= READ_MODE;

    switch (eep_teState)
    {
    case READ_MODE:
    {
        if ((!I2cbRegs.I2CMDR.bit.STP)
                && (currentMsgPtr->msgStatus == EEP_mREAD_MODE))
        {
            eep_teState = SET_ADDRESS;
        }
    }
        break;

    case SET_ADDRESS:
    {
        I2cbRegs.I2CSAR.all = currentMsgPtr->slaveAddr;
        if (!I2cbRegs.I2CSTR.bit.BB)
        {
            I2cbRegs.I2CCNT = 2;

            I2cbRegs.I2CDXR.all = currentMsgPtr->memoryHighAddr;
            I2cbRegs.I2CDXR.all = currentMsgPtr->memoryLowAddr;

            I2cbRegs.I2CMDR.all = 0x6620;
            eep_teState = CHECK_FOR_NAK;
        }
        else
        {
            eep_teState = TIME_OUT;
        }
    }
    break;

    case CHECK_FOR_NAK:
    {
        if (I2cbRegs.I2CSTR.bit.ARDY)
        {
            if (I2cbRegs.I2CSTR.bit.NACK)
            {
                I2cbRegs.I2CMDR.bit.STP = 1;
                I2cbRegs.I2CSTR.all = (1U << 1);
                eep_teState = READ_MODE;
                eep_tzflag.bt_eepfail_flag = false;
            }
            else
            {
                I2cbRegs.I2CCNT = currentMsgPtr->numBytes;
                I2cbRegs.I2CMDR.all = 0x6C20;
                eep_teState = CHECK_FOR_SCD;
            }
        }
        else
        {
            eep_teState = TIME_OUT;
        }
    }
    break;

    case CHECK_FOR_SCD:
    {
        if(I2cbRegs.I2CSTR.bit.SCD)
        {
            eep_teState = READ_DATA;

        }
        else
        {
            eep_teState = TIME_OUT;

        }
    }
    break;

    case READ_DATA:
    {
        if (currentMsgPtr->numBytes > 0)
        {
            *(currentMsgPtr->msgBuffer) =
                    I2cbRegs.I2CDRR.all;
            currentMsgPtr->msgBuffer++;
            currentMsgPtr->numBytes--;
        }
        else
        {
            eep_fnEvent = eep_fnidle;
            eep_fnEvent_cmp = eep_fnidle;
            eep_tzflag.bt_eepsuccess_flag = true;
        }
    }
    break;

    case TIME_OUT:
        {
            eep_fnEvent = eep_fnidle;
            eep_fnEvent_cmp = eep_fnidle;
            eep_tzflag.bt_eepfail_flag = true;
        }
            break;


    }
}

/*=============================================================================
 @Function Name : uint16_t I2C_fnReadData(struct I2CMsg *msg)
 @brief read function for i2c bus

 @param struct I2CMsg *msg
 @return uint16_t (0) on SUCCES
 ============================================================================ */
bool EEP_fnWrite(EEP_tzMSG_BUFF  *pTx_msg, uint16_t ui_addr, uint16_t *pdata, uint16_t uilen)
{
    bool bt_status = false;

   // if ((!eep_tzflag.bt_busyflag) && (eep_tzflag.bt_eepsuccess_flag == true))
    {
        eep_tzflag.bt_busyflag = true;
        eep_tzflag.bt_eepfail_flag = false;
        pTx_msg->msgStatus = EEP_mWRITE_MODE;
        pTx_msg->slaveAddr = 0x50;
        pTx_msg->memoryHighAddr = (ui_addr >> 8) & 0xFF;
        pTx_msg->memoryLowAddr = (ui_addr >> 0) & 0xFF;
        pTx_msg->numBytes = uilen;
        pTx_msg->msgBuffer = pdata;
        eep_teState = WRITE_MODE;
        eep_fnEvent = eep_fnwritetask;
        eep_fnEvent_cmp = eep_fnwritetask;
        currentMsgPtr = pTx_msg;
        bt_status = true;
    }

    return bt_status;
}

/*=============================================================================
 @Function Name : uint16_t I2C_fnReadData(struct I2CMsg *msg)
 @brief read function for i2c bus

 @param struct I2CMsg *msg
 @return uint16_t (0) on SUCCES
 ============================================================================ */
bool EEP_fnRead(EEP_tzMSG_BUFF  *pRx_msg, uint16_t ui_addr, uint16_t *pdata, uint16_t uilen)
{
    bool bt_status = false;


   // if((!eep_tzflag.bt_busyflag) && (eep_tzflag.bt_eepsuccess_flag == true))
    {
        eep_tzflag.bt_busyflag = true;
        eep_tzflag.bt_eepfail_flag = false;
        pRx_msg->msgStatus = EEP_mREAD_MODE;
        eep_teState = READ_MODE;
        pRx_msg->slaveAddr = 0x50;
        pRx_msg->memoryHighAddr = (ui_addr >> 8) & 0xFF;
        pRx_msg->memoryLowAddr = ui_addr & 0xFF;
        pRx_msg->numBytes = uilen;
        pRx_msg->msgBuffer =  pdata;
        eep_fnEvent = eep_fnreadtask;
        eep_fnEvent_cmp = eep_fnreadtask;
        currentMsgPtr = pRx_msg;
        bt_status = true;
    }

    return bt_status;

}

bool eep_is_transaction_in_progress(EEP_tzMSG_BUFF  *pRx_msg)
{
    return eep_tzflag.bt_busyflag;
}

bool eep_is_transaction_complete(EEP_tzMSG_BUFF  *pRx_msg)
{
    return (!eep_tzflag.bt_busyflag) && (eep_tzflag.bt_eepsuccess_flag);
}

bool eep_is_transaction_fail(EEP_tzMSG_BUFF  *pRx_msg)
{
    return (!eep_tzflag.bt_busyflag) && (eep_tzflag.bt_eepfail_flag);
}

/*=============================================================================
 @Function Name : uint16_t I2C_fnReadData(struct I2CMsg *msg)
 @brief read function for i2c bus

 @param struct I2CMsg *msg
 @return uint16_t (0) on SUCCES
 ============================================================================ */
void eep_fnTask(void)
{

    if (eep_fnEvent == eep_fnEvent_cmp)
        (*eep_fnEvent)();
    else
    {
        if ((eep_fnEvent == eep_fnidle) || (eep_fnEvent == eep_fnreadtask) || (eep_fnEvent == eep_fnwritetask))
        {
            eep_fnEvent_cmp = eep_fnEvent;
        }
        else if ((eep_fnEvent_cmp == eep_fnidle) || (eep_fnEvent_cmp == eep_fnreadtask) || (eep_fnEvent_cmp == eep_fnwritetask))
        {
            eep_fnEvent = eep_fnEvent_cmp;
        }
        else
        {
//            sl_power_off(MEF_INVALID_MEM);
        }
    }

}

/*=============================================================================
 @Function Name : void eep_fnidle(void)
 @brief eep_fnidle mode of EEPROM functions

 @param void
 @return void
 ============================================================================ */
void eep_fnidle(void)
{
    eep_tzflag.bt_busyflag = false;

}

/*==============================================================================
 End of File
==============================================================================*/
