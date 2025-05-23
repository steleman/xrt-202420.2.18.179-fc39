/******************************************************************************
* Copyright (C) 2019 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*****************************************************************************/
/**
* @file xaie_locks_aie.c
* @{
*
* This file contains routines for AIE locks.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------  -------- -----------------------------------------------------
* 1.0   Tejus   03/17/2020  Initial creation
* 1.1   Tejus   06/10/2020  Switch to new io backend.
* </pre>
*
******************************************************************************/
/***************************** Include Files *********************************/
#include "xaie_feature_config.h"
#include "xaie_helper.h"
#include "xaie_locks.h"
#include "xaiegbl_defs.h"

#ifdef XAIE_FEATURE_LOCK_ENABLE

/************************** Constant Definitions *****************************/
#define XAIE_LOCK_WITH_VALUE_OFF	0x20U
#define XAIE_LOCK_RESULT_SUCCESS	1U
#define XAIE_LOCK_RESULT_LSB		0x0
#define XAIE_LOCK_RESULT_MASK		0x1

/************************** Function Definitions *****************************/
/*****************************************************************************/
/**
*
* This API is used to acquire the specified lock with or without value. Lock
* acquired without value if LockVal is (XAIE_LOCK_WTIH_NO_VALUE). Lock without
* value can be acquired for AIE only. This API can be blocking or non-blocking
* based on the TimeOut value. If the TimeOut is 0us, the API behaves in a
* non-blocking fashion and returns immediately after the first lock acquire
* request. If TimeOut > 0, then the API is a blocking call and will issue lock
* acquire request until the acquire is successful or it times out, whichever
* occurs first.
*
* @param	DevInst: Device Instance
* @param	LockMod: Internal lock module data structure.
* @param	Loc: Location of AIE Tile
* @param	Lock: Lock data structure with LockId and LockValue.
* @param	TimeOut: Timeout value for which the acquire request needs to be
*		repeated. Value in usecs.
*
* @return	XAIE_OK if Lock Acquired, else XAIE_LOCK_RESULT_FAILED.
*
* @note 	Internal API for AIE. This API should not be called directly.
*		It is invoked only using the function pointer part of the lock
*		module data structure.
*
******************************************************************************/
AieRC _XAie_LockAcquire(XAie_DevInst *DevInst, const XAie_LockMod *LockMod,
		XAie_LocType Loc, XAie_Lock Lock, u32 TimeOut, u8 BusyPoll)
{
	u64 RegAddr;
	u32 RegOff;
	AieRC Status = XAIE_OK;

	if(Lock.LockVal == XAIE_LOCK_WITH_NO_VALUE) {
		RegOff = LockMod->BaseAddr +
			(Lock.LockId * LockMod->LockIdOff) + LockMod->RelAcqOff;
	} else {
		RegOff = LockMod->BaseAddr + (Lock.LockId * LockMod->LockIdOff) +
			LockMod->RelAcqOff + XAIE_LOCK_WITH_VALUE_OFF +
			((u8)Lock.LockVal * LockMod->LockValOff);
	}

	RegAddr = XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col) + RegOff;

	if (BusyPoll != XAIE_ENABLE) {
		Status = XAie_MaskPoll(DevInst, RegAddr, XAIE_LOCK_RESULT_MASK,
			(XAIE_LOCK_RESULT_SUCCESS << XAIE_LOCK_RESULT_LSB), TimeOut);
	} else {
		Status = XAie_MaskPollBusy(DevInst, RegAddr, XAIE_LOCK_RESULT_MASK,
			(XAIE_LOCK_RESULT_SUCCESS << XAIE_LOCK_RESULT_LSB), TimeOut);
	}

	if (Status != XAIE_OK) {
		XAIE_DBG("Wait for lock acquire timed out\n");
		return XAIE_LOCK_RESULT_FAILED;
	}

	return Status;
}

