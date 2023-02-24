/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  oi_glycolloop.c
 @author DEEPTI.K
 @date 06-Sep-2021

 @brief Description
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

//#include "f28004x_device.h"      // Headerfile Include File
//#include "f28004x_examples.h"    // Examples Include File
#include <stdint.h>
#include "control_defs.h"
#include "inc/hw_types.h"
#include "mathcalc.h"
#include "cana_defs.h"
/*==============================================================================
 Defines
 ==============================================================================*/

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/

PI_CONTROLLER CONTROL_tzGlycolLp;

PI_CONTROLLER CONTROL_tzGlycolLp = { .Fbk = 0, .Ref = 80.0, .Out = 0,
                                     .Kp = 0.2,
                                     .TsKi = 2.0e-2, //17.8524e-6
                                     .i1 = 0, .ui = 0, .up = 0, .v1 = 0,
                                     .w1 = 0, .Umax = 16.0, .Umin = 0.0 };

/*==============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

void CONTROL_fnGlycolLoop(void);

/*==============================================================================
 Local Variables
 ==============================================================================*/

uint16_t ui16GlycolTref = 80;
uint16_t ui16PIgain = 20;
float32_t xvar = 79;
float32_t f32EBVOutvalue = 0;

/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
void CONTROL_fnGlycolLoop(void)
{
    CONTROL_tzGlycolLp.Ref = ui16GlycolTref;

    CONTROL_tzGlycolLp.Fbk = MATHConvtzRegs.TempSensorTTC102; //xvar

    PI_MACRO(CONTROL_tzGlycolLp);  // Change Ti to speed up the process

    f32EBVOutvalue = (CONTROL_tzGlycolLp.Out) + 4.0;

    CANA_tzAnaOPParams.CANA_tzAOI[1][0].AOI1 = (f32EBVOutvalue * 10);

    CANA_fnCmdsForAnaOPIs(CANA_tzIORegs.uiUnitID, 1, 0, &CANA_tzAnaOPParams);

}

/*==============================================================================
 End of File
 ==============================================================================*/
