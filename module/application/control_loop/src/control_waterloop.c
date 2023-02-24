/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  control_waterloop.c
 @author DEEPTI.K
 @date 06-Sep-2021

 @brief Description: Water-flow to the stack is decided based on Water level in tank
 @and requesting for a DI water from System Controller
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include <stdint.h>
#include "F28x_Project.h"
#include <stdint.h>
#include <stdbool.h>

#include "math.h"
#include "isr.h"
#include "scheduler.h"
#include "cana_defs.h"
#include "canb_defs.h"
#include "state_machine.h"
#include "mathcalc.h"

/*==============================================================================
 Defines
 ==============================================================================*/

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

void control_waterloop(void);
void control_waterloop_stopped(void);

/*==============================================================================
 Local Variables
 ==============================================================================*/

uint16_t ui16SiteCommandTurnONPMP3 = 0;
uint16_t ui16VoltMotorSpeedCntrl = 0;
uint16_t ui16VoltMotorSpeedCntrl1 = 0;

float32_t f32VFDVoltageCAN = 0;
float32_t f32MotorSpeed = 0,vfdset = 0;

/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
void control_waterloop(void)
{

    CANA_tzIOtimers.TxCntWaterloop++;

    if (CANA_tzIOtimers.TxCntWaterloop > 120)
    {
        CANA_tzIOtimers.TxCntWaterloop = 1;
    }

    // Operating Water Solenoid Valve based on LVL101

    if (CANA_tzIOtimers.TxCntWaterloop == 1)
    {
        if ((MATHConvtzRegs.AISensorLVL101 >= 85.0) // greater than 85 % turn OFF
             || (!CANB_tzSiteRegs.StartCmd)) // or stop command is received
        {
            CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC10_IO].bit.DO0 = 0x0;

            CANB_tzSiteRegs.WaterDemand = 0x0;

            //CANA_fnMSTxCmds(1, 0, &CANA_tzDO[1][0]); //Turn OFF WSV101
        }
        else if (MATHConvtzRegs.AISensorLVL101 <= 70.0) // lesser than 70 % turn ON
        {
            CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC10_IO].bit.DO0 = 0x1;

            CANB_tzSiteRegs.WaterDemand = 0xAA;


            //CANA_fnMSTxCmds(1, 0, &CANA_tzDO[1][0]); //Turn OFF WSV101
        }
    }
    else if ((CANA_tzIOtimers.TxCntWaterloop == 60)
            || (CANA_tzIOtimers.TxCntWaterloop == 120))
    {
        if (MATHConvtzRegs.AISensorLVL101 >= 60.0) // greater than 85 % turn OFF
        {
            if (CANA_tzIOtimers.TxCntWaterloop == 60)
            {
                CANA_tzDO[0][1].bit.DO2 = 0x1;
                CANA_tzDO[0][1].bit.DO3 = 0x1;


                CANA_fnMSTxCmds(3, 1, &CANA_tzDO[0][1]); //Turn ON CTR302

            }
            else if (CANA_tzIOtimers.TxCntWaterloop == 120)
            {
                CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV4 = 9800;  // 50Hz

                CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
                                      &CANA_tzAnaOPParams); // Turn ON PMP101 VFD
            }

        }
        else if (MATHConvtzRegs.AISensorLVL101 <= 58.0)
        {
            if (CANA_tzIOtimers.TxCntWaterloop == 60)
            {
                CANA_tzDO[0][1].bit.DO2 = 0x0;
                CANA_tzDO[0][1].bit.DO3 = 0x0;
                CANA_fnMSTxCmds(3, 1, &CANA_tzDO[0][1]); //Turn OFF CTR302

            }
            else if (CANA_tzIOtimers.TxCntWaterloop == 120)
            {
                CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV4 = 9800;//vfdset;

                CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
                                      &CANA_tzAnaOPParams); // Turn OFF PMP101 VFD
            }

        }
    }

}

void control_waterloop_stopped(void)
{
    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC10_IO].bit.DO0 = 0x0;

    CANB_tzSiteRegs.WaterDemand = 0x0;
}

/*==============================================================================
 End of File
 ==============================================================================*/

