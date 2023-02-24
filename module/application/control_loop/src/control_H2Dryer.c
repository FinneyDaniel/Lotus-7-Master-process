/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  control_H2Dryer.c
 @author OHMLAP0042   
 @date 01-Feb-2022

 @brief Description
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include "F28x_Project.h"              // Standard libraries headers
#include "F2837xS_Device.h"
#include "math.h"
#include <stdint.h>
#include "control_defs.h"
#include "cana_defs.h"
#include "state_machine.h"
#include "eep.h"

/*==============================================================================
 Defines
 ==============================================================================*/

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/

union H2_tzVALVES H2_tzValves;
union H2_tzVALVES_PHS_SEP H2_tzValves_PHS;
/*==============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

void H2_fnSVcontrol(void);
void CONTROL_DryerOutletValveLogic();
void CONTROL_DryerValveBleedLogic();
void CONTROL_ReclaimTank(void);
/*==============================================================================
 Local Variables
 ==============================================================================*/

uint32_t ui16H2count, ui16H2count_eprom, ui16eepcount = 0;
uint16_t ui16valSetCnt_SV401402, ui16valResetCnt_SV401402;
uint16_t ui16CycleCount = 0;
uint16_t ui16GPIOStatus = 0;

uint16_t ui16AllValvesOffCnt = 0;

uint16_t valvestatus = 0;
uint16_t ui16Bleedh2 = 0, ui16BleedCnt = 0, dryerVal = 0, dryerVal1 = 1;
uint16_t uieepData1[8] = { 0 };
uint16_t uireadData1[8] = { 0 };

