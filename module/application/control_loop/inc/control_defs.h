/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  oi_controlloop.h
 @author DEEPTI.K
 @date 06-Sep-2021

 @brief Description
 ==============================================================================*/

#ifndef MODULE_CONTROL_LOOP_INC_CONTROL_DEFS_H_
#define MODULE_CONTROL_LOOP_INC_CONTROL_DEFS_H_

/*==============================================================================
 Includes
 ==============================================================================*/

#include "F28x_Project.h"
#include <stdint.h>
#include "inc/hw_types.h"

/*==============================================================================
 Defines
 ==============================================================================*/

#define GPIO_SET            (0x01U)
#define GPIO_CLEAR          (0x0U)

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/
struct CONTROL_FLT_REG
{
    Uint16 VFDPumpFail:1;
    Uint16 WaterlvlsFail:1;
    Uint16 LPCCurHealthy:1;
     Uint16 xtnalFlt:1;
    Uint16 StackcellFault:1;
    Uint16 Rsvd2:1;
    Uint16 Rsvd3:1;
    Uint16 Rsvd4:1;
};

union CONTROL_FLTS_REG
{
    Uint16 all;
    struct CONTROL_FLT_REG bit;
};

typedef struct
{
    float32 Ref;             // Input: reference set-point
    float32 Fbk;             // Input: feedback
    float32 Out;             // Output: controller output
    float32 Kp;              // Parameter: proportional loop gain
    float32 TsKi;            // Parameter: integral gain * sampling time
    float32 Umax;            // Parameter: upper saturation limit
    float32 Umin;            // Parameter: lower saturation limit
    float32 up;              // Data: proportional term
    float32 ui;              // Data: integral term
    float32 v1;              // Data: pre-saturated controller output
    float32 i1;              // Data: integrator storage: ui(k-1)
    float32 w1;              // Data: saturation record: [u(k-1) - v(k-1)]
} PI_CONTROLLER;

#define PI_MACRO(v)                                           \
                                                                \
    /* proportional term */                                     \
    v.up = v.Fbk - v.Ref;                                       \
                                                                \
    /* integral term */                                         \
    v.ui = (v.Out == v.v1)?( (v.TsKi * v.up)+ v.i1) : ( (v.TsKi * ( v.up + ( ui16PIgain*(v.Out-v.v1) )) ) + v.i1); \
/*    v.ui = (v.Out == v.v1)?( (v.TsKi * v.up)+ v.i1) : v.i1;*/     \
    if(v.ui > 8) \
        { \
            v.i1 = 8; \
        } \
    else if(v.ui < 0) \
        { \
             v.i1 = 0; \
         } \
         else v.i1 = v.ui; \
                                                                \
    /* control output */                                        \
    v.v1 = (v.Kp * v.up) + v.i1;                           \
    /* Saturate PI Output */    \
    if(v.v1 > v.Umax) \
        { \
            v.Out = v.Umax; \
        } \
    else if(v.v1 < v.Umin) \
        { \
             v.Out = v.Umin; \
         } \
         else v.Out = v.v1;\

#define RESET_PI(v)                 \
    v.i1 = 0;     \
    v.v1 = 0;         \
    v.Out = 0;               \
    v.ui = 0;               \


// Dryer Section


struct H2_tzVAL
{
    uint16_t bt_Svalve1:1;
    uint16_t bt_Svalve2:1;
    uint16_t bt_Svalve3:1;
    uint16_t bt_Svalve4:1;
    uint16_t bt_Svalve5:1;
    uint16_t bt_Svalve6:1;
    uint16_t bt_Svalve7:1;
    uint16_t unused:1;

};

union H2_tzVALVES
{
    uint16_t all;
    struct H2_tzVAL bit;

};

struct H2_tzVAL_PHS_SEP
{
    uint16_t bt_Svalve8:1;
    uint16_t bt_Svalve9:1;
    uint16_t bt_Svalve10:1;
    uint16_t bt_Svalve11:1;
    uint16_t bt_Svalve12:1;
    uint16_t unused:1;
    uint16_t unused1:1;
    uint16_t unused2:1;

};

union H2_tzVALVES_PHS_SEP
{
    uint16_t all;
    struct H2_tzVAL_PHS_SEP bit;

};

/*==============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Extern/Public Function Prototypes
 ==============================================================================*/



extern void control_waterloop(void);
extern void control_waterloop_stopped(void);
extern void CONTROL_fnPSU_IRef(void);
extern void H2_fnSVcontrol(void);
extern void CONTROL_fnGlycolLoop(void);
extern void CONTROL_DryerOutletValveLogic();
extern void CONTROL_DryerValveBleedLogic();
extern void CONTROL_ReclaimTank(void);
extern void CONTROL_PhaseSeparator(void);
extern void CONTROL_DryerOutlet(void);

/*==============================================================================
 Extern/Public Variables
 ==============================================================================*/

extern union CONTROL_FLTS_REG CONTROLtzFaultRegs;


extern uint16_t ui16SafeShutDownFlg;
extern uint16_t ui16InstShutDownFlg;
extern uint16_t ui16Bleedh2,ui16StH2CmdFrmMSFlg,ui16CycleCount;
extern uint32_t ui32NodePresent;
extern float32_t f32EBVOutvalue;
extern uint32_t ui16H2count,val1_2Cnt;
extern uint16_t eepwritecount;
extern uint16_t epromStAdd;
extern uint16_t uiepromAdd;
//extern union H2_tzVALVES H2_tzValves_PHS;
/*==============================================================================
 Extern/Public Constants
 ==============================================================================*/

#endif /* MODULE_CONTROL_LOOP_INC_CONTROL_DEFS_H_ */
