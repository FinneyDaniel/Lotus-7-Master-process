/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

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
#include "hal/driverlib/can.h"
#include <stdint.h>
#include "mathcalc.h"
#include "control_defs.h"

/*==============================================================================
 Defines
 ==============================================================================*/

uint16_t faultSettimer1[16] = { 0 };
uint16_t faultResettimer1[16] = { 0 };

uint16_t faultSettimer2[16] = { 0 };
uint16_t faultResettimer2[16] = { 0 };

uint16_t faultSettimer3[16] = { 0 };
uint16_t faultResettimer3[16] = { 0 };

uint16_t faultSettimer4[16] = { 0 };
uint16_t faultResettimer4[16] = { 0 };

uint16_t faultSettimer5[16] = { 0 };
uint16_t faultResettimer5[16] = { 0 };

uint16_t faultSettimer6[16] = { 0 };
uint16_t faultResettimer6[16] = { 0 };

uint16_t LPCIO1FaultRegs[16] = { 0 };
uint16_t LPCIO2FaultRegs[16] = { 0 };
uint16_t LHCIO1FaultRegs[16] = { 0 };
uint16_t LHCIO2FaultRegs[16] = { 0 };
uint16_t LHCIO3FaultRegs[16] = { 0 };
uint16_t LHCIO4FaultRegs[16] = { 0 };
/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/

/*==============================================================================
 Macros
 ==============================================================================*/
// LPC Faults
#define FAULT_mWLS101              (0U)
#define FAULT_mWLS102              (1U)
#define FAULT_mWLS103              (2U)
#define FAULT_mDOS301              (3U)
#define FAULT_mDOS303              (4U)

#define FAULT_mHYS101              (8U)
#define FAULT_mHYS501              (9U)
#define FAULT_mHYS401              (10U)
#define FAULT_mHYS102_SHUTDOWN     (11U)
#define FAULT_mHYS102_RAMPDOWN     (12U)
#define FAULT_mOXS101_SHUTDOWN     (13U)
#define FAULT_mOXS101_RAMPDOWN     (14U)

#define FAULT_mVFD_PMPFAIL         (15U)

// LHC Faults

#define FAULT_mDOS101_103          (0U)
#define FAULT_mLGT101_402          (1U)
//#define FAULT_mLGT101_303          (1U)

#define FAULT_mLVL101              (8U)
#define FAULT_mPRT101              (9U)
#define FAULT_mPRT102              (10U)
#define FAULT_mCOS101              (11U)

#define FAULT_mTTC101              (12U)
#define FAULT_mTTC102              (13U)
#define FAULT_mTTC301              (14U)
#define FAULT_mKTC401              (15U)

#define FAULT_mPRT401              (8U)
#define FAULT_mPRT409              (9U) //NOT USED
#define FAULT_mTE401               (10U)
#define FAULT_mDPT401              (11U)

//LHC IO 3 & 4
#define FAULT_mPRT402              (0U)
#define FAULT_mPRT403              (1U)
#define FAULT_mPRT404              (2U)
#define FAULT_mPRT405              (3U)
#define FAULT_mPRT406              (4U)
#define FAULT_mPRT407              (5U)
#define FAULT_mRTD401              (6U)
#define FAULT_mRTD402              (7U)
#define FAULT_mRTD403              (8U)
#define FAULT_mRTD404              (9U)
/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

static bool faultLPCIO1_fnSet(uint16_t ui16DIFaultType, uint16_t timerValueSet);
static bool faultLPCIO1_fnReset(uint16_t ui16DIFaultType,
                                uint16_t timerValueReset);

static bool faultLHCIO1_fnSet(uint16_t ui16DIFaultType1,
                              uint16_t timerValueSet1);
static bool faultLHCIO1_fnReset(uint16_t ui16DIFaultType1,
                                uint16_t timerValueReset1);

static bool faultLPCIO2_fnSet(uint16_t ui16DIFaultType, uint16_t timerValueSet);
static bool faultLPCIO2_fnReset(uint16_t ui16DIFaultType,
                                uint16_t timerValueReset);

static bool faultLHCIO2_fnSet(uint16_t ui16DIFaultType1,
                              uint16_t timerValueSet1);
