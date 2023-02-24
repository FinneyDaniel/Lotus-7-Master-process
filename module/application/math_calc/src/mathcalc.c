///*=============================================================================
//Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
//All trademarks are owned by Enarka India Private Limited
//============================================================================ */
//
///*==============================================================================
// @file  oi_mathcalc.c
// @author DEEPTI.K
// @date 06-Sep-2021
//
// @brief Description
//==============================================================================*/
//
///*==============================================================================
// Includes
//==============================================================================*/
#include <state_machine.h>
#include "cana_defs.h"
#include "F28x_Project.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "hal/driverlib/can.h"
#include "hal/driverlib/debug.h"
#include "hal/driverlib/interrupt.h"
#include "math.h"
#include "isr.h"
#include "cana_defs.h"
#include "canb_defs.h"
#include "mathcalc.h"
#include "control_defs.h"

///*==============================================================================
// Defines
//==============================================================================*/
//
///*==============================================================================
// Enums
//==============================================================================*/
//
///*==============================================================================
// Structures
//==============================================================================*/

MATHCONV_tzREGS MATHConvtzRegs;

///*==============================================================================
// Macros
//==============================================================================*/
//
///*==============================================================================
// Local Function Prototypes
//==============================================================================*/

void MATH_fnCalc(void);

///*==============================================================================
// Local Variables
//==============================================================================*/

float32_t LccControl = 70.0;

///*==============================================================================
// Local Constants
//==============================================================================*/
//
///*=============================================================================
// @brief infinite loop for the main where tasks are executed is defined here
//
// @param void
// @return void
// ============================================================================ */

inline float32 math_fnLVLmAtoPercent(float32 LVL_mA)
{
    return (((LVL_mA - MATH_mLVL_MIN) / MATH_mLVL_RANGE) * 100);

}

inline float32 math_fnEBVmAtoPercent(float32 EBV_mA)
{
    return (((EBV_mA - MATH_mEBV_MIN) / MATH_mEBV_RANGE) * 100);

}

inline float32 math_fnHYSmAtoPercent(float32 HYS_mA, float32 HYSoffset)
{
    return (((HYS_mA - MATH_mHYS_MIN - HYSoffset) / 16) * 100);

}

inline float32 math_fnOXSmAtoPercent(float32 OXS_mA, float32 OXSoffset)
{
    return (((OXS_mA - MATH_mOXS_MIN - OXSoffset) / 16) * 25);
}

inline float32 math_fnKTCmVtoTemp(float32 KTC_mV)
{
    return (MATH_mKTC_C1 * (KTC_mV) * (KTC_mV) * (KTC_mV))
            + (MATH_mKTC_C2 * (KTC_mV) * (KTC_mV)) + (MATH_mKTC_C3 * KTC_mV)
            + MATH_mKTC_C4;
}

inline float32 math_fnTTCmVtoTemp(float32 TTC_mV)
{
    return (MATH_mTTC_C1 * (TTC_mV) * (TTC_mV) * (TTC_mV))
            + (MATH_mTTC_C2 * (TTC_mV) * (TTC_mV)) + (MATH_mTTC_C3 * TTC_mV)
            + MATH_mTTC_C4;
}

inline float32 math_fnPRTmAtoBar(float32 PRT_mV, float32 SPAN, float32 offset)
{
    return (((PRT_mV - offset) / MATH_mPRT_RANGE) * SPAN) + MATH_mPRT_LRV;
}

inline float32 math_fnCOSmAtouSpercm(float32 COS_mV)
{
    return (((COS_mV - MATH_mCOS_MIN) / MATH_mCOS_RANGE) * MATH_mCOS_SPAN)
            + MATH_mCOS_LRV;
}


inline float32 math_fnRTDmAtoTemp(float32 RTD_mA)
{
    return ((RTD_mA * MATH_mRTDGAIN) - MATH_mRTDOFFSET);
}

inline float32 math_fnDPTmAtoTemp(float32 mA)
{
    if (mA < 4)
        mA = 4;
    mA -= 4;
    return ((mA * MATH_mDPTGAIN) - MATH_mDPTOFFSET);
}


