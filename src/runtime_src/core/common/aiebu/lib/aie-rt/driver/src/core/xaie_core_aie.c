/******************************************************************************
* Copyright (C) 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*****************************************************************************/
/**
* @file xaie_core_aie.c
* @{
*
* This file contains routines for AIE core control..
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------  -------- -----------------------------------------------------
* 1.0   Tejus   10/02/2020  Initial creation
* </pre>
*
******************************************************************************/
/***************************** Include Files *********************************/
#ifdef __linux__
#include <unistd.h>
#endif

#include "xaie_core_aie.h"
#include "xaie_events_aie.h"
#include "xaie_feature_config.h"

#ifdef XAIE_FEATURE_CORE_ENABLE

/************************** Constant Definitions *****************************/

/************************** Function Definitions *****************************/
/*****************************************************************************/
/*
*
* This API configures the enable events register with disable event. This
* configuration will be used to check if the core has triggered the disable
* event.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the aie tile.
* @param	CoreMod: Pointer to the core module data structure.
*
* @return	XAIE_OK on success, Error code on failure.
*
* @note		Internal only.
*
******************************************************************************/
AieRC _XAie_CoreConfigureDone(XAie_DevInst *DevInst, XAie_LocType Loc,
		const struct XAie_CoreMod *CoreMod)
{
	u32 Value, Mask;
	u64 RegAddr;

	Mask = CoreMod->CoreEvent->DisableEvent.Mask |
		CoreMod->CoreEvent->DisableEventOccurred.Mask |
		CoreMod->CoreEvent->EnableEventOccurred.Mask;
	Value = (u32)(XAIE_EVENTS_CORE_INSTR_EVENT_2 <<
		CoreMod->CoreEvent->DisableEvent.Lsb);
	RegAddr = CoreMod->CoreEvent->EnableEventOff +
		XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col);

	return XAie_MaskWrite32(DevInst, RegAddr, Mask, Value);
}

/*****************************************************************************/
/*
*
* This API writes to the Core control register of a tile to enable the core.
* Any gracefulness required in enabling/disabling the core are required to be
* handled by the application layer.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the AIE tile.
* @param	CoreMod: Pointer to the core module data structure.
*
* @return	XAIE_OK on success, Error code on failure.
*
* @note		Internal only.
*
******************************************************************************/
AieRC _XAie_CoreEnable(XAie_DevInst *DevInst, XAie_LocType Loc,
		const struct XAie_CoreMod *CoreMod)
{
	AieRC RC;
	u32 Mask, Value;
	u64 RegAddr;

	/* Clear the disable event occurred bit */
	Mask = CoreMod->CoreEvent->DisableEventOccurred.Mask |
		CoreMod->CoreEvent->EnableEventOccurred.Mask;
	Value = (u32)(1U << CoreMod->CoreEvent->DisableEventOccurred.Lsb);
	RegAddr = CoreMod->CoreEvent->EnableEventOff +
		XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col);

	RC = XAie_MaskWrite32(DevInst, RegAddr, Mask, Value);
	if(RC != XAIE_OK) {
		return RC;
	}

	/* Enable the core */
	Mask = CoreMod->CoreCtrl->CtrlEn.Mask;
	Value = (u32)(1U << CoreMod->CoreCtrl->CtrlEn.Lsb);
	RegAddr = CoreMod->CoreCtrl->RegOff +
		XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col);

	return XAie_MaskWrite32(DevInst, RegAddr, Mask, Value);
}