static bool faultLHCIO2_fnReset(uint16_t ui16DIFaultType1,
                                uint16_t timerValueReset1);

static bool faultLHCIO3_fnSet(uint16_t ui16DIFaultType1,
                              uint16_t timerValueSet1);
static bool faultLHCIO3_fnReset(uint16_t ui16DIFaultType1,
                                uint16_t timerValueReset1);
static bool faultLHCIO4_fnSet(uint16_t ui16DIFaultType1,
                              uint16_t timerValueSet1);
static bool faultLHCIO4_fnReset(uint16_t ui16DIFaultType1,
                                uint16_t timerValueReset1);
/*==============================================================================
 Local Variables
 ==============================================================================*/

/*==============================================================================
 @file  faultCheck.c
 @author DEEPTI.K
 @date 20-Jun-2022

 @brief Description Of fault Checks of Various Sensors with delay
 ==============================================================================*/

void faultCheck(void)
{
    // Digital Inputs Faults of LPC

    // Fault checking of Water leak Sensor
    // Water Leak Sensor Status is true, PSU Current Reference will be ramped down

    if (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit0 == 0x0)
    {
        CANA_tzLPCIO1_DIFaultRegs.bit.WLS_101 = faultLPCIO1_fnSet(FAULT_mWLS101,
                                                                  20);
    }
    else if (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit0 == 0x1)
    {
        CANA_tzLPCIO1_DIFaultRegs.bit.WLS_101 = faultLPCIO1_fnReset(
        FAULT_mWLS101,
                                                                    20);
    }

    if (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit1 == 0x0)
    {
        CANA_tzLPCIO1_DIFaultRegs.bit.WLS_102 = faultLPCIO1_fnSet(
        FAULT_mWLS102,
                                                                  20);
    }
    else if (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit1 == 0x1)
    {
        CANA_tzLPCIO1_DIFaultRegs.bit.WLS_102 = faultLPCIO1_fnReset(
        FAULT_mWLS102,
                                                                    20);
    }

    if (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit2 == 0x0)
    {
        CANA_tzLPCIO1_DIFaultRegs.bit.WLS_103 = faultLPCIO1_fnSet(
        FAULT_mWLS103,
                                                                  20);
    }
    else if (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit2 == 0x1)
    {
        CANA_tzLPCIO1_DIFaultRegs.bit.WLS_103 = faultLPCIO1_fnReset(
        FAULT_mWLS103,
                                                                    20);
    }

    // Analog Inputs Faults of LPC

    // Fault checking of Hydrogen and Oxygen Sensors
    // For HYS101, HYS501, HYS401 Current References will be ramped Down
    // For HYS102 and OXS101 2 types of faults
    // 1. Ramp down(PSU Current Reference will be ramped down)
    // 2. Shutdown(PSU will be turned Off)

    if (MATHConvtzRegs.AISensorHYS101 >= 25.0f)
    {
        CANA_tzLPCIO2_AIFaultRegs.bit.HYS_101 = faultLPCIO2_fnSet(
        FAULT_mHYS101,
                                                                  20);
    }
    else if (MATHConvtzRegs.AISensorHYS101 <= 23.0f)
    {
        CANA_tzLPCIO2_AIFaultRegs.bit.HYS_101 = faultLPCIO2_fnReset(
        FAULT_mHYS101,
                                                                    20);
    }

    if (MATHConvtzRegs.AISensorHYS501 >= 25.0f)
    {
        CANA_tzLPCIO2_AIFaultRegs.bit.HYS_501 = faultLPCIO2_fnSet(
        FAULT_mHYS501,
                                                                  20);
    }
    else if (MATHConvtzRegs.AISensorHYS501 <= 23.0f)
    {
        CANA_tzLPCIO2_AIFaultRegs.bit.HYS_501 = faultLPCIO2_fnReset(
        FAULT_mHYS501,
                                                                    20);
    }

    if (MATHConvtzRegs.AISensorHYS401 >= 25.0f)
    {
        CANA_tzLPCIO2_AIFaultRegs.bit.HYS_401 = faultLPCIO2_fnSet(
        FAULT_mHYS401,
                                                                  20);
    }
    else if (MATHConvtzRegs.AISensorHYS401 <= 23.0f)
    {
        CANA_tzLPCIO2_AIFaultRegs.bit.HYS_401 = faultLPCIO2_fnReset(
        FAULT_mHYS401,
                                                                    20);
    }

    if ((CANA_tzLPCIO2_AIFaultRegs.bit.HYS_101 == 0.0)
            && (CANA_tzLPCIO2_AIFaultRegs.bit.HYS_501 == 0.0)
            && (CANA_tzLPCIO2_AIFaultRegs.bit.HYS_401 == 0.0)
            && (CANA_tzLPCIO2_AIFaultRegs.bit.HYS_102_RmpDwn == 0.0))
    {
        CANA_tzLPCIO2_AIFaultRegs.bit.HYS_NotOk = 0;
    }
    else if ((CANA_tzLPCIO2_AIFaultRegs.bit.HYS_101 == 1.0)
            || (CANA_tzLPCIO2_AIFaultRegs.bit.HYS_501 == 1.0)
            || (CANA_tzLPCIO2_AIFaultRegs.bit.HYS_401 == 1.0)
            || (CANA_tzLPCIO2_AIFaultRegs.bit.HYS_102_RmpDwn == 1.0))
    {
        CANA_tzLPCIO2_AIFaultRegs.bit.HYS_NotOk = 1;
    }

    if (MATHConvtzRegs.AISensorHYS102 >= 75.0f)
    {

        CANA_tzLPCIO2_AIFaultRegs.bit.HYS_102_ShtDwn = faultLPCIO2_fnSet(
        FAULT_mHYS102_SHUTDOWN,
                                                                         20);
        CANA_tzLPCIO2_AIFaultRegs.bit.HYS_102_RmpDwn = faultLPCIO2_fnReset(
        FAULT_mHYS102_RAMPDOWN,
                                                                           20);

    }

    else if ((MATHConvtzRegs.AISensorHYS102 >= 50.0f)
            && (MATHConvtzRegs.AISensorHYS102 < 72.0f))
    {
        CANA_tzLPCIO2_AIFaultRegs.bit.HYS_102_ShtDwn = faultLPCIO2_fnReset(
        FAULT_mHYS102_SHUTDOWN,
                                                                           20);
        CANA_tzLPCIO2_AIFaultRegs.bit.HYS_102_RmpDwn = faultLPCIO2_fnSet(
        FAULT_mHYS102_RAMPDOWN,
                                                                         20);
    }
    else if (MATHConvtzRegs.AISensorHYS102 < 48.0f)
    {
        CANA_tzLPCIO2_AIFaultRegs.bit.HYS_102_RmpDwn = faultLPCIO2_fnReset(
        FAULT_mHYS102_RAMPDOWN,
                                                                           20);
        CANA_tzLPCIO2_AIFaultRegs.bit.HYS_102_ShtDwn = faultLPCIO2_fnReset(
        FAULT_mHYS102_SHUTDOWN,
                                                                           20);
    }

    if (MATHConvtzRegs.AISensorOXS101 > 24.9f)
    {
        CANA_tzLPCIO2_AIFaultRegs.bit.OXS_101_RmpDwn = faultLPCIO2_fnReset(
        FAULT_mOXS101_RAMPDOWN,
                                                                           20);
        CANA_tzLPCIO2_AIFaultRegs.bit.OXS_101_ShtDwn = faultLPCIO2_fnSet(
        FAULT_mOXS101_SHUTDOWN,
                                                                         20);
    }
    else if ((MATHConvtzRegs.AISensorOXS101 >= 23.0f)
            && (MATHConvtzRegs.AISensorOXS101 <= 24.8f))
    {
        CANA_tzLPCIO2_AIFaultRegs.bit.OXS_101_RmpDwn = faultLPCIO2_fnSet(
        FAULT_mOXS101_RAMPDOWN,
                                                                         20);
        CANA_tzLPCIO2_AIFaultRegs.bit.OXS_101_ShtDwn = faultLPCIO2_fnReset(
        FAULT_mOXS101_SHUTDOWN,
                                                                           20);
    }
    else if (MATHConvtzRegs.AISensorOXS101 <= 22.5f)
    {
        CANA_tzLPCIO2_AIFaultRegs.bit.OXS_101_RmpDwn = faultLPCIO2_fnReset(
        FAULT_mOXS101_RAMPDOWN,
                                                                           20);
        CANA_tzLPCIO2_AIFaultRegs.bit.OXS_101_ShtDwn = faultLPCIO2_fnReset(
        FAULT_mOXS101_SHUTDOWN,
                                                                           20);
    }

    // LHC Cabinet DI Faults

    // PRT101

    if (MATHConvtzRegs.AISensorPRT101 >= 0.5)
    {
        CANA_tzLHCIO1_AIFaultRegs.bit.PRT_101 = faultLHCIO1_fnSet(
        FAULT_mPRT101,
                                                                  20);
    }
    else if (MATHConvtzRegs.AISensorPRT101 <= 0.2)
    {
        CANA_tzLHCIO1_AIFaultRegs.bit.PRT_101 = faultLHCIO1_fnReset(
        FAULT_mPRT101,
                                                                    20);
    }

    if (MATHConvtzRegs.AISensorPRT102 > 1.15)
    {
        CANA_tzLHCIO1_AIFaultRegs.bit.PRT_102 = faultLHCIO1_fnReset(
        FAULT_mPRT102,
                                                                    20);
//        CONTROLtzFaultRegs.bit.VFDPumpFail = faultLPCIO1_fnReset(
//        FAULT_mVFD_PMPFAIL,
//                                                                 20);
    }
    else if ((MATHConvtzRegs.AISensorPRT102 >= 0.51)
            && (MATHConvtzRegs.AISensorPRT102 <= 1.1))
    {
        CANA_tzLHCIO1_AIFaultRegs.bit.PRT_102 = faultLHCIO1_fnSet(
        FAULT_mPRT102,
                                                                  60);
//        CONTROLtzFaultRegs.bit.VFDPumpFail = faultLPCIO1_fnReset(
//        FAULT_mVFD_PMPFAIL,
//                                                                 20);
    }
//    else if (MATHConvtzRegs.AISensorPRT102 < 0.5)
//    {
//        CONTROLtzFaultRegs.bit.VFDPumpFail = faultLPCIO1_fnSet(
//        FAULT_mVFD_PMPFAIL,
//                                                               20);
//
//        CANA_tzLHCIO1_AIFaultRegs.bit.PRT_102 = faultLHCIO1_fnReset(
//        FAULT_mPRT102,
//                                                                    20);
//
//    }

    if (MATHConvtzRegs.AISensorCOS101 >= 10.0)
    {
        CANA_tzLHCIO1_AIFaultRegs.bit.COS_101 = faultLHCIO1_fnSet(
        FAULT_mCOS101,
                                                                  20);
    }
    else if (MATHConvtzRegs.AISensorCOS101 <= 8.0)
    {
        CANA_tzLHCIO1_AIFaultRegs.bit.COS_101 = faultLHCIO1_fnReset(
        FAULT_mCOS101,
                                                                    20);
    }

/*
    if (MATHConvtzRegs.AISensorPRT1 >= 0.4)
    {
        CANA_tzLHCIO3_AIFaultRegs.bit.PRT_402 = faultLHCIO3_fnSet(
        FAULT_mPRT402,
                                                                  20);
    }
    else if (MATHConvtzRegs.AISensorPRT1 <= 0.2)
    {
        CANA_tzLHCIO3_AIFaultRegs.bit.PRT_402 = faultLHCIO3_fnReset(
        FAULT_mPRT402,
                                                                    20);
    }
*/
    if (MATHConvtzRegs.AISensorTE401 >= 45.0)
    {
        CANA_tzLHCIO2_AIFaultRegs.bit.TE_401 = faultLHCIO2_fnSet(
        FAULT_mTE401,
                                                                 20);
    }
    else if (MATHConvtzRegs.AISensorTE401 <= 42.0)
    {
        CANA_tzLHCIO2_AIFaultRegs.bit.TE_401 = faultLHCIO2_fnReset(
        FAULT_mTE401,
                                                                   20);
    }

    // ThermoCouple Faults Based on Temperatures
    // PSU Current Reference will be ramped down upon Thermocouple Faults

    if (MATHConvtzRegs.TempSensorTTC101 >= 90.0)  //80
    {
        CANA_tzThermalFaultRegs.bit.TTC_101 = faultLHCIO1_fnSet(
        FAULT_mTTC101,
                                                                20);
    }
    else if (MATHConvtzRegs.TempSensorTTC101 <= 88.0)  // 75deg
    {
        CANA_tzThermalFaultRegs.bit.TTC_101 = faultLHCIO1_fnReset(
        FAULT_mTTC101,
                                                                  20);
    }

    if (MATHConvtzRegs.TempSensorTTC102 >= 85.0)  //80
    {
        CANA_tzThermalFaultRegs.bit.TTC_102 = faultLHCIO1_fnSet(
        FAULT_mTTC102,
                                                                20);
    }
    else if (MATHConvtzRegs.TempSensorTTC102 <= 82.0)  // 75deg
    {
        CANA_tzThermalFaultRegs.bit.TTC_102 = faultLHCIO1_fnReset(
        FAULT_mTTC102,
                                                                  20);
    }

    if (MATHConvtzRegs.TempSensorTTC301 >= 90.0)  //80
    {
        CANA_tzThermalFaultRegs.bit.TTC_301 = faultLHCIO1_fnSet(
        FAULT_mTTC301,
                                                                20);
    }
    else if (MATHConvtzRegs.TempSensorTTC301 <= 85.0)  // 75deg
    {
        CANA_tzThermalFaultRegs.bit.TTC_301 = faultLHCIO1_fnReset(
        FAULT_mTTC301,
                                                                  20);
    }

    if (MATHConvtzRegs.TempSensorKTC401 >= 80.0)  //80
    {
        CANA_tzThermalFaultRegs.bit.KTC_401 = faultLHCIO1_fnSet(
        FAULT_mKTC401,
                                                                20);
    }
    else if (MATHConvtzRegs.TempSensorKTC401 <= 75.0)  // 75deg
    {
        CANA_tzThermalFaultRegs.bit.KTC_401 = faultLHCIO1_fnReset(
        FAULT_mKTC401,
                                                                  20);
    }
/*********************** LHC IO 3 & 4 *********************************/
    /*
    if (MATHConvtzRegs.AISensorPRT402 >= 0.4)
     {
         CANA_tzLHCIO3_AIFaultRegs.bit.PRT_403 = faultLHCIO3_fnSet(
         FAULT_mPRT403,
                                                                   20);
     }
     else if (MATHConvtzRegs.AISensorPRT402 <= 0.2)
     {
         CANA_tzLHCIO3_AIFaultRegs.bit.PRT_403 = faultLHCIO3_fnReset(
         FAULT_mPRT403,
                                                                     20);
     }
     */
/*--------------------------------------------------------------------*/
    /*
    if (MATHConvtzRegs.AISensorPRT402 >= 0.4)
      {
          CANA_tzLHCIO3_AIFaultRegs.bit.PRT_403 = faultLHCIO3_fnSet(
          FAULT_mPRT403,
                                                                    20);
      }
      else if (MATHConvtzRegs.AISensorPRT402 <= 0.2)
      {
          CANA_tzLHCIO3_AIFaultRegs.bit.PRT_403 = faultLHCIO3_fnReset(
          FAULT_mPRT403,
                                                                      20);
      }
      */
/*--------------------------------------------------------------------*/
    /*
    if (MATHConvtzRegs.AISensorPRT403 >= 0.4)
      {
          CANA_tzLHCIO3_AIFaultRegs.bit.PRT_404 = faultLHCIO3_fnSet(
          FAULT_mPRT404,
                                                                    20);
      }
      else if (MATHConvtzRegs.AISensorPRT403 <= 0.2)
      {
          CANA_tzLHCIO3_AIFaultRegs.bit.PRT_404 = faultLHCIO3_fnReset(
          FAULT_mPRT404,
                                                                      20);
      }
      */
/*--------------------------------------------------------------------*/
    /*
if (MATHConvtzRegs.AISensorPRT404 >= 0.4)
  {
      CANA_tzLHCIO3_AIFaultRegs.bit.PRT_405 = faultLHCIO3_fnSet(
      FAULT_mPRT405,
                                                                20);
  }
  else if (MATHConvtzRegs.AISensorPRT404 <= 0.2)
  {
      CANA_tzLHCIO3_AIFaultRegs.bit.PRT_405 = faultLHCIO3_fnReset(
      FAULT_mPRT405,
                                                                  20);
  }
  */
/*--------------------------------------------------------------------*/
    /*
if (MATHConvtzRegs.AISensorPRT405 >= 0.4)
  {
      CANA_tzLHCIO3_AIFaultRegs.bit.PRT_406 = faultLHCIO3_fnSet(
      FAULT_mPRT406,
                                                                20);
  }
  else if (MATHConvtzRegs.AISensorPRT405 <= 0.2)
  {
      CANA_tzLHCIO3_AIFaultRegs.bit.PRT_406 = faultLHCIO3_fnReset(
      FAULT_mPRT406,
                                                                  20);
  }
  */
/*--------------------------------------------------------------------*/
    /*
if (MATHConvtzRegs.AISensorPRT406 >= 0.4)
  {
      CANA_tzLHCIO3_AIFaultRegs.bit.PRT_407 = faultLHCIO3_fnSet(
      FAULT_mPRT407,
                                                                20);
  }
  else if (MATHConvtzRegs.AISensorPRT406 <= 0.2)
  {
      CANA_tzLHCIO3_AIFaultRegs.bit.PRT_407 = faultLHCIO3_fnReset(
      FAULT_mPRT407,
                                                                  20);
  }
  */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/

if (MATHConvtzRegs.AISensorRTD401 >= 85.0)
{
    CANA_tzLHCIO4_AIFaultRegs.bit.RTD_401 = faultLHCIO4_fnSet(
    FAULT_mRTD401,
                                                             20);
}
else if (MATHConvtzRegs.AISensorRTD401 <= 42.0)
{
    CANA_tzLHCIO4_AIFaultRegs.bit.RTD_401 = faultLHCIO4_fnReset(
            FAULT_mRTD401,
                                                               20);
}
/*--------------------------------------------------------------------*/

if (MATHConvtzRegs.AISensorRTD402 >= 85.0)
{
    CANA_tzLHCIO4_AIFaultRegs.bit.RTD_402 = faultLHCIO4_fnSet(
    FAULT_mRTD402,
                                                             20);
}
else if (MATHConvtzRegs.AISensorRTD402 <= 42.0)
{
    CANA_tzLHCIO4_AIFaultRegs.bit.RTD_402 = faultLHCIO4_fnReset(
            FAULT_mRTD402,
                                                               20);
}
/*--------------------------------------------------------------------*/

if (MATHConvtzRegs.AISensorRTD403 >= 85.0)
{
    CANA_tzLHCIO4_AIFaultRegs.bit.RTD_403 = faultLHCIO4_fnSet(
    FAULT_mRTD403,
                                                             20);
}
else if (MATHConvtzRegs.AISensorRTD403 <= 42.0)
{
    CANA_tzLHCIO4_AIFaultRegs.bit.RTD_403 = faultLHCIO4_fnReset(
            FAULT_mRTD403,
                                                               20);
}
/*--------------------------------------------------------------------*/

if (MATHConvtzRegs.AISensorRTD404 >= 85.0)
{
    CANA_tzLHCIO4_AIFaultRegs.bit.RTD_404 = faultLHCIO4_fnSet(
    FAULT_mRTD404,
                                                             20);
}
else if (MATHConvtzRegs.AISensorRTD404 <= 42.0)
{
    CANA_tzLHCIO4_AIFaultRegs.bit.RTD_404 = faultLHCIO4_fnReset(
            FAULT_mRTD404,
                                                               20);
}
/*--------------------------------------------------------------------*/

}
/*==============================================================================
 @file  faultLPCIO1_fnSet.c
 @author DEEPTI.K
 @date 20-Jun-2022

 @brief Description Of Setting LPCIO-1 fault
 ==============================================================================*/

