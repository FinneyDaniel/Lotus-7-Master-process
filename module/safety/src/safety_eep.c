/*=============================================================================
Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
All trademarks are owned by Enarka India Private Limited
============================================================================ */

/*==============================================================================
 @file  safety_eep.c
 @author JOTHI RAMESH
 @date 26-Jan-2022

 @brief Description
==============================================================================*/

/*==============================================================================
 Includes
==============================================================================*/

#include "F28x_Project.h"
#include <stdint.h>
#include "safety_lib.h"
#include "safety_lib_prv.h"
#include "eep.h"
/*==============================================================================
 Defines
==============================================================================*/
volatile Uint16 timeout_count1;
#define I2C_DELAY1           5000
#define I2C_WAIT_UNTIL1(x)   { \
                                timeout_count1 = 0; \
                                while((x)) \
                                { \
                                    timeout_count1++; \
                                    if (timeout_count1 >= I2C_DELAY1) \
                                        { \
                                        return 0; \
                                        } \
                                } \
                            } \
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


bool eep_read_word(uint16_t address, uint16_t *data, uint16_t bytes);
bool eep_write_word(uint16_t address, uint16_t *data, uint16_t bytes);
/*==============================================================================
 Local Variables
==============================================================================*/

/*==============================================================================
 Local Constants
==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
bool eep_read_word(uint16_t address, uint16_t *data, uint16_t bytes)
{
    uint16_t i = 0;

       I2C_WAIT_UNTIL1(I2caRegs.I2CMDR.bit.STP == 1);

       I2caRegs.I2CSAR.all = 0x50;

       // If we are in the the address setup phase, send the address without a
       // stop condition.
       //
       // Check if bus busy
       I2C_WAIT_UNTIL1(I2caRegs.I2CSTR.bit.BB == 1);

       I2caRegs.I2CCNT = 2;

       I2caRegs.I2CDXR.all = (address >> 8) & 0xFF;
       I2caRegs.I2CDXR.all =  (address >> 0) & 0xFF;

       I2caRegs.I2CMDR.all = 0x6620;

       I2C_WAIT_UNTIL1(I2caRegs.I2CSTR.bit.ARDY != 1);

       if (I2caRegs.I2CSTR.bit.NACK == 1)
       {
           I2caRegs.I2CMDR.bit.STP = 1;
           I2caRegs.I2CSTR.all = (1U << 1);
           return false;
       }

       I2caRegs.I2CCNT = bytes;

       I2caRegs.I2CMDR.all = 0x6C20;

       if (I2caRegs.I2CSTR.bit.NACK == 1)
       {
           I2caRegs.I2CMDR.bit.STP = 1;
           I2caRegs.I2CSTR.all = (1U << 1);
           return false;
       }

       I2C_WAIT_UNTIL1(I2caRegs.I2CSTR.bit.SCD == 0);

       DELAY_US(5000);

       for (i = 0; i < bytes; i++)
       {
           data[i] = I2caRegs.I2CDRR.all;
       }

       return true;

}
/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */

bool eep_write_word(uint16_t address, uint16_t *data, uint16_t bytes)
{
    uint16_t i = 0;

        I2C_WAIT_UNTIL1(I2caRegs.I2CMDR.bit.STP == 1);

        I2caRegs.I2CSAR.all = 0x50;

        //
        // Check if bus busy
        //
        I2C_WAIT_UNTIL1(I2caRegs.I2CSTR.bit.BB == 1);

        I2caRegs.I2CCNT = bytes + 2;

        I2caRegs.I2CDXR.all = (address >> 8) & 0xFF;
        I2caRegs.I2CDXR.all =  (address >> 0) & 0xFF;

        for (i = 0; i < bytes; i++)
        {
            I2caRegs.I2CDXR.all = data[i];
        }

        //
        // Send start as master transmitter
        //
        I2caRegs.I2CMDR.all = 0x6E20;
        DELAY_US(5000);

        I2C_WAIT_UNTIL1(I2caRegs.I2CSTR.bit.SCD == 0);

        return true;
}
/****************************************************************************/
//__attribute__((always_inline))
//static void i2c_generate_stop(void)
//{
//    volatile int delay = 1000U;
//    I2caRegs.I2CMDR.all = 0x4E20U;
//    for ( delay = 1000U; delay; delay--)
//        ;
//    I2caRegs.I2CMDR.all = 0;
//}
 /*==============================================================================
 End of File
==============================================================================*/

/*==============================================================================
 End of File
==============================================================================*/
