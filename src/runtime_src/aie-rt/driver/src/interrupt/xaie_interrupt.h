/******************************************************************************
* Copyright (C) 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*****************************************************************************/
/**
* @file xaie_interrupt.h
* @{
*
* Header file for AIE interrupt module.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------  -------- -----------------------------------------------------
* 1.0   Nishad  07/21/2020  Initial creation
* 1.1   Nishad  07/23/2020  Add APIs to configure second level interrupt
*			    controller.
* 1.2   Nishad  07/23/2020  Add API to initialize error broadcast network.
* 1.3   Nishad  08/13/2020  Add macro for error broadcast mask.
* </pre>
*
******************************************************************************/
#ifndef XAIE_INTERRUPT_H
#define XAIE_INTERRUPT_H

/***************************** Include Files *********************************/
#include "xaie_events.h"
#include "xaie_core.h"

/**************************** Type Definitions *******************************/
#define XAIE_ERROR_BROADCAST_ID			0x0U
#define XAIE_ERROR_BROADCAST_MASK		0x1U
#define XAIE_ERROR_SHIM_INTR_ID			0x10U
#define XAIE_ERROR_SHIM_INTR_MASK		0x10000U
#define XAIE_ERROR_NPI_INTR_ID			0x1U
#define XAIE_ERROR_L2_ENABLE			0x3FU

/************************** Function Prototypes  *****************************/
XAIE_AIG_EXPORT AieRC XAie_IntrCtrlL1Enable(XAie_DevInst *DevInst, XAie_LocType Loc,
		XAie_BroadcastSw Switch, u8 IntrId);
XAIE_AIG_EXPORT AieRC XAie_IntrCtrlL1Disable(XAie_DevInst *DevInst, XAie_LocType Loc,
		XAie_BroadcastSw Switch, u8 IntrId);
XAIE_AIG_EXPORT AieRC XAie_IntrCtrlL1IrqSet(XAie_DevInst *DevInst, XAie_LocType Loc,
		XAie_BroadcastSw Switch, u8 BroadcastId);
XAIE_AIG_EXPORT AieRC XAie_IntrCtrlL1Event(XAie_DevInst *DevInst, XAie_LocType Loc,
		XAie_BroadcastSw Switch, u8 IrqEventId, XAie_Events Event);
XAIE_AIG_EXPORT AieRC XAie_IntrCtrlL1BroadcastBlock(XAie_DevInst *DevInst, XAie_LocType Loc,
		XAie_BroadcastSw Switch, u32 ChannelBitMap);
XAIE_AIG_EXPORT AieRC XAie_IntrCtrlL1BroadcastUnblock(XAie_DevInst *DevInst, XAie_LocType Loc,
		XAie_BroadcastSw Switch, u32 ChannelBitMap);
XAIE_AIG_EXPORT AieRC XAie_IntrCtrlL2Enable(XAie_DevInst *DevInst, XAie_LocType Loc,
		u32 ChannelBitMap);
XAIE_AIG_EXPORT AieRC XAie_IntrCtrlL2Disable(XAie_DevInst *DevInst, XAie_LocType Loc,
		u32 ChannelBitMap);
XAIE_AIG_EXPORT AieRC XAie_ErrorHandlingInit(XAie_DevInst *DevInst);
XAIE_AIG_EXPORT void XAie_DisableErrorInterrupts(u8 IrqId);
XAIE_AIG_EXPORT void XAie_ClearErrorInterrupts(u8 IrqId);
XAIE_AIG_EXPORT u32 XAie_LIntrCtrlL2Status(XAie_LocType Loc);
XAIE_AIG_EXPORT AieRC XAie_BacktrackErrorInterrupts(XAie_DevInst *DevInst,
		XAie_ErrorMetaData *MData);
#endif		/* end of protection macro */