static bool faultLPCIO1_fnSet(uint16_t ui16DIFaultType, uint16_t timerValueSet)
{
    faultSettimer1[ui16DIFaultType]++;
    if (faultSettimer1[ui16DIFaultType] >= timerValueSet)
    {
        faultSettimer1[ui16DIFaultType] = timerValueSet;
        faultResettimer1[ui16DIFaultType] = 0;
        LPCIO1FaultRegs[ui16DIFaultType] = 1;
    }
    return LPCIO1FaultRegs[ui16DIFaultType];

}

/*==============================================================================
 @file  faultLPCIO1_fnReset.c
 @author DEEPTI.K
 @date 20-Jun-2022

 @brief Description Of Resetting LPCIO-1 fault
 ==============================================================================*/

static bool faultLPCIO1_fnReset(uint16_t ui16DIFaultType,
                                uint16_t timerValueReset)
{
    faultResettimer1[ui16DIFaultType]++;
    if (faultResettimer1[ui16DIFaultType] >= timerValueReset)
    {
        faultResettimer1[ui16DIFaultType] = timerValueReset;
        faultSettimer1[ui16DIFaultType] = 0;
        LPCIO1FaultRegs[ui16DIFaultType] = 0;
    }

    return LPCIO1FaultRegs[ui16DIFaultType];
}

