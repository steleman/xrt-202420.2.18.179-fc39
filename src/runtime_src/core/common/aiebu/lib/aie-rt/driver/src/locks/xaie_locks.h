/******************************************************************************
* Copyright (C) 2019 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*****************************************************************************/
/**
* @file xaie_locks.h
* @{
*
* This file contains routines for AIE tile control.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------  -------- -----------------------------------------------------
* 1.0   Tejus   03/17/2020  Initial creation
* 1.1   Tejus   03/23/2020  Include xaiegbl header file
* </pre>
*
******************************************************************************/
#ifndef XAIELOCKS_H
#define XAIELOCKS_H

/***************************** Include Files *********************************/
#include "xaiegbl.h"
#include "xaiegbl_defs.h"
/**************************** Type Definitions *******************************/
/************************** Function Prototypes  *****************************/
XAIE_AIG_EXPORT AieRC XAie_LockAcquire(XAie_DevInst *DevInst, XAie_LocType Loc, XAie_Lock Lock,
		u32 TimeOut);
XAIE_AIG_EXPORT AieRC XAie_LockAcquireBusy(XAie_DevInst *DevInst, XAie_LocType Loc, XAie_Lock Lock,
		u32 TimeOut);
XAIE_AIG_EXPORT AieRC XAie_LockRelease(XAie_DevInst *DevInst, XAie_LocType Loc, XAie_Lock Lock,
		u32 TimeOut);
XAIE_AIG_EXPORT AieRC XAie_LockReleaseBusy(XAie_DevInst *DevInst, XAie_LocType Loc, XAie_Lock Lock,
		u32 TimeOut);
XAIE_AIG_EXPORT AieRC XAie_LockSetValue(XAie_DevInst *DevInst, XAie_LocType Loc,
		XAie_Lock Lock);
XAIE_AIG_EXPORT AieRC XAie_LockGetValue(XAie_DevInst *DevInst, XAie_LocType Loc, XAie_Lock Lock,
		u32 *LockValue);
#endif		/* end of protection macro */