/*****************************************************************************/
/**
*
* This API is used to release the specified lock with or without value. Lock is
* released without value if LockVal is (XAIE_LOCK_WITH_NO_VALUE). Lock without
* value can be released for AIE only. This API can be blocking or non-blocking
* based on the TimeOut value. If the TimeOut is 0us, the API behaves in a
* non-blocking fashion and returns immediately after the first lock release
* request. If TimeOut > 0, then the API is a blocking call and will issue lock
* release request until the release is successful or it times out, whichever
* occurs first.
*
* @param	DevInst: Device Instance
* @param	LockMod: Internal lock module data structure.
* @param	Loc: Location of AIE Tile
* @param	Lock: Lock data structure with LockId and LockValue.
* @param	TimeOut: Timeout value for which the release request needs to be
*		repeated. Value in usecs.
*
* @return	XAIE_OK if Lock Release, else XAIE_LOCK_RESULT_FAILED.
*
* @note 	Internal API for AIE. This API should not be called directly.
*		It is invoked only using the function pointer part of the lock
*		module data structure.
*
******************************************************************************/
AieRC _XAie_LockRelease(XAie_DevInst *DevInst, const XAie_LockMod *LockMod,
		XAie_LocType Loc, XAie_Lock Lock, u32 TimeOut, u8 BusyPoll)
{
	u64 RegAddr;
	u32 RegOff;
	AieRC Status = XAIE_OK;

	if(Lock.LockVal == XAIE_LOCK_WITH_NO_VALUE) {
		RegOff = LockMod->BaseAddr + (Lock.LockId * LockMod->LockIdOff);
	} else {
		RegOff = LockMod->BaseAddr +
			(Lock.LockId * LockMod->LockIdOff) +
			XAIE_LOCK_WITH_VALUE_OFF +
			((u8)Lock.LockVal * LockMod->LockValOff);
	}

	RegAddr = XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col) + RegOff;

	if (BusyPoll != XAIE_ENABLE) {
		Status = XAie_MaskPoll(DevInst, RegAddr, XAIE_LOCK_RESULT_MASK,
			(XAIE_LOCK_RESULT_SUCCESS << XAIE_LOCK_RESULT_LSB), TimeOut);
	} else {
		Status = XAie_MaskPollBusy(DevInst, RegAddr, XAIE_LOCK_RESULT_MASK,
			(XAIE_LOCK_RESULT_SUCCESS << XAIE_LOCK_RESULT_LSB), TimeOut);
	}

	if (Status != XAIE_OK) {
		XAIE_DBG("Wait for lock release timed out\n");
		return XAIE_LOCK_RESULT_FAILED;
	}

	return Status;
}

/*****************************************************************************/
/**
*
* This API is used to get Lock Status Value.
*
* @param	DevInst: Device Instance
* @param	LockMod: Internal lock module data structure.
* @param	Loc: Location of AIE Tile
* @param	Lock: Lock data structure with LockId and LockValue.
* @param	LockVal: Lock Value.
*
* @return	XAIE_OK if Lock Release, else error code.
*
* @note		Internal only.
*
******************************************************************************/
AieRC _XAie_LockGetValue(XAie_DevInst *DevInst, const XAie_LockMod *LockMod,
		XAie_LocType Loc, XAie_Lock Lock, u32 *LockVal)
{
	(void)DevInst;
	(void)LockMod;
	(void)Loc;
	(void)Lock;
	(void)LockVal;

	return XAIE_FEATURE_NOT_SUPPORTED;
}

/*****************************************************************************/
/**
*
* This API is used to initalize a lock with given value.
*
* @param	DevInst: Device Instance
* @param	LockMod: Internal lock module data structure.
* @param	Loc: Location of AIE Tile
* @param	Lock: Lock data structure with LockId and LockValue.
*
* @return	XAIE_OK if Lock Release, else error code.
*
* @note 	Internal only.
*
******************************************************************************/
AieRC _XAie_LockSetValue(XAie_DevInst *DevInst, const XAie_LockMod *LockMod,
		XAie_LocType Loc, XAie_Lock Lock)
{
	(void)DevInst;
	(void)LockMod;
	(void)Loc;
	(void)Lock;

	return XAIE_FEATURE_NOT_SUPPORTED;
}

#endif /* XAIE_FEATURE_LOCK_ENABLE */
/** @} */