/*==============================================================================
 @file  faultLPCIO2_fnSet.c
 @author DEEPTI.K
 @date 20-Jun-2022

 @brief Description Of Setting LPCIO-2 fault
 ==============================================================================*/

static bool faultLPCIO2_fnSet(uint16_t ui16DIFaultType, uint16_t timerValueSet)
{
    faultSettimer2[ui16DIFaultType]++;
    if (faultSettimer2[ui16DIFaultType] >= timerValueSet)
    {
        faultSettimer2[ui16DIFaultType] = timerValueSet;
        faultResettimer2[ui16DIFaultType] = 0;
        LPCIO2FaultRegs[ui16DIFaultType] = 1;
    }
    return LPCIO2FaultRegs[ui16DIFaultType];

}

/*==============================================================================
 @file  faultLPCIO2_fnReset.c
 @author DEEPTI.K
 @date 20-Jun-2022

 @brief Description Of Resetting LPCIO-2 fault
 ==============================================================================*/

static bool faultLPCIO2_fnReset(uint16_t ui16DIFaultType,
                                uint16_t timerValueReset)
{
    faultResettimer2[ui16DIFaultType]++;
    if (faultResettimer2[ui16DIFaultType] >= timerValueReset)
    {
        faultResettimer2[ui16DIFaultType] = timerValueReset;
        faultSettimer2[ui16DIFaultType] = 0;
        LPCIO2FaultRegs[ui16DIFaultType] = 0;
    }

    return LPCIO1FaultRegs[ui16DIFaultType];
}

/*==============================================================================
 @file  faultLHCIO1_fnSet.c
 @author DEEPTI.K
 @date 20-Jun-2022

 @brief Description Of Setting LHCIO-1 fault
 ==============================================================================*/

static bool faultLHCIO1_fnSet(uint16_t ui16DIFaultType, uint16_t timerValueSet)
{
    faultSettimer3[ui16DIFaultType]++;
    if (faultSettimer3[ui16DIFaultType] >= timerValueSet)
    {
        faultSettimer3[ui16DIFaultType] = timerValueSet;
        faultResettimer3[ui16DIFaultType] = 0;
        LHCIO1FaultRegs[ui16DIFaultType] = 1;
    }
    return LHCIO1FaultRegs[ui16DIFaultType];

}

/*==============================================================================
 @file  faultLHCIO1_fnReset.c
 @author DEEPTI.K
 @date 20-Jun-2022

 @brief Description Of Resetting LHCIO-1 fault
 ==============================================================================*/

static bool faultLHCIO1_fnReset(uint16_t ui16DIFaultType,
                                uint16_t timerValueReset)
{
    faultResettimer3[ui16DIFaultType]++;
    if (faultResettimer3[ui16DIFaultType] >= timerValueReset)
    {
        faultResettimer3[ui16DIFaultType] = timerValueReset;
        faultSettimer3[ui16DIFaultType] = 0;
        LHCIO1FaultRegs[ui16DIFaultType] = 0;
    }

    return LHCIO1FaultRegs[ui16DIFaultType];
}