//inline float32 math_fnDPmAtoTemp(float32 DP_mA)
//{
//    return (((DP_mA -  MATH_mDP_MIN)* MATH_mDP_GAIN) - MATH_mDP_TEMPOFFSET);
//}

// CJC TO mV conversion

inline float32 math_fnKTCCJCtomV(float32 CJC_temp)
{
    return (MATH_mCJC_KTC_C1 * (CJC_temp) * (CJC_temp) * (CJC_temp))
            + (MATH_mCJC_KTC_C2 * (CJC_temp) * (CJC_temp))
            + (MATH_mCJC_KTC_C3 * CJC_temp) + MATH_mCJC_KTC_C4;
}

inline float32 math_fnMmAtoKGHR(float32 MFM_ma)
{
    return ((MFM_ma * 0.678f)/16);
}
inline float32 math_fnTTCCJCtomV(float32 CJC_temp)
{
    return (MATH_mCJC_TTC_C1 * (CJC_temp) * (CJC_temp) * (CJC_temp))
            + (MATH_mCJC_TTC_C2 * (CJC_temp) * (CJC_temp))
            + (MATH_mCJC_TTC_C3 * CJC_temp) + MATH_mCJC_TTC_C4;
}

void MATH_fnCalc(void)
{



    MATHConvtzRegs.AISensorLVL101 = math_fnLVLmAtoPercent(
            CANA_tzAISensorData.LVL_101);

    MATHConvtzRegs.AISensorEBV801 = math_fnEBVmAtoPercent(f32EBVOutvalue);

    MATHConvtzRegs.AISensorHYS101 = math_fnHYSmAtoPercent(
            CANA_tzAISensorData.HYS_101, 0.0);

    MATHConvtzRegs.AISensorHYS501 = math_fnHYSmAtoPercent(
            CANA_tzAISensorData.HYS_501, 0.0);

    MATHConvtzRegs.AISensorHYS401 = math_fnHYSmAtoPercent(
            CANA_tzAISensorData.HYS_401, 0.0);

    MATHConvtzRegs.AISensorHYS102 = math_fnHYSmAtoPercent(
            CANA_tzAISensorData.HYS_102, 0);

    MATHConvtzRegs.AISensorOXS101 = math_fnOXSmAtoPercent(
            CANA_tzAISensorData.OXS_101, 0);

    if (CANA_tzThermoCoupleData.TTC_101 > 64.0)
    {
        CANA_tzThermoCoupleData.TTC_101 = 0;
    }

    if (CANA_tzThermoCoupleData.TTC_102 > 64.0)
    {
        CANA_tzThermoCoupleData.TTC_102 = 0;
    }

    if (CANA_tzThermoCoupleData.TTC_301 > 64.0)
    {
        CANA_tzThermoCoupleData.TTC_301 = 0;
    }

    if (CANA_tzThermoCoupleData.KTC_401 > 64.0)
    {
        CANA_tzThermoCoupleData.KTC_401 = 0;
    }

    MATHConvtzRegs.TempSensorTTC101 = math_fnTTCmVtoTemp(
            math_fnTTCCJCtomV(CANA_tzIORegs.CJC[CANA_mLHC10_IO])
                    + CANA_tzThermoCoupleData.TTC_101);
    MATHConvtzRegs.TempSensorTTC102 = math_fnTTCmVtoTemp(
            math_fnTTCCJCtomV(CANA_tzIORegs.CJC[CANA_mLHC10_IO])
                    + CANA_tzThermoCoupleData.TTC_102);

    MATHConvtzRegs.TempSensorTTC301 = math_fnTTCmVtoTemp(
            math_fnTTCCJCtomV(CANA_tzIORegs.CJC[CANA_mLHC10_IO])
                    + CANA_tzThermoCoupleData.TTC_301);

    MATHConvtzRegs.TempSensorKTC401 = math_fnKTCmVtoTemp(
            math_fnKTCCJCtomV(CANA_tzIORegs.CJC[CANA_mLHC10_IO])
                    + CANA_tzThermoCoupleData.KTC_401);

    MATHConvtzRegs.AISensorPRT101 = math_fnPRTmAtoBar(
            CANA_tzAISensorData.PRT_101, 4, 4.0);  //3.9

    MATHConvtzRegs.AISensorPRT102 = math_fnPRTmAtoBar(
            CANA_tzAISensorData.PRT_102, 4, 4.0);  //4.4

    MATHConvtzRegs.AISensorPRT401 = math_fnPRTmAtoBar(
            CANA_tzAISensorData.PRT_401, 40, 4.0); // 3.9

    MATHConvtzRegs.AISensorPRT1 = math_fnPRTmAtoBar(
            CANA_tzAISensorData.PRT1, 4, 4.0); // 3.9

    MATHConvtzRegs.AISensorCOS101 = math_fnCOSmAtouSpercm(
            CANA_tzAISensorData.COS_101);

    MATHConvtzRegs.AISensorTE401 = math_fnRTDmAtoTemp(
            CANA_tzAISensorData.TE_401);

    MATHConvtzRegs.AISensorDPT401 = math_fnDPTmAtoTemp(CANA_tzAISensorData.DPT_401);

    MATHConvtzRegs.AISensorPMP101 = (CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV4/10000) * 50;

    if (MATHConvtzRegs.TempSensorTTC102 >= LccControl)
    {
        CANB_tzSiteRegs.TurnONLCC = 0xCC;
    }
    else if (MATHConvtzRegs.TempSensorTTC102 <= (LccControl - 5))
    {
        CANB_tzSiteRegs.TurnONLCC = 0x00;

    }

    MATHConvtzRegs.AISensorPMP101 = ((float32_t)(CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV4)/10000) * 50;

/*****************************************************************************************/
    MATHConvtzRegs.AISensorPRT402 = math_fnPRTmAtoBar(
             CANA_tzAISensorData.PRT_402, 50, 4.0); // 3.9

     MATHConvtzRegs.AISensorPRT403 = math_fnPRTmAtoBar(
             CANA_tzAISensorData.PRT_403, 50, 4.0); // 3.9

     MATHConvtzRegs.AISensorPRT404 = math_fnPRTmAtoBar(
              CANA_tzAISensorData.PRT_404, 50, 4.0); // 3.9

      MATHConvtzRegs.AISensorPRT405 = math_fnPRTmAtoBar(
              CANA_tzAISensorData.PRT_405, 50, 4.0); // 3.9

      MATHConvtzRegs.AISensorPRT406 = math_fnPRTmAtoBar(
               CANA_tzAISensorData.PRT_406, 50, 4.0); // 3.9

      MATHConvtzRegs.AISensorPRT407 = math_fnPRTmAtoBar(
               CANA_tzAISensorData.PRT_407, 50, 4.0); // 3.9

      MATHConvtzRegs.AISensorRTD401 = math_fnRTDmAtoTemp(
              CANA_tzAISensorData.RTD_401);
      MATHConvtzRegs.AISensorRTD402 = math_fnRTDmAtoTemp(
              CANA_tzAISensorData.RTD_402);
      MATHConvtzRegs.AISensorRTD403 = math_fnRTDmAtoTemp(
              CANA_tzAISensorData.RTD_403);
      MATHConvtzRegs.AISensorRTD404 = math_fnRTDmAtoTemp(
              CANA_tzAISensorData.RTD_404);

      MATHConvtzRegs.AISensorMFM401 = math_fnMmAtoKGHR(
              CANA_tzAISensorData.MFM_401);  // CONVERSION NOT DONE
      MATHConvtzRegs.AISensorMFM402 = math_fnMmAtoKGHR(
                    CANA_tzAISensorData.MFM_402);
   //   MATHConvtzRegs.AISensorMFM402=CANA_tzAISensorData.MFM_402;
//      MATHConvtzRegs.AISensorMFM401=math_fnMFMmAtoKGHR(CANA_tzAISensorData.MFM_401);      
//      MATHConvtzRegs.AISensorMFM402=math_fnMFMmAtoKGHR(CANA_tzAISensorData.MFM_402);


}


//
///*==============================================================================
// End of File
//==============================================================================*/
