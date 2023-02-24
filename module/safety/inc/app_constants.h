/*=============================================================================
Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
All trademarks are owned by Enarka India Private Limited
============================================================================ */

/*==============================================================================
 @file  app_constants.h
 @author JOTHI RAMESH
 @date 26-Jan-2022

 @brief Description
==============================================================================*/

#ifndef MODULE_SAFETY_INC_APP_CONSTANTS_H_
#define MODULE_SAFETY_INC_APP_CONSTANTS_H_

/*==============================================================================
 Includes
==============================================================================*/


/*==============================================================================
 Defines
==============================================================================*/
#define MAJ_VER                     (5U)
#define MIN_VER                     (2U)
#define DEV_VER                     (2001U)

#define NUM_mTIME_SLOTS              (5U)

#define OVER_VOLT_LIMIT_HI          (480.0)
#define OVER_VOLT_LIMIT_LO          (475.0)
#define OVER_TEMPERATURE_LIMIT_HI   (85.0)
#define OVER_TEMPERATURE_LIMIT_LO   (80.0)
#define MAX_MODULE_VTG              (75.0)
#define MAX_UCM_VTG                 (500.0)
#define COMMUNICATION_TIMEOUT       (60U) // 60 seconds
#define FAULT_TIMEOUT               (1U)  // seconds
#define MAX_FAULT_COUNT_CELL_OV     (300) // max limit counter count to declare cell over volt, 3 seconds
#define MIN_FAULT_COUNT_CELL_OV     (100) // min limit counter count to declare cell over volt, 500 milliseconds
#define MAX_CURRENT_CELL_OV         _IQ18(8.0)

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
 Extern/Public Function Prototypes
==============================================================================*/

/*==============================================================================
 Extern/Public Variables
==============================================================================*/

/*==============================================================================
 Extern/Public Constants
==============================================================================*/


#endif /* MODULE_SAFETY_INC_APP_CONSTANTS_H_ */
