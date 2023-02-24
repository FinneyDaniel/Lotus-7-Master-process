/*=============================================================================
Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
All trademarks are owned by Enarka India Private Limited
============================================================================ */

/*==============================================================================
 @file  rte_hardware.h
 @author OHMLAP0112   
 @date 22-Mar-2022

 @brief Description
==============================================================================*/

#ifndef MODULE_RTE_INC_RTE_HARDWARE_H_
#define MODULE_RTE_INC_RTE_HARDWARE_H_

/*==============================================================================
 Includes
==============================================================================*/

#include "F28x_Project.h"
#include <stdint.h>

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
 Extern/Public Function Prototypes
==============================================================================*/
//void hw_init(void);
//void hw_reset(void);
void hw_enable_wdog(void);
void hw_disable_wdog(void);
void hw_service_wdog(void);
void hw_start_timers(void);
//void hw_peripheral_config_check(void);

/*==============================================================================
 Extern/Public Variables
==============================================================================*/

/*==============================================================================
 Extern/Public Constants
==============================================================================*/


#endif /* MODULE_RTE_INC_RTE_HARDWARE_H_ */