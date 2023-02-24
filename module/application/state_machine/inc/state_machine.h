/*=============================================================================
Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
All trademarks are owned by Enarka India Private Limited
============================================================================ */

/*==============================================================================
 @file  oi_state.h
 @author DEEPTI.K
 @date 06-Sep-2021

 @brief Description
==============================================================================*/

#ifndef MODULE_STATE_MACHINE_INC_STATE_MACHINE_H_
#define MODULE_STATE_MACHINE_INC_STATE_MACHINE_H_

/*==============================================================================
 Includes
==============================================================================*/

#include "F28x_Project.h"
#include <stdint.h>
#include <stdbool.h>
#include "state_machine.h"
/*==============================================================================
 Defines
==============================================================================*/



/*==============================================================================
 Enums
==============================================================================*/

/*==============================================================================
 Structures
==============================================================================*/

typedef enum
{
    MS_STANDBY,
    MS_PURGE,
    MS_IOPOWER,
    MS_ARMED_POWER,
    MS_FAULT,
    MS_SHUTDOWN,
}STATE_enumVALMS;

typedef struct STAT_zSTATE_MSMAC
{
    STATE_enumVALMS Present_st;

}STAT_tzSTATE_MSMAC;

typedef enum
{
    STAND_BY,
    READY,
    STACK_CHECK,
    STACK_POWER,
    FAULT,
    COMMISSION,
}STATE_enumVAL;

typedef struct STAT_zSTATEMAC
{
    STATE_enumVAL Next_st;
    STATE_enumVAL Present_st;
    STATE_enumVAL Previous_st;
}STAT_tzSTATEMAC;



/*==============================================================================
 Macros
==============================================================================*/

/*==============================================================================
 Extern/Public Function Prototypes
==============================================================================*/

extern void STAT_fnFSMCheck(void);
extern void stat_fnInitState(void);

/*==============================================================================
 Extern/Public Structures
==============================================================================*/

extern STAT_tzSTATEMAC STAT_tzStateMac;
extern STAT_tzSTATE_MSMAC STAT_tzStateMacMS;


/*==============================================================================
 Extern/Public Constants
==============================================================================*/


#endif /* MODULE_STATE_MACHINE_INC_STATE_MACHINE_H_ */
