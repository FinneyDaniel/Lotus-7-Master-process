/*
 * io.c
 *
 *  Created on: 28 Aug 2017
 *      Author: jmoodliar
 */

#include <rte_io.h>
#include "F28x_Project.h"
#include <stdint.h>
#include "rte_timer.h"

static bool loss_of_osc = false;
//static uint32_t fan_timer;

//static long calc_fan_freq(struct ECAP_REGS volatile * const p_ecap);
static long fan_freq[2];

void io_init(void)
{
	fan_freq[0] = fan_freq[1] = 0;
//	fan_timer = timer_get_tick() + DELAY_SECOND(1);
}

void io_task(void)
{
//	if(timer_is_expired(fan_timer))
//	{
//		fan_timer += DELAY_SECOND(1);
//
//		fan_freq[0] = calc_fan_freq(&ECap1Regs);
//		fan_freq[1] = calc_fan_freq(&ECap2Regs);
//	}
}
//
//uint16_t io_get_node_id(void)
//{
//	uint16_t node_id;
//
//	node_id = GpioDataRegs.GPADAT.bit.GPIO27 << 3;
//	node_id |= GpioDataRegs.GPADAT.bit.GPIO19 << 2;
//	node_id |= GpioDataRegs.GPADAT.bit.GPIO13 << 1;
//	node_id |= GpioDataRegs.GPADAT.bit.GPIO28;
//	node_id = (~node_id) & 0xFU;
//
//	return node_id;
//}
//
//void io_self_discharge_relay_on(void)
//{
//	GpioDataRegs.GPACLEAR.bit.GPIO6 = 1U;
//}
//
//void io_self_discharge_relay_off(void)
//{
//	GpioDataRegs.GPASET.bit.GPIO6 = 1U;
//}
//
//bool io_self_discharge_relay_get(void)
//{
//	bool bit = false;
//
//	if (!GpioDataRegs.GPADAT.bit.GPIO6)
//		bit = true;
//
//	return bit;
//}

//void io_discharge_contactor_1_on(void)
//{
//	GpioDataRegs.GPASET.bit.GPIO9 = 1U;
//}
//
//void io_discharge_contactor_1_off(void)
//{
//	GpioDataRegs.GPACLEAR.bit.GPIO9 = 1U;
//}
//
//bool io_discharge_contactor_1_get(void)
//{
//	bool bit = false;
//
//	if (GpioDataRegs.GPADAT.bit.GPIO9)
//		bit = true;
//
//	return bit;
//}
//
//void io_discharge_contactor_2_on(void)
//{
//	GpioDataRegs.GPASET.bit.GPIO2 = 1U;
//}
//
//void io_discharge_contactor_2_off(void)
//{
//	GpioDataRegs.GPACLEAR.bit.GPIO2 = 1U;
//}
//
//bool io_discharge_contactor_2_get(void)
//{
//	bool bit = false;
//
//	if (GpioDataRegs.GPADAT.bit.GPIO2)
//		bit = true;
//
//	return bit;
//}
//
//void io_charge_relay_1_on(void)
//{
//	GpioDataRegs.GPASET.bit.GPIO4 = 1U;
//}
//
//void io_charge_relay_1_off(void)
//{
//	GpioDataRegs.GPACLEAR.bit.GPIO4 = 1U;
//}
//
//bool io_charge_relay_1_get(void)
//{
//	bool bit = false;
//
//	if (GpioDataRegs.GPADAT.bit.GPIO4)
//		bit = true;
//
//	return bit;
//}
//
//void io_charge_relay_2_on(void)
//{
//	GpioDataRegs.GPASET.bit.GPIO8 = 1U;
//}
//
//void io_charge_relay_2_off(void)
//{
//	GpioDataRegs.GPACLEAR.bit.GPIO8 = 1U;
//}
//
//bool io_charge_relay_2_get(void)
//{
//	bool bit = false;
//
//	if (GpioDataRegs.GPADAT.bit.GPIO4)
//		bit = true;
//
//	return bit;
//}

void io_fan_relay_on(void)
{
	GpioDataRegs.GPACLEAR.bit.GPIO10 = 1U;
}

void io_fan_relay_off(void)
{
	GpioDataRegs.GPASET.bit.GPIO10 = 1U;
}
long io_get_fan_freq(uint16_t ch)
{
	if (ch < 2)
	{
		return fan_freq[ch];
	}

	return 0;
}

void enable_loss_of_osc(void)
{
	loss_of_osc = true;
}

void disable_loss_of_osc(void)
{
	loss_of_osc = false;
}

__attribute__((always_inline))
void loss_of_osc_toggle(void)
{
	if (loss_of_osc)
	{
		GpioDataRegs.GPATOGGLE.bit.GPIO17 = 1;
	}
}
