/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  init_peripherals.c
 @author DEEPTI.K
 @date 14-Jan-2022

 @brief Description
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_can.h"

#include "hal/driverlib/can.h"
#include "init_gpio.h"
#include "init_peripherals.h"
#include "cana_defs.h"
/*==============================================================================
 Defines
 ==============================================================================*/
#define PER_mCPUTIMER0_COUNT           (20000U)
#define PER_mCPUTIMER1_COUNT           (2000000U) //200000000U - 1SEC    2000000U - 10msec
/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/

/*==============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

void INIT_fnGPIO(void);
void INIT_fnPeripherals(void);
static void per_fnIniti2cB(void);
void INIT_fnCpu_Timer(void);
void init_fnCAN_AB(void);
void INIT_fnStart_CPUtimers(void);
/*==============================================================================
 Local Variables
 ==============================================================================*/

/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @Function name : void INIT_fnPeripherals(void)
 @brief  configures CANA CANB module with a bitrate @500Kbps
         GPIO

 @param void
 @return void
 ============================================================================ */
void INIT_fnPeripherals(void)
{

    EALLOW;
    ClkCfgRegs.PERCLKDIVSEL.bit.EPWMCLKDIV = PER_mCLKDIV_BY_TWO; // Divide by 2 of PLLSYSCLK
    EDIS;
    INIT_fnGPIO();                            // GPIO initialization to the PWMs
    init_fnCAN_AB();
}
/*=============================================================================
 @Function name : void per_fnInitCAN(void)
 @brief  configures CANA module with a bitrate @500Kbps

 @param void
 @return void
 ============================================================================ */
void init_fnCAN_AB(void)
{


    CAN_initModule(CANA_BASE);
//    CAN_initModule(CANB_BASE);

    //
    // Set up the CAN bus bit rate to 500 kbps
    // Refer to the Driver Library User Guide for information on how to set
    // tighter timing control. Additionally, consult the device data sheet
    // for more information about the CAN module clocking.
    //
    CAN_setBitRate(CANA_BASE, 200000000, 500000, 20);

    CAN_setBitRate(CANB_BASE, 200000000, 500000, 20);

    CAN_disableTestMode(CANA_BASE);  // Disable Test Mode in CANA Module.

    CAN_disableTestMode(CANB_BASE);  // Disable Test Mode in CANA Module.

    CAN_startModule(CANA_BASE);

    CAN_startModule(CANB_BASE);

    GPIO_SetupPinMux(2, GPIO_MUX_CPU1, 6); //GPIO30 -  CANRXA
    GPIO_SetupPinMux(3, GPIO_MUX_CPU1, 6); //GPIO30 -  CANRXA

    per_fnIniti2cB();


}
/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void

 ============================================================================ */


void INIT_fnCANAMailBox(void)

{
    // CAN-A RX  message IDs
    /************************ for MK2 *************************************/
        CAN_setupMessageObject(
                CANA_BASE, CAN_mMAILBOX_21, CANA_mRX_mPSUACDCMSGID, CAN_MSG_FRAME_EXT,
                CAN_MSG_OBJ_TYPE_RX, 0x1F0FFFF0,
                CAN_MSG_OBJ_USE_ID_FILTER | CAN_MSG_OBJ_USE_EXT_FILTER,  //AC-DC
                CANA_mEIGHT_BYTE);

        CAN_setupMessageObject(
                CANA_BASE, CAN_mMAILBOX_22, CANA_mRX_mPSUDCDCMSGID, CAN_MSG_FRAME_EXT,
                CAN_MSG_OBJ_TYPE_RX, 0x1F0FF0F0,
                CAN_MSG_OBJ_USE_ID_FILTER | CAN_MSG_OBJ_USE_EXT_FILTER,  // DC-DC
                CANA_mEIGHT_BYTE);

        CAN_setupMessageObject(
                 CANA_BASE, CAN_mMAILBOX_15, 0x11111111, CAN_MSG_FRAME_EXT,
                 CAN_MSG_OBJ_TYPE_RX, 0x1F0FFFFF,
                 CAN_MSG_OBJ_USE_ID_FILTER | CAN_MSG_OBJ_USE_EXT_FILTER,  // cmds from PCAN to operate the PSU's
                 CANA_mEIGHT_BYTE);


    /*********************************************************************/
    // PSU - Mailbox 5-7 Receive Mailbox, Mailbox 1 Transmit Mailbox

    CAN_setupMessageObject(
            CANA_BASE, CAN_mMAILBOX_5, 0x111B0962, CAN_MSG_FRAME_EXT,
            CAN_MSG_OBJ_TYPE_RX, 0x1FFF00FF,
            CAN_MSG_OBJ_USE_ID_FILTER | CAN_MSG_OBJ_USE_EXT_FILTER,  //
            CANA_mEIGHT_BYTE);

    CAN_setupMessageObject(
            CANA_BASE, CAN_mMAILBOX_6, 0x111B096C, CAN_MSG_FRAME_EXT,
            CAN_MSG_OBJ_TYPE_RX, 0x1FFF00FF,
            CAN_MSG_OBJ_USE_ID_FILTER | CAN_MSG_OBJ_USE_EXT_FILTER,
            CANA_mEIGHT_BYTE);

    CAN_setupMessageObject(
            CANA_BASE, CAN_mMAILBOX_7, 0x111B0961, CAN_MSG_FRAME_EXT,
            CAN_MSG_OBJ_TYPE_RX, 0x1FFF00FF,
            CAN_MSG_OBJ_USE_ID_FILTER | CAN_MSG_OBJ_USE_EXT_FILTER,
            CANA_mEIGHT_BYTE);

    CAN_setupMessageObject(
            CANA_BASE, CAN_mMAILBOX_8, 0x111B0972, CAN_MSG_FRAME_EXT,
            CAN_MSG_OBJ_TYPE_RX, 0x1FFF00FF,
            CAN_MSG_OBJ_USE_ID_FILTER | CAN_MSG_OBJ_USE_EXT_FILTER,
            CANA_mEIGHT_BYTE);



    // TO RECEIVE CAN FLASHING
    CAN_setupMessageObject(CANA_BASE, CAN_mMAILBOX_1, 0x141101F2,
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_RX, 0,
                           CAN_MSG_OBJ_NO_FLAGS, CANA_mEIGHT_BYTE);

// LPCIO
    CAN_setupMessageObject(
            CANA_BASE, CAN_mMAILBOX_3, CANA_mRX_LPCMSGID1, CAN_MSG_FRAME_EXT,
            CAN_MSG_OBJ_TYPE_RX, 0X1F0FF0F0,
            CAN_MSG_OBJ_USE_ID_FILTER | CAN_MSG_OBJ_USE_EXT_FILTER, //QUERY FOR MASTER OPERATING STATE
            CANA_mEIGHT_BYTE);

    //LHCIO
    CAN_setupMessageObject(
            CANA_BASE, CAN_mMAILBOX_4, CANA_mRX_LHCMSGID1, CAN_MSG_FRAME_EXT,
            CAN_MSG_OBJ_TYPE_RX, 0X1F0FF0F0,
            CAN_MSG_OBJ_USE_ID_FILTER | CAN_MSG_OBJ_USE_EXT_FILTER, //QUERY FOR MASTER OPERATING STATE
            CANA_mEIGHT_BYTE);

    //MS
    CAN_setupMessageObject(
            CANA_BASE, CAN_mMAILBOX_11, CANA_mRX_MSMSGID1, CAN_MSG_FRAME_EXT,
            CAN_MSG_OBJ_TYPE_RX, 0X1F0FFFFF,
            CAN_MSG_OBJ_USE_ID_FILTER | CAN_MSG_OBJ_USE_EXT_FILTER, //QUERY FOR MASTER OPERATING STATE
            CANA_mEIGHT_BYTE);

     //VSC
    CAN_setupMessageObject(
              CANA_BASE, CAN_mMAILBOX_12, CANA_mRX_VSCMSGID1, CAN_MSG_FRAME_EXT,
              CAN_MSG_OBJ_TYPE_RX, 0X1F0FFFF0,
              CAN_MSG_OBJ_USE_ID_FILTER | CAN_MSG_OBJ_USE_EXT_FILTER, //QUERY FOR MASTER OPERATING STATE
              CANA_mEIGHT_BYTE);


}


/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */

void INIT_fnCANBMailBox(void)