/*==============================================================================
 @file  faultLHCIO2_fnSet.c
 @author DEEPTI.K
 @date 20-Jun-2022

 @brief Description Of Setting LHCIO-2 fault
 ==============================================================================*/

static bool faultLHCIO2_fnSet(uint16_t ui16DIFaultType, uint16_t timerValueSet)
{
    faultSettimer4[ui16DIFaultType]++;
    if (faultSettimer4[ui16DIFaultType] >= timerValueSet)
    {
        faultSettimer4[ui16DIFaultType] = timerValueSet;
        faultResettimer4[ui16DIFaultType] = 0;
        LHCIO2FaultRegs[ui16DIFaultType] = 1;
    }
    return LHCIO2FaultRegs[ui16DIFaultType];

}

/*==============================================================================
 @file  faultLHCIO2_fnReset.c
 @author DEEPTI.K
 @date 20-Jun-2022

 @brief Description Of Resetting LHCIO-2 fault
 ==============================================================================*/

static bool faultLHCIO2_fnReset(uint16_t ui16DIFaultType,
                                uint16_t timerValueReset)
{
    faultResettimer4[ui16DIFaultType]++;
    if (faultResettimer4[ui16DIFaultType] >= timerValueReset)
    {
        faultResettimer4[ui16DIFaultType] = timerValueReset;
        faultSettimer4[ui16DIFaultType] = 0;
        LHCIO2FaultRegs[ui16DIFaultType] = 0;
    }

    return LHCIO2FaultRegs[ui16DIFaultType];
}
/*==============================================================================
 @file  faultLHCIO3_fnSet.c
 @author JOTHI RAMESH
 @date 11-Feb-2023

 @brief Description Of Setting LHCIO-3 fault
 ==============================================================================*/

