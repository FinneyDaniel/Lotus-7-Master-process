/*=============================================================================
Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
All trademarks are owned by Enarka India Private Limited
============================================================================ */

/*==============================================================================
 @file  io.h
 @author OHMLAP0112   
 @date 22-Mar-2022

 @brief Description
==============================================================================*/

#ifndef MODULE_RTE_INC_RTE_IO_H_
#define MODULE_RTE_INC_RTE_IO_H_

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
void io_init(void);
void io_task(void);
uint16_t io_get_node_id(void);
//void io_self_discharge_relay_on(void);
//void io_self_discharge_relay_off(void);
//bool io_self_discharge_relay_get(void);
//void io_discharge_contactor_1_on(void);
//void io_discharge_contactor_1_off(void);
//bool io_discharge_contactor_1_get(void);
//void io_discharge_contactor_2_on(void);
//void io_discharge_contactor_2_off(void);
//bool io_discharge_contactor_2_get(void);
//void io_charge_relay_1_on(void);
//void io_charge_relay_1_off(void);
//bool io_charge_relay_1_get(void);
//void io_charge_relay_2_on(void);
//void io_charge_relay_2_off(void);
//bool io_charge_relay_2_get(void);
void io_fan_relay_on(void);
void io_fan_relay_off(void);
//void io_output_enable(void);
//void io_output_disable(void);
//void io_charge_stop_on(void);
//void io_charge_stop_off(void);
//void io_disable_act_bal_fault(void);
//void io_enable_act_bal_fault(void);
//uint16_t io_get_cell_over_volt(void);
long io_get_fan_freq(uint16_t ch);
void enable_loss_of_osc(void);
void disable_loss_of_osc(void);
void loss_of_osc_toggle(void);

/*==============================================================================
 Extern/Public Variables
==============================================================================*/

/*==============================================================================
 Extern/Public Constants
==============================================================================*/


#endif /* MODULE_RTE_INC_RTE_IO_H_ */