/*****************************************************************************/
/*
*
* This API implements a blocking wait function to check the core to be in
* done state for a AIE tile. API comes out of the loop when core status
* changes to done or the timeout elapses, whichever happens first.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the AIE tile.
* @param	TimeOut: TimeOut in usecs. If set to 0, the default timeout will
*		be set to 500us. The TimeOut value passed is per tile.
* @param	CoreMod: Pointer to the core module data structure.
*
* @return	XAIE_OK on success, Error code on failure.
*
* @note		Internal only.
*
******************************************************************************/
AieRC _XAie_CoreWaitForDone(XAie_DevInst *DevInst, XAie_LocType Loc,
		u32 TimeOut, const struct XAie_CoreMod *CoreMod, u8 BusyPoll)
{
	u32 Mask, Value;
	u64 EventRegAddr;
	AieRC Status = XAIE_OK;

	Mask = CoreMod->CoreEvent->DisableEventOccurred.Mask;
	Value = (u32)(1U << CoreMod->CoreEvent->DisableEventOccurred.Lsb);
	EventRegAddr = CoreMod->CoreEvent->EnableEventOff +
		XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col);

	if (BusyPoll != XAIE_ENABLE){
		Status = XAie_MaskPoll(DevInst, EventRegAddr, Mask, Value, TimeOut);
	} else {
		Status = XAie_MaskPollBusy(DevInst, EventRegAddr, Mask, Value, TimeOut);
	}

	if (Status != XAIE_OK) {
		XAIE_DBG("Core Wait Done poll time out\n");
		return XAIE_CORE_STATUS_TIMEOUT;
	}

	return Status;
}

/*****************************************************************************/
/*
*
* This API reads the Done bit value in the core status register.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the AIE tile.
* @param	DoneBit: Pointer to store the value of Done bit. Returns 1 if
*		Done bit is set, 0 otherwise.
* @param	CoreMod: Pointer to the core module data structure.
*
* @return	XAIE_OK on success, Error code on failure.
*
* @note		Internal only.
*
******************************************************************************/
AieRC _XAie_CoreReadDoneBit(XAie_DevInst *DevInst, XAie_LocType Loc,
		u8 *DoneBit, const struct XAie_CoreMod *CoreMod)
{
	AieRC RC;
	u64 RegAddr;
	u32 EventReg;

	/* Read enable events register */
	RegAddr = CoreMod->CoreEvent->EnableEventOff +
		XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col);
	RC = XAie_Read32(DevInst, RegAddr, &EventReg);
	if(RC != XAIE_OK) {
		return RC;
	}

	EventReg = XAie_GetField(EventReg,
			CoreMod->CoreEvent->DisableEventOccurred.Lsb,
			CoreMod->CoreEvent->DisableEventOccurred.Mask);

	*DoneBit = (EventReg == 1U) ? 1U : 0U;
	return XAIE_OK;
}

/*****************************************************************************/
/*
*
* This API reads the core status register value.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the AIE tile.
* @param	CoreStatus: Pointer to store the core status register value.
* @param	CoreMod: Pointer to the core module data structure.
*
* @return	XAIE_OK on success, Error code on failure.
*
* @note		Internal only.
*
******************************************************************************/
AieRC _XAie_CoreGetStatus(XAie_DevInst *DevInst, XAie_LocType Loc,
		u32 *CoreStatus, const struct XAie_CoreMod *CoreMod)
{
	AieRC RC;
	u64 RegAddr;
	u32 RegVal;
	u8 DoneBit;

	/* Read core status register */
	RegAddr = CoreMod->CoreSts->RegOff +
		XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col);
	RC = XAie_Read32(DevInst, RegAddr, &RegVal);
	if(RC != XAIE_OK) {
		return RC;
	}

	/* Read Done bit in the core status register */
	RC = _XAie_CoreReadDoneBit(DevInst, Loc, &DoneBit, CoreMod);
	if(RC != XAIE_OK) {
		return RC;
	}

	*CoreStatus = XAie_GetField(RegVal, 0U, CoreMod->CoreSts->Mask);
	*CoreStatus |= XAie_SetField(DoneBit, CoreMod->CoreSts->Done.Lsb,
			CoreMod->CoreSts->Done.Mask);

	return XAIE_OK;
}

#endif /* XAIE_FEATURE_CORE_ENABLE */
/** @} */