static bool faultLHCIO3_fnSet(uint16_t ui16DIFaultType, uint16_t timerValueSet)
{
    faultSettimer5[ui16DIFaultType]++;
    if (faultSettimer5[ui16DIFaultType] >= timerValueSet)
    {
        faultSettimer5[ui16DIFaultType] = timerValueSet;
        faultResettimer5[ui16DIFaultType] = 0;
        LHCIO3FaultRegs[ui16DIFaultType] = 1;
    }
    return LHCIO3FaultRegs[ui16DIFaultType];

}

/*==============================================================================
 @file  faultLHCIO3_fnReset.c
 @author JOTHI RAMESH
 @date 11-Feb-2023

 @brief Description Of ReSetting LHCIO-3 fault
 ==============================================================================*/

static bool faultLHCIO3_fnReset(uint16_t ui16DIFaultType,
                                uint16_t timerValueReset)
{
    faultResettimer5[ui16DIFaultType]++;
    if (faultResettimer5[ui16DIFaultType] >= timerValueReset)
    {
        faultResettimer5[ui16DIFaultType] = timerValueReset;
        faultSettimer5[ui16DIFaultType] = 0;
        LHCIO3FaultRegs[ui16DIFaultType] = 0;
    }

    return LHCIO3FaultRegs[ui16DIFaultType];
}
/*==============================================================================
 @file  faultLHCIO4_fnSet.c
 @author JOTHI RAMESH
 @date 11-Feb-2023

 @brief Description Of Setting LHCIO-4 fault
 ==============================================================================*/

static bool faultLHCIO4_fnSet(uint16_t ui16DIFaultType, uint16_t timerValueSet)
{
    faultSettimer6[ui16DIFaultType]++;
    if (faultSettimer6[ui16DIFaultType] >= timerValueSet)
    {
        faultSettimer6[ui16DIFaultType] = timerValueSet;
        faultResettimer6[ui16DIFaultType] = 0;
        LHCIO4FaultRegs[ui16DIFaultType] = 1;
    }
    return LHCIO4FaultRegs[ui16DIFaultType];

}

/*==============================================================================
 @file  faultLHCIO3_fnReset.c
 @author JOTHI RAMESH
 @date 11-Feb-2023

 @brief Description Of ReSetting LHCIO-3 fault
 ==============================================================================*/

static bool faultLHCIO4_fnReset(uint16_t ui16DIFaultType,
                                uint16_t timerValueReset)
{
    faultResettimer6[ui16DIFaultType]++;
    if (faultResettimer6[ui16DIFaultType] >= timerValueReset)
    {
        faultResettimer6[ui16DIFaultType] = timerValueReset;
        faultSettimer6[ui16DIFaultType] = 0;
        LHCIO4FaultRegs[ui16DIFaultType] = 0;
    }

    return LHCIO4FaultRegs[ui16DIFaultType];
}
/*==============================================================================
 End of File
 ==============================================================================*/