uint16_t eepwritecount = 0, epromStAdd = 0, uiepromAdd = 0;
uint16_t eepPagecnt = 0, eepPagecnt_rd = 0, readCnt = 0, writedone = 0;
uint16_t epromStAdd_wt = 0, epromStAdd_rd = 0,jk=0;
/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
void H2_fnSVcontrol(void)
{

    if (ui16Bleedh2 == 0)

    {
      if (STAT_tzStateMac.Present_st == STACK_POWER)
        {
            ui16AllValvesOffCnt = 0;
            ui16H2count++;

            // Resetting the count after 20 minutes

            if (ui16H2count > 24000)
            {
                ui16H2count = 0;
              //  ui16CycleCount = !ui16CycleCount;
            }


            if (ui16CycleCount == 0)
            {
                if ((ui16H2count == 2) || (ui16H2count == 4))
                {

                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO0 = 0x0; // Turn OFF SV1
                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO1 = 0x0; // Turn ON SV2
                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO2 = 0x1; // Turn ON SV3
                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO3 = 0x0; // Turn ON SV4
                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO4 = 0x1; // Turn ON SV5
                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO5 = 0x1; // Turn ON SV6

                    H2_tzValves.bit.bt_Svalve1 = 0;
                    H2_tzValves.bit.bt_Svalve2 = 0;
                    H2_tzValves.bit.bt_Svalve3 = 1;
                    H2_tzValves.bit.bt_Svalve4 = 0;
                    H2_tzValves.bit.bt_Svalve5 = 1;
                    H2_tzValves.bit.bt_Svalve6 = 1;
                    CANA_fnMSTxCmds(
                            CANA_mLHC_CABID, CANA_mLHC11_IO,
                            &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);

                }
                else if ((ui16H2count >= 41) && (ui16H2count <= 42)) // 2SEC
                 {
                     CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO0 = 0x0; // Turn OFF SV1
                     CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO1 = 0x1; // Turn ON SV2
                     CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO2 = 0x0; // Turn OFF SV3
                     CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO3 = 0x1; // Turn ON SV4
                     CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO4 = 0x1; // Turn ON SV5
                     CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO5 = 0x0; // Turn OFF SV6

                     H2_tzValves.bit.bt_Svalve1 = 0;
                     H2_tzValves.bit.bt_Svalve2 = 1;
                     H2_tzValves.bit.bt_Svalve3 = 0;
                     H2_tzValves.bit.bt_Svalve4 = 1;
                     H2_tzValves.bit.bt_Svalve5 = 1;
                     H2_tzValves.bit.bt_Svalve6 = 0;

                     CANA_fnMSTxCmds(
                             CANA_mLHC_CABID, CANA_mLHC11_IO,
                             &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);



                 }

//                else if ((ui16H2count >= 601) && (ui16H2count <= 602)) // 30SEC
//                {
//                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO0 = 0x0; // Turn OFF SV1
//                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO1 = 0x1; // Turn ON SV2
//                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO4 = 0x1; // Turn ON SV5
//                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO5 = 0x0; // Turn OFF SV6
//                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO2 = 0x1; // Turn ON SV3
//                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO3 = 0x1; // Turn ON SV4
//
//                    H2_tzValves.bit.bt_Svalve1 = 0;
//                    H2_tzValves.bit.bt_Svalve2 = 1;
//                    H2_tzValves.bit.bt_Svalve3 = 1;
//                    H2_tzValves.bit.bt_Svalve4 = 1;
//                    H2_tzValves.bit.bt_Svalve5 = 1;
//                    H2_tzValves.bit.bt_Svalve6 = 0;
//
//                    CANA_fnMSTxCmds(
//                            CANA_mLHC_CABID, CANA_mLHC11_IO,
//                            &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);
//
//
//
//                }
//                else if ((ui16H2count >= 9001) && (ui16H2count <= 9002)) // 450 SEC
//                 {
//                     CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO0 = 0x0; // Turn OFF SV1
//                     CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO1 = 0x1; // Turn ON SV2
//                     CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO4 = 0x1; // Turn ON SV5
//                     CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO5 = 0x1; // Turn ON SV6
//                     CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO2 = 0x1; // Turn ON SV3
//                     CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO3 = 0x1; // Turn ON SV4
//
//                     H2_tzValves.bit.bt_Svalve1 = 0;
//                     H2_tzValves.bit.bt_Svalve2 = 1;
//                     H2_tzValves.bit.bt_Svalve3 = 1;
//                     H2_tzValves.bit.bt_Svalve4 = 1;
//                     H2_tzValves.bit.bt_Svalve5 = 1;
//                     H2_tzValves.bit.bt_Svalve6 = 1;
//
//                     CANA_fnMSTxCmds(
//                             CANA_mLHC_CABID, CANA_mLHC11_IO,
//                             &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);
//
//
//
//                 }

                else if ((ui16H2count >= 11401) && (ui16H2count <= 11402)) // 570 SEC
                  {
                      CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO0 = 0x0; // Turn OFF SV1
                      CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO1 = 0x1; // Turn ON SV2
                      CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO2 = 0x0; // Turn OFF SV3
                      CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO3 = 0x1; // Turn ON SV4
                      CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO4 = 0x1; // Turn ON SV5
                      CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO5 = 0x1; // Turn ON SV6

                      H2_tzValves.bit.bt_Svalve1 = 0;
                      H2_tzValves.bit.bt_Svalve2 = 1;
                      H2_tzValves.bit.bt_Svalve3 = 0;
                      H2_tzValves.bit.bt_Svalve4 = 1;
                      H2_tzValves.bit.bt_Svalve5 = 1;
                      H2_tzValves.bit.bt_Svalve6 = 1;

                      CANA_fnMSTxCmds(
                              CANA_mLHC_CABID, CANA_mLHC11_IO,
                              &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);


                  }

                else if ((ui16H2count >= 12001) && (ui16H2count <= 12002)) // 600 SEC
                   {
                       CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO0 = 0x0; // Turn OFF SV1
                       CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO1 = 0x0; // Turn OFF SV2
                       CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO2 = 0x0; // Turn OFF SV3
                       CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO3 = 0x1; // Turn ON SV4
                       CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO4 = 0x1; // Turn ON SV5
                       CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO5 = 0x1; // Turn ON SV6

                       H2_tzValves.bit.bt_Svalve1 = 0;
                       H2_tzValves.bit.bt_Svalve2 = 0;
                       H2_tzValves.bit.bt_Svalve3 = 0;
                       H2_tzValves.bit.bt_Svalve4 = 1;
                       H2_tzValves.bit.bt_Svalve5 = 1;
                       H2_tzValves.bit.bt_Svalve6 = 1;

                       CANA_fnMSTxCmds(
                               CANA_mLHC_CABID, CANA_mLHC11_IO,
                               &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);

                   }
                else if ((ui16H2count >= 12041) && (ui16H2count <= 12042)) // 602 SEC
                   {
                       CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO0 = 0x1; // Turn ON SV1
                       CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO1 = 0x0; // Turn OFF SV2
                       CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO2 = 0x1; // Turn ON SV3
                       CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO3 = 0x0; // Turn OFF SV4
                       CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO4 = 0x0; // Turn OFF SV5
                       CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO5 = 0x1; // Turn ON SV6

                       H2_tzValves.bit.bt_Svalve1 = 1;
                       H2_tzValves.bit.bt_Svalve2 = 0;
                       H2_tzValves.bit.bt_Svalve3 = 1;
                       H2_tzValves.bit.bt_Svalve4 = 0;
                       H2_tzValves.bit.bt_Svalve5 = 0;
                       H2_tzValves.bit.bt_Svalve6 = 1;

                       CANA_fnMSTxCmds(
                               CANA_mLHC_CABID, CANA_mLHC11_IO,
                               &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);

                   }
//                else if ((ui16H2count >= 20401) && (ui16H2count <= 20402)) // 1020 SEC
//                   {
//                       CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO0 = 0x1; // Turn ON SV1
//                       CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO1 = 0x0; // Turn OFF SV2
//                       CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO4 = 0x1; // Turn ON SV5
//                       CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO5 = 0x1; // Turn ON SV6
//                       CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO2 = 0x1; // Turn ON SV3
//                       CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO3 = 0x1; // Turn ON SV4
//
//                       H2_tzValves.bit.bt_Svalve1 = 1;
//                       H2_tzValves.bit.bt_Svalve2 = 0;
//                       H2_tzValves.bit.bt_Svalve3 = 1;
//                       H2_tzValves.bit.bt_Svalve4 = 1;
//                       H2_tzValves.bit.bt_Svalve5 = 1;
//                       H2_tzValves.bit.bt_Svalve6 = 1;
//
//                       CANA_fnMSTxCmds(
//                               CANA_mLHC_CABID, CANA_mLHC11_IO,
//                               &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);
//
//                   }

                else if ((ui16H2count >= 23401) && (ui16H2count <= 23402)) // 1170 SEC
                    {
                        CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO0 = 0x1; // Turn ON SV1
                        CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO1 = 0x0; // Turn OFF SV2
                        CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO2 = 0x1; // Turn ON SV3
                        CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO3 = 0x0; // Turn OFF SV4//Before OFF
                        CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO4 = 0x1; // Turn ON SV5
                        CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO5 = 0x1; // Turn ON SV6 //Before ON

                        H2_tzValves.bit.bt_Svalve1 = 1;
                        H2_tzValves.bit.bt_Svalve2 = 0;
                        H2_tzValves.bit.bt_Svalve3 = 1;
                        H2_tzValves.bit.bt_Svalve4 = 0;
                        H2_tzValves.bit.bt_Svalve5 = 1;
                        H2_tzValves.bit.bt_Svalve6 = 1;

                        CANA_fnMSTxCmds(
                                CANA_mLHC_CABID, CANA_mLHC11_IO,
                                &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);

                    }

                else if (ui16H2count >= 24000) // 1200 SEC
                    {
                        CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO0 = 0x0; // Turn OFF SV1
                        CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO1 = 0x0; // Turn off SV2
                        CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO2 = 0x1; // Turn ON SV3
                        CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO3 = 0x0; // Turn OFF SV4
                        CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO4 = 0x1; // Turn ON SV5
                        CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO5 = 0x1; // Turn ON SV6

                        H2_tzValves.bit.bt_Svalve1 = 0;
                        H2_tzValves.bit.bt_Svalve2 = 0;
                        H2_tzValves.bit.bt_Svalve3 = 1;
                        H2_tzValves.bit.bt_Svalve4 = 0;
                        H2_tzValves.bit.bt_Svalve5 = 1;
                        H2_tzValves.bit.bt_Svalve6 = 1;

                        CANA_fnMSTxCmds(
                                CANA_mLHC_CABID, CANA_mLHC11_IO,
                                &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);

                    }
            }
        }

        // Keeping all the valves OFF in Other states Except Stack Power

        else if ((ui16Bleedh2 == 0)
                && (STAT_tzStateMac.Present_st != STACK_POWER))
        {
            ui16CycleCount = 0;
            ui16AllValvesOffCnt++;
            if (ui16AllValvesOffCnt > 3)
            {
                ui16AllValvesOffCnt = 3;
            }

            if ((ui16AllValvesOffCnt >= 1) && (ui16AllValvesOffCnt <= 2))
            {
                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO0 = 0x1; // Close SV1
                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO1 = 0x1; // Close SV2
                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO2 = 0x1; // Close SV3
                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO3 = 0x1; // Close SV4
                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO4 = 0x1; // Close SV5
                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO5 = 0x1; // Close SV6

                                       H2_tzValves.bit.bt_Svalve1 = 1;
                                       H2_tzValves.bit.bt_Svalve2 = 1;
                                       H2_tzValves.bit.bt_Svalve3 = 1;
                                       H2_tzValves.bit.bt_Svalve4 = 1;
                                       H2_tzValves.bit.bt_Svalve5 = 1;
                                       H2_tzValves.bit.bt_Svalve6 = 1;


                CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC11_IO,
                                &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);

          //      H2_tzValves.all = 0;

            }
        }
      }
}
/****************************************************************************************************/
// Reclamation tank logic
void CONTROL_ReclaimTank(void)
{
    if ((STAT_tzStateMac.Present_st == STACK_POWER) ||
        (STAT_tzStateMac.Present_st == STACK_CHECK) ||
        (STAT_tzStateMac.Present_st == READY))
    {
        if (!CANA_tzLHCDI_IORegs[CANA_mLHC12_IO].bit.DI_bit3)
        {
            // turn off pump
            CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC10_IO].bit.DO3 = 0x0;
        }
        else
        {
            if (CANA_tzLHCDI_IORegs[CANA_mLHC12_IO].bit.DI_bit2)
            {
                // turn on pump
                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC10_IO].bit.DO3 = 0x1;
            }
        }
    }
    else
    {
        // turn off pump
        CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC10_IO].bit.DO3 = 0x0;
    }
}
/****************************************************************************************************/
// Phase separator logic
void CONTROL_PhaseSeparator(void)
{
    if ((STAT_tzStateMac.Present_st == STACK_POWER) ||
        (STAT_tzStateMac.Present_st == STACK_CHECK) ||
        (STAT_tzStateMac.Present_st == READY))
    {
         if ((CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].bit.DI_bit5 ==1)&&(CANA_tzLHCDI_IORegs[CANA_mLHC12_IO].bit.DI_bit7 ==1))    // IO-0 DO 6 for LVL-402 && IO-2 - DO7(LVL414)
        {
            CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO6 = 0x1;  //SV7 ON
            H2_tzValves.bit.bt_Svalve7 = 1;
        }
        else//if(CANA_tzLHCDI_IORegs[CANA_mLHC12_IO].bit.DI_bit7 ==0) //  IO-2 - DO7(LVL414)
        {
            CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO6 = 0x0;  //SV7 OFF
            H2_tzValves.bit.bt_Svalve7 = 0;
        }


        if ((CANA_tzLHCDI_IORegs[CANA_mLHC11_IO].bit.DI_bit4 ==1)&&(CANA_tzLHCDI_IORegs[CANA_mLHC12_IO].bit.DI_bit6==1)) // LVL 404 && LVL413
        {
            CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC13_IO].bit.DO3 = 0x1;  //SV8 ON
            H2_tzValves_PHS.bit.bt_Svalve8 =1;
        }
        else//  IO-2 - DO6(LVL413)
        {
             CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC13_IO].bit.DO3 = 0x0;  //SV8 OFF
             H2_tzValves_PHS.bit.bt_Svalve8 =0;
        }


        if ((CANA_tzLHCDI_IORegs[CANA_mLHC11_IO].bit.DI_bit6 ==1)&&(CANA_tzLHCDI_IORegs[CANA_mLHC12_IO].bit.DI_bit5==1)) // LVL 406 && LVL412
         {
             CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC13_IO].bit.DO2 = 0x1;  //SV9 ON
             H2_tzValves_PHS.bit.bt_Svalve9=1;
         }
        else
        {
            CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC13_IO].bit.DO2 = 0x0;  //SV9 ON
            H2_tzValves_PHS.bit.bt_Svalve9 =0;
        }


        if((CANA_tzLHCDI_IORegs[CANA_mLHC12_IO].bit.DI_bit1==1)&&(CANA_tzLHCDI_IORegs[CANA_mLHC12_IO].bit.DI_bit4==1))  //LVL 408 && LVL 411
        {
            CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC13_IO].bit.DO1 = 0x1;  //SV10 ON
            H2_tzValves_PHS.bit.bt_Svalve10 =1;
        }
        else
        {
            CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC13_IO].bit.DO1 = 0x0;  //SV10 OFF
            H2_tzValves_PHS.bit.bt_Svalve10 =0;
        }
        CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC11_IO,
                        &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);

        CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC13_IO,
                        &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC13_IO]);

    }
        /****************************************************************************************/
}

// Dryer outlet SV401/2
void CONTROL_DryerOutlet(void)
{
#if 0
    if ((ui16SafeShutDownFlg == 1) || (ui16InstShutDownFlg == 1) || (STAT_tzStateMac.Present_st != STACK_POWER))
    {
        // Turn off SV
        CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC10_IO].bit.DO1 = 0x0;
    }
    else if ((ui16SafeShutDownFlg == 0) && (ui16InstShutDownFlg == 0) && (STAT_tzStateMac.Present_st == STACK_POWER))
    {
        // Turn on SV
        CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC10_IO].bit.DO1 = 0x1;
    }
#endif
#if 1
    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC10_IO].bit.DO1 = 0x1;
    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC10_IO].bit.DO2 = 0x1;
#else
    if ((ui16SafeShutDownFlg == 1) || (ui16InstShutDownFlg == 1))
    {
        // Turn off SV
        CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC10_IO].bit.DO1 = 0x0;
    }
    else if ((ui16SafeShutDownFlg == 0) && (ui16InstShutDownFlg == 0))
    {
        // Turn on SV
        CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC10_IO].bit.DO1 = 0x1;
    }
#endif
}

/*==============================================================================
 End of File
 ==============================================================================*/