{
    // CAN-B RX  message IDs
    //CANB   CAN RX  message ID's
//    CAN_setupMessageObject(
//            CANB_BASE, CAN_mMAILBOX_7, 0x14160103, CAN_MSG_FRAME_EXT,
//            CAN_MSG_OBJ_TYPE_RX, 0X1FFF00FF,
//            CAN_MSG_OBJ_USE_ID_FILTER | CAN_MSG_OBJ_USE_EXT_FILTER, // RUT
//            CANA_mTWO_BYTE);
//
//    CAN_setupMessageObject(
//            CANB_BASE, CAN_mMAILBOX_8, 0x141601A1, CAN_MSG_FRAME_EXT,
//            CAN_MSG_OBJ_TYPE_RX, 0X1FFF00FF,
//            CAN_MSG_OBJ_USE_ID_FILTER | CAN_MSG_OBJ_USE_EXT_FILTER, // set volt ,set H2 percent
//            CANA_mTWO_BYTE);

    CAN_setupMessageObject(
            CANB_BASE, CAN_mMAILBOX_11, 0x10143020, CAN_MSG_FRAME_EXT,
            CAN_MSG_OBJ_TYPE_RX, 0X1F0FFFFF,
            CAN_MSG_OBJ_USE_ID_FILTER | CAN_MSG_OBJ_USE_EXT_FILTER, // Start Command, set volt ,set H2 percent
            CANA_mEIGHT_BYTE);
}

/*=============================================================================
 @Function Name : void per_fnIniti2cB(void)
 @brief initialize I2CB module with a bit rate @400khz

 @param void
 @return void
 ============================================================================ */
static void per_fnIniti2cB(void)
{
    // Initialize I2C, I2C clock = 10MHz, I2C Bus Clock = 400KHz
//    I2caRegs.I2CSAR.all = 0x0000;     // Slave address - 0x00

    I2cbRegs.I2CPSC.all = 9;         // Prescaler - need 10 Mhz on module clk
    I2cbRegs.I2CCLKL = 10;            // NOTE: must be non zero
    I2cbRegs.I2CCLKH = 5;             // NOTE: must be non zero
    I2cbRegs.I2CIER.all = 0x00;       // Enable SCD & ARDY __interrupts

    I2cbRegs.I2CMDR.all = 0x0020;     // Take I2C out of reset
                                      // Stop I2C when suspended

    I2cbRegs.I2CFFTX.all = 0x6000;    // Enable FIFO mode and TXFIFO
    I2cbRegs.I2CFFRX.all = 0x2040;    // Enable RXFIFO, clear RXFFINT,

}
/*=============================================================================
 @Function name : void per_fntimerInit(void)
 @brief  configures Timer module - 10ms

 @param void
 @return void
 ============================================================================ */
void INIT_fnCpu_Timer(void)
{
    CpuTimer0Regs.TCR.all = 0;
    CpuTimer0Regs.TCR.bit.TSS = 1U;

    CpuTimer0Regs.PRD.all = PER_mCPUTIMER0_COUNT;
    CpuTimer0Regs.TPR.all = 0;
    CpuTimer0Regs.TPRH.all = 0;

    CpuTimer0Regs.TCR.bit.TIF = 1U;
    CpuTimer0Regs.TCR.bit.TRB = 1U;
    CpuTimer0Regs.TCR.bit.TIE = 1U;
    CpuTimer0Regs.TIM.all = PER_mCPUTIMER0_COUNT;

    //for timer value setting @1ms
    CpuTimer1Regs.TCR.all = 0;
    CpuTimer1Regs.TCR.bit.TSS = 1U;

    CpuTimer1Regs.PRD.all = PER_mCPUTIMER1_COUNT;
    CpuTimer1Regs.TPR.all = 0;
    CpuTimer1Regs.TPRH.all = 0;

    CpuTimer1Regs.TCR.bit.TIF = 1U;
    CpuTimer1Regs.TCR.bit.TRB = 1U;
    CpuTimer1Regs.TCR.bit.TIE = 1U;
    CpuTimer1Regs.TIM.all = PER_mCPUTIMER1_COUNT;

}
/*=============================================================================
 @Function name : void per_fntimerInit(void)
 @brief  configures Timer module - 10ms

 @param void
 @return void
 ============================================================================ */

void INIT_fnStart_CPUtimers(void)
{
    CpuTimer0Regs.TCR.bit.TSS = 0;
    CpuTimer1Regs.TCR.bit.TSS = 0;
    CpuTimer2Regs.TCR.bit.TSS = 0;
}
/*==============================================================================
 End of File
 ==============================================================================*/
