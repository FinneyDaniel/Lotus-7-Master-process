/*=============================================================================
Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
All trademarks are owned by Enarka India Private Limited
============================================================================ */

/*==============================================================================
 @file  cana_vsc.h
 @author OHMLAP0107   
 @date 14-Jul-2022

 @brief Description
==============================================================================*/

#ifndef MODULE_APPLICATION_CANA_VSC_INC_CANA_VSC_H_
#define MODULE_APPLICATION_CANA_VSC_INC_CANA_VSC_H_

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
typedef struct CANA_zVSC
{
   uint16_t uiNodeID;
   uint16_t uiMsgType;

}CANA_tzVSC;

typedef struct CANA_zVSC_CELL_INFO
{
   float32_t f32Cellvolt[17];
   float32_t f32MaxCellVolt[2];
   float32_t f32MinCellVolt[2];
   uint16_t uiMaxcellNum[2];
   uint16_t uiMincellNum[2];

   float32_t f32Z_Real[17];
   float32_t f32Z_Img[17];
   uint32_t CANfailCnt;
   uint16_t isReceived;

}CANA_tzVSC_CELL_INFO;

extern CANA_tzVSC canA_VSCbuff;
extern CANA_tzVSC_CELL_INFO canA_tzVSC_info[17];

/*==============================================================================
 Macros
==============================================================================*/

/*==============================================================================
 Extern/Public Function Prototypes
==============================================================================*/
void cana_fnmsgPrcsVSC(uint16_t nodeID, uint16_t msgtype, uint16_t *msgbuf);
/*==============================================================================
 Extern/Public Variables
==============================================================================*/
extern void VSC_fnBrdcnt(void);
/*==============================================================================
 Extern/Public Constants
==============================================================================*/


#endif /* MODULE_APPLICATION_CANA_VSC_INC_CANA_VSC_H_ */
