/******************************************************************************
* Copyright (C) 2019 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*****************************************************************************/
/**
* @file xaie_core.c
* @{
*
* This file contains routines for AIE tile control.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------  -------- -----------------------------------------------------
* 1.0   Tejus   09/24/2019  Initial creation
* 1.1   Tejus	01/04/2020  Cleanup error messages
* 1.2   Tejus   03/20/2020  Reorder functions
* 1.3   Tejus   03/20/2020  Make internal functions static
* 1.4   Tejus   04/13/2020  Remove range apis and change to single tile apis
* 1.5   Tejus   06/01/2020  Add core debug halt apis.
* 1.6   Tejus   06/01/2020  Add api to read core done bit.
* 1.7   Tejus   06/05/2020  Change core enable/disable api to mask write.
* 1.8   Tejus   06/05/2020  Add api to reset/unreset aie cores.
* 1.9   Tejus   06/05/2020  Add null check for DevInst in core status apis.
* 2.0   Tejus   06/10/2020  Switch to new io backend apis.
* </pre>
*
******************************************************************************/
/***************************** Include Files *********************************/
#include "xaie_core.h"
#include "xaie_events.h"
#include "xaie_feature_config.h"

#ifdef XAIE_FEATURE_CORE_ENABLE

#include "xaie_helper_internal.h"
/************************** Constant Definitions *****************************/
#define XAIETILE_CORE_STATUS_DEF_WAIT_USECS 500U

/************************** Function Definitions *****************************/
/*****************************************************************************/
/*
*
* This API implements a blocking wait function to check the core status for a
* range of AIE Tiles for a given Mask and Value. API comes out of the wait loop
* when the status changes to the Mask, Value pair or the timeout elapses,
* whichever happens first.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the AIE tile.
* @param	TimeOut: TimeOut in usecs. If set to 0, the default timeout will
*		be set to 500us.
* @param	Mask: Mask for the core status register.
* @param	Value: Value for the core status register.
*
* @return	XAIE_OK on success, Error code on failure.
*
* @note		Internal API only.
*
******************************************************************************/
static AieRC _XAie_CoreWaitStatus(XAie_DevInst *DevInst, XAie_LocType Loc,
		u32 TimeOut, u32 Mask, u32 Value, u8 BusyPoll)
{

	u64 RegAddr;
	const XAie_CoreMod *CoreMod;
	u8 TileType;
	AieRC Status = XAIE_OK;

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	CoreMod = DevInst->DevProp.DevMod[TileType].CoreMod;

	/* TimeOut passed by the user is per Core */
	if(TimeOut == 0U) {
		/* Set timeout to default value */
		TimeOut = XAIETILE_CORE_STATUS_DEF_WAIT_USECS;
	}

	RegAddr = CoreMod->CoreSts->RegOff +
		XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col);

	if (BusyPoll != XAIE_ENABLE){
		Status = XAie_MaskPoll(DevInst, RegAddr, Mask, Value, TimeOut);
	} else {
		Status = XAie_MaskPollBusy(DevInst, RegAddr, Mask, Value, TimeOut);
	}

	if (Status != XAIE_OK) {
		XAIE_DBG("Status poll time out\n");
		return XAIE_CORE_STATUS_TIMEOUT;
	}

	return Status;
}

/*****************************************************************************/
/*
*
* This API writes to the Core control register of a tile to disable the core.
* Any gracefulness required in enabling/disabling the core are required to be
* handled by the application layer.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of AIE Tiles
*
* @return	XAIE_OK on success, Error code on failure.
*
* @note		None.
*
******************************************************************************/
AieRC XAie_CoreDisable(XAie_DevInst *DevInst, XAie_LocType Loc)
{
	u8 TileType;
	u32 Mask, Value;
	u64 RegAddr;
	const XAie_CoreMod *CoreMod;

	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	CoreMod = DevInst->DevProp.DevMod[TileType].CoreMod;

	Mask = CoreMod->CoreCtrl->CtrlEn.Mask;
	Value = (u32)(0U << CoreMod->CoreCtrl->CtrlEn.Lsb);
	RegAddr = CoreMod->CoreCtrl->RegOff +
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
*
* @return	XAIE_OK on success, Error code on failure.
*
* @note		None.
*
******************************************************************************/
AieRC XAie_CoreEnable(XAie_DevInst *DevInst, XAie_LocType Loc)
{
	u8 TileType;
	const XAie_CoreMod *CoreMod;

	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	CoreMod = DevInst->DevProp.DevMod[XAIEGBL_TILE_TYPE_AIETILE].CoreMod;

	return CoreMod->Enable(DevInst, Loc, CoreMod);
}

/*****************************************************************************/
/*
*
* This API writes to the Core control register of a tile to reset the core.
* Any gracefulness required in reset/unreset of the core are required to be
* handled by the application layer.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the AIE tile.
*
* @return	XAIE_OK on success, Error code on failure.
*
* @note		None.
*
******************************************************************************/
AieRC XAie_CoreReset(XAie_DevInst *DevInst, XAie_LocType Loc)
{
	u8 TileType;
	u32 Mask, Value;
	u64 RegAddr;
	const XAie_CoreMod *CoreMod;

	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	CoreMod = DevInst->DevProp.DevMod[XAIEGBL_TILE_TYPE_AIETILE].CoreMod;
	if ((_XAie_CheckPrecisionExceeds(CoreMod->CoreCtrl->CtrlRst.Lsb,
			_XAie_MaxBitsNeeded(1U),MAX_VALID_AIE_REG_BIT_INDEX))) {
		XAIE_ERROR("Check Precision Exceeds Failed\n");
		return XAIE_ERR;
	}
	Mask = CoreMod->CoreCtrl->CtrlRst.Mask;
	Value = (u32)(1U << CoreMod->CoreCtrl->CtrlRst.Lsb);
	RegAddr = CoreMod->CoreCtrl->RegOff +
		XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col);

	return XAie_MaskWrite32(DevInst, RegAddr, Mask, Value);
}

/*****************************************************************************/
/*
*
* This API writes to the Core control register of a tile to unreset the core.
* Any gracefulness required in reset/unreset of the core are required to be
* handled by the application layer.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the AIE tile.
*
* @return	XAIE_OK on success, Error code on failure.
*
* @note		None.
*
******************************************************************************/
AieRC XAie_CoreUnreset(XAie_DevInst *DevInst, XAie_LocType Loc)
{
	u8 TileType;
	u32 Mask, Value;
	u64 RegAddr;
	const XAie_CoreMod *CoreMod;

	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}


	CoreMod = DevInst->DevProp.DevMod[XAIEGBL_TILE_TYPE_AIETILE].CoreMod;
	Mask = CoreMod->CoreCtrl->CtrlRst.Mask;
	Value = (u32)(0U << CoreMod->CoreCtrl->CtrlRst.Lsb);
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
* @return	XAIE_OK on success, Error code on failure.
*
* @note		This API in context of TXN flow will be a yeilded poll wait.
*
******************************************************************************/
AieRC XAie_CoreWaitForDone(XAie_DevInst *DevInst, XAie_LocType Loc, u32 TimeOut)
{
	u8 TileType;
	const XAie_CoreMod *CoreMod;

	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	CoreMod = DevInst->DevProp.DevMod[XAIEGBL_TILE_TYPE_AIETILE].CoreMod;

	/* TimeOut passed by the user is per Core */
	if(TimeOut == 0U) {
		/* Set timeout to default value */
		TimeOut = XAIETILE_CORE_STATUS_DEF_WAIT_USECS;
	}

	return CoreMod->WaitForDone(DevInst, Loc, TimeOut, CoreMod, XAIE_DISABLE);
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
* @return	XAIE_OK on success, Error code on failure.
*
* @note		This API in context of TXN flow will be a busy poll wait.
*
******************************************************************************/
AieRC XAie_CoreWaitForDoneBusy(XAie_DevInst *DevInst, XAie_LocType Loc, u32 TimeOut)
{
	u8 TileType;
	const XAie_CoreMod *CoreMod;

	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	CoreMod = DevInst->DevProp.DevMod[XAIEGBL_TILE_TYPE_AIETILE].CoreMod;

	/* TimeOut passed by the user is per Core */
	if(TimeOut == 0U) {
		/* Set timeout to default value */
		TimeOut = XAIETILE_CORE_STATUS_DEF_WAIT_USECS;
	}

	return CoreMod->WaitForDone(DevInst, Loc, TimeOut, CoreMod, XAIE_ENABLE);
}

/*****************************************************************************/
/*
*
* This API implements a blocking wait function to check the core to be in
* disable state for a AIE tile. API comes out of the loop when core status
* changes to disable or the timeout elapses, whichever happens first.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the AIE tile.
* @param	TimeOut: TimeOut in usecs. If set to 0, the default timeout will
*		be set to 500us. The TimeOut value passed is per tile.
* @return	XAIE_OK on success, Error code on failure.
*
* @note		This API in context of TXN flow will be a yeilded poll wait.
*
******************************************************************************/
AieRC XAie_CoreWaitForDisable(XAie_DevInst *DevInst, XAie_LocType Loc,
		u32 TimeOut)
{
	const XAie_CoreMod *CoreMod;
	u32 Mask;
	u32 Value;

	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	CoreMod = DevInst->DevProp.DevMod[XAIEGBL_TILE_TYPE_AIETILE].CoreMod;
	Mask = CoreMod->CoreSts->En.Mask;
	Value = (u32)(0U << CoreMod->CoreSts->En.Lsb);
	return _XAie_CoreWaitStatus(DevInst, Loc, TimeOut, Mask, Value,
				XAIE_DISABLE);
}

/*****************************************************************************/
/*
*
* This API implements a blocking wait function to check the core to be in
* disable state for a AIE tile. API comes out of the loop when core status
* changes to disable or the timeout elapses, whichever happens first.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the AIE tile.
* @param	TimeOut: TimeOut in usecs. If set to 0, the default timeout will
*		be set to 500us. The TimeOut value passed is per tile.
* @return	XAIE_OK on success, Error code on failure.
*
* @note		This API in context of TXN flow will be a busy poll wait.
*
******************************************************************************/
AieRC XAie_CoreWaitForDisableBusy(XAie_DevInst *DevInst, XAie_LocType Loc,
		u32 TimeOut)
{
	const XAie_CoreMod *CoreMod;
	u32 Mask;
	u32 Value;

	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	CoreMod = DevInst->DevProp.DevMod[XAIEGBL_TILE_TYPE_AIETILE].CoreMod;
	Mask = CoreMod->CoreSts->En.Mask;
	Value = (u32)(0U << CoreMod->CoreSts->En.Lsb);
	return _XAie_CoreWaitStatus(DevInst, Loc, TimeOut, Mask, Value,
				XAIE_ENABLE);
}

/*****************************************************************************/
/*
*
* This API writes to the Core debug control register to control the debug halt
* bit.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the AIE tile.
* @param	Enable: Enable/Disable the debug halt bit(1- Enable, 0-Disable).
* @return	XAIE_OK on success, Error code on failure.
*
* @note		Internal only.
*
******************************************************************************/
static AieRC _XAie_CoreDebugCtrlHalt(XAie_DevInst *DevInst, XAie_LocType Loc,
		u8 Enable)
{
	u64 RegAddr;
	const XAie_CoreMod *CoreMod;
	u8 TileType;

	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	CoreMod = DevInst->DevProp.DevMod[TileType].CoreMod;

	RegAddr = CoreMod->CoreDebug->RegOff +
		XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col);

	return XAie_MaskWrite32(DevInst, RegAddr,
			CoreMod->CoreDebug->DebugHalt.Mask, Enable);
}

/*****************************************************************************/
/*
*
* This API writes to the Core debug control register to enable the debug halt
* bit.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the AIE tile.
* @return	XAIE_OK on success, Error code on failure.
*
* @note		None.
*
******************************************************************************/
AieRC XAie_CoreDebugHalt(XAie_DevInst *DevInst, XAie_LocType Loc)
{
	return _XAie_CoreDebugCtrlHalt(DevInst, Loc, XAIE_ENABLE);
}

/*****************************************************************************/
/*
*
* This API writes to the Core debug control register to disable the debug halt
* bit.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the AIE tile.
* @return	XAIE_OK on success, Error code on failure.
*
* @note		None.
*
******************************************************************************/
AieRC XAie_CoreDebugUnhalt(XAie_DevInst *DevInst, XAie_LocType Loc)
{
	return _XAie_CoreDebugCtrlHalt(DevInst, Loc, XAIE_DISABLE);
}

/*****************************************************************************/
/*
*
* This API reads the status from the debug halt status register of AIE.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the AIE tile.
* @param	DebugStatus: Pointer to store status.
* @return	XAIE_OK on success, Error code on failure.
*
* @note		None.
*
******************************************************************************/
AieRC XAie_CoreGetDebugHaltStatus(XAie_DevInst *DevInst, XAie_LocType Loc,
		u32 *DebugStatus)
{
	AieRC RC;
	u8 TileType;
	u32 Mask;
	u64 RegAddr;
	const XAie_CoreMod *CoreMod;
	const XAie_RegCoreDebugStatus *DbgStat;

	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	CoreMod = DevInst->DevProp.DevMod[TileType].CoreMod;

	RegAddr = CoreMod->CoreDebugStatus->RegOff +
		XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col);

	RC = XAie_Read32(DevInst, RegAddr, DebugStatus);
	if(RC != XAIE_OK) {
		return RC;
	}

	DbgStat = CoreMod->CoreDebugStatus;

	Mask = DbgStat->DbgEvent1Halt.Mask | DbgStat->DbgEvent0Halt.Mask |
		DbgStat->DbgStrmStallHalt.Mask |
		DbgStat->DbgLockStallHalt.Mask | DbgStat->DbgMemStallHalt.Mask |
		DbgStat->DbgPCEventHalt.Mask | DbgStat->DbgHalt.Mask;

	*DebugStatus &= Mask;

	return XAIE_OK;
}

/*****************************************************************************/
/*
*
* This API reads the current value of the AIE PC value.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the AIE tile.
* @param	PCValue: Pointer to store current PC value.
* @return	XAIE_OK on success, Error code on failure.
*
* @note		None.
*
******************************************************************************/
AieRC XAie_CoreGetPCValue(XAie_DevInst *DevInst, XAie_LocType Loc,
		u32 *PCValue)
{
	u8 TileType;
	u64 RegAddr;
	const XAie_CoreMod *CoreMod;

	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	CoreMod = DevInst->DevProp.DevMod[TileType].CoreMod;
	RegAddr = CoreMod->CorePCOff +
		XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col);

	return  XAie_Read32(DevInst, RegAddr, PCValue);
}

/*****************************************************************************/
/*
*
* This API reads the current value of the AIE SP value.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the AIE tile.
* @param	SPValue: Pointer to store current SP value.
* @return	XAIE_OK on success, Error code on failure.
*
* @note		None.
*
******************************************************************************/
AieRC XAie_CoreGetSPValue(XAie_DevInst *DevInst, XAie_LocType Loc,
		u32 *SPValue)
{
	u8 TileType;
	u64 RegAddr;
	const XAie_CoreMod *CoreMod;

	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	CoreMod = DevInst->DevProp.DevMod[TileType].CoreMod;
	RegAddr = CoreMod->CoreSPOff +
		XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col);

	return  XAie_Read32(DevInst, RegAddr, SPValue);
}

/*****************************************************************************/
/*
*
* This API reads the current value of the AIE LR value.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the AIE tile.
* @param	LRValue: Pointer to store current LR value.
* @return	XAIE_OK on success, Error code on failure.
*
* @note		None.
*
******************************************************************************/
AieRC XAie_CoreGetLRValue(XAie_DevInst *DevInst, XAie_LocType Loc,
		u32 *LRValue)
{
	u8 TileType;
	u64 RegAddr;
	const XAie_CoreMod *CoreMod;

	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	CoreMod = DevInst->DevProp.DevMod[TileType].CoreMod;
	RegAddr = CoreMod->CoreLROff +
		XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col);

	return  XAie_Read32(DevInst, RegAddr, LRValue);
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
* @return	XAIE_OK on success, Error code on failure.
*
* @note		None.
*
******************************************************************************/
AieRC XAie_CoreReadDoneBit(XAie_DevInst *DevInst, XAie_LocType Loc,
		u8 *DoneBit)
{
	const XAie_CoreMod *CoreMod;
	u8 TileType;

	if((DevInst == XAIE_NULL) || (DoneBit == NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid arguments\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	CoreMod = DevInst->DevProp.DevMod[TileType].CoreMod;

	return CoreMod->ReadDoneBit(DevInst, Loc, DoneBit, CoreMod);
}

/*****************************************************************************/
/*
*
* This API reads the status from the core status register.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the AIE tile.
* @param	CoreStatus: Pointer to store the status from the core status
                register.
* @return	XAIE_OK on success, Error code on failure.
*
* @note		None.
*
******************************************************************************/
AieRC XAie_CoreGetStatus(XAie_DevInst *DevInst, XAie_LocType Loc,
		u32 *CoreStatus)
{
	u8 TileType;
	const XAie_CoreMod *CoreMod;

	if((DevInst == XAIE_NULL) || (CoreStatus == NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	CoreMod = DevInst->DevProp.DevMod[TileType].CoreMod;

	return CoreMod->GetCoreStatus(DevInst, Loc, CoreStatus, CoreMod);
}

/*****************************************************************************/
/*
*
* This API configures the debug control register of aie core
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the aie tile.
* @param	Event0: Event to halt the aie core
* @param	Event1: Event to halt the aie core
* @param	SingleStepEvent: Event to single step aie core
* @param	ResumeCoreEvent: Event to resume aie core execution after
*		debug halt
*
* @return	XAIE_OK on success, Error code on failure.
*
* @note		None.
*
******************************************************************************/
AieRC XAie_CoreConfigDebugControl1(XAie_DevInst *DevInst, XAie_LocType Loc,
		XAie_Events Event0, XAie_Events Event1,
		XAie_Events SingleStepEvent, XAie_Events ResumeCoreEvent)
{
	u8 TileType;
	u16 MEvent1, MEvent0, MSStepEvent, MResumeCoreEvent;
	u32 RegVal, Event0Val, Event1Val, SingleStepEventVal, ResumeCoreEventVal;
	u64 RegAddr;
	const XAie_CoreMod *CoreMod;
	const XAie_EvntMod *EvntMod;

	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	Event0Val = (u32)Event0;
	Event1Val = (u32)Event1;
	SingleStepEventVal = (u32)SingleStepEvent;
	ResumeCoreEventVal = (u32)ResumeCoreEvent;
	CoreMod = DevInst->DevProp.DevMod[TileType].CoreMod;
	EvntMod = &DevInst->DevProp.DevMod[TileType].EvntMod[XAIE_CORE_MOD];

	if((Event0Val < EvntMod->EventMin || Event0Val > EvntMod->EventMax) ||
				(Event1Val < EvntMod->EventMin ||
				 Event1Val > EvntMod->EventMax) ||
				(SingleStepEventVal < EvntMod->EventMin ||
				 SingleStepEventVal > EvntMod->EventMax) ||
				(ResumeCoreEventVal < EvntMod->EventMin ||
				 ResumeCoreEventVal > EvntMod->EventMax)) {
		XAIE_ERROR("Invalid event ID\n");
		return XAIE_INVALID_ARGS;
	}

	Event0Val -= EvntMod->EventMin;
	Event1Val -= EvntMod->EventMin;
	SingleStepEventVal -= EvntMod->EventMin;
	ResumeCoreEventVal -= EvntMod->EventMin;

	MEvent0 = EvntMod->XAie_EventNumber[Event0Val];
	MEvent1 = EvntMod->XAie_EventNumber[Event1Val];
	MSStepEvent = EvntMod->XAie_EventNumber[SingleStepEventVal];
	MResumeCoreEvent = EvntMod->XAie_EventNumber[ResumeCoreEventVal];

	if((MEvent0 == XAIE_EVENT_INVALID) ||
			(MEvent1 == XAIE_EVENT_INVALID) ||
			(MSStepEvent == XAIE_EVENT_INVALID) ||
			(MResumeCoreEvent == XAIE_EVENT_INVALID)) {
		XAIE_ERROR("Invalid event ID\n");
		return XAIE_INVALID_ARGS;
	}


	if ((_XAie_CheckPrecisionExceeds(CoreMod->CoreDebug->DebugHaltCoreEvent0.Lsb,
			_XAie_MaxBitsNeeded(MEvent0),MAX_VALID_AIE_REG_BIT_INDEX)) ||
			(_XAie_CheckPrecisionExceeds(CoreMod->CoreDebug->DebugHaltCoreEvent1.Lsb,
			_XAie_MaxBitsNeeded(MEvent1),MAX_VALID_AIE_REG_BIT_INDEX)) ||
			(_XAie_CheckPrecisionExceeds(CoreMod->CoreDebug->DebugSStepCoreEvent.Lsb,
			_XAie_MaxBitsNeeded(MSStepEvent),MAX_VALID_AIE_REG_BIT_INDEX)) ||
			(_XAie_CheckPrecisionExceeds(CoreMod->CoreDebug->DebugResumeCoreEvent.Lsb,
			_XAie_MaxBitsNeeded(MResumeCoreEvent),MAX_VALID_AIE_REG_BIT_INDEX))) {
		XAIE_ERROR("Check Precision Exceeds Failed\n");
		return XAIE_ERR;
	}

	RegVal = XAie_SetField(MEvent0,
			CoreMod->CoreDebug->DebugHaltCoreEvent0.Lsb,
			CoreMod->CoreDebug->DebugHaltCoreEvent0.Mask) |
		XAie_SetField(MEvent1,
				CoreMod->CoreDebug->DebugHaltCoreEvent1.Lsb,
				CoreMod->CoreDebug->DebugHaltCoreEvent1.Mask) |
		XAie_SetField(MSStepEvent,
				CoreMod->CoreDebug->DebugSStepCoreEvent.Lsb,
				CoreMod->CoreDebug->DebugSStepCoreEvent.Mask) |
		XAie_SetField(MResumeCoreEvent,
				CoreMod->CoreDebug->DebugResumeCoreEvent.Lsb,
				CoreMod->CoreDebug->DebugResumeCoreEvent.Mask);

	RegAddr = CoreMod->CoreDebug->DebugCtrl1Offset +
		XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col);

	return XAie_Write32(DevInst, RegAddr, RegVal);
}

/*****************************************************************************/
/*
*
* This API clears the configuration in debug control1 register to hardware
* reset values.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the aie tile.
*
* @return	XAIE_OK on success, Error code on failure.
*
* @note		None.
*
******************************************************************************/
AieRC XAie_CoreClearDebugControl1(XAie_DevInst *DevInst, XAie_LocType Loc)
{
	u8 TileType;
	u64 RegAddr;
	const XAie_CoreMod *CoreMod;

	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	CoreMod = DevInst->DevProp.DevMod[TileType].CoreMod;

	RegAddr = CoreMod->CoreDebug->DebugCtrl1Offset +
		XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col);

	return XAie_Write32(DevInst, RegAddr, 0U);
}

/*****************************************************************************/
/*
*
* This API configures the enable events register with enable event. This
* configuration will be used to enable the core when a particular event occurs.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the aie tile.
* @param	Event: Event to enable the core.
*
* @return	XAIE_OK on success, Error code on failure.
*
* @note		None.
*
******************************************************************************/
AieRC XAie_CoreConfigureEnableEvent(XAie_DevInst *DevInst, XAie_LocType Loc,
		XAie_Events Event)
{
	u8 TileType;
	u16 MappedEvent;
	u32 Mask, Value, EventVal;
	u64 RegAddr;
	const XAie_CoreMod *CoreMod;
	const XAie_EvntMod *EvntMod;

	EventVal = (u32)Event;
	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	CoreMod = DevInst->DevProp.DevMod[TileType].CoreMod;
	EvntMod = &DevInst->DevProp.DevMod[TileType].EvntMod[XAIE_CORE_MOD];

	if(EventVal < EvntMod->EventMin || EventVal > EvntMod->EventMax) {
		XAIE_ERROR("Invalid event ID\n");
		return XAIE_INVALID_ARGS;
	}

	EventVal -= EvntMod->EventMin;
	MappedEvent = EvntMod->XAie_EventNumber[EventVal];
	if(MappedEvent == XAIE_EVENT_INVALID) {
		XAIE_ERROR("Invalid event ID\n");
		return XAIE_INVALID_ARGS;
	}

	RegAddr = CoreMod->CoreEvent->EnableEventOff +
		XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col);

	Mask = CoreMod->CoreEvent->EnableEvent.Mask |
		CoreMod->CoreEvent->DisableEventOccurred.Mask |
		CoreMod->CoreEvent->EnableEventOccurred.Mask;

	if ((_XAie_CheckPrecisionExceeds(CoreMod->CoreEvent->EnableEvent.Lsb,
			_XAie_MaxBitsNeeded(MappedEvent),MAX_VALID_AIE_REG_BIT_INDEX))) {
		XAIE_ERROR("Check Precision Exceeds Failed\n");
		return XAIE_ERR;
	}
	Value = (u32)(MappedEvent << CoreMod->CoreEvent->EnableEvent.Lsb);

	return XAie_MaskWrite32(DevInst, RegAddr, Mask, Value);
}

/*****************************************************************************/
/*
*
* This API writes to the Error Halt Event register to halt the core
* bit.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the AIE tile.
* @param        Event: Event to halt the aie core
* @return	XAIE_OK on success, Error code on failure.
*
* @note         None.
*
******************************************************************************/
AieRC XAie_CoreConfigureErrorHaltEvent(XAie_DevInst *DevInst, XAie_LocType Loc,
		XAie_Events Event)
{
	u64 RegAddr;
	const XAie_EvntMod *EvntMod;
	u16 HwEvent;
	u8 TileType;
	AieRC RC;

	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

        TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
        if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
                XAIE_ERROR("Invalid Tile Type\n");
                return XAIE_INVALID_TILE;
        }

	EvntMod = &DevInst->DevProp.DevMod[TileType].EvntMod[XAIE_CORE_MOD];

	RegAddr = XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col) +
		EvntMod->ErrorHaltRegOff;

	RC = XAie_EventLogicalToPhysicalConv(DevInst, Loc, XAIE_CORE_MOD,
			Event, &HwEvent);
	if(RC != XAIE_OK) {
		XAIE_ERROR("Invalid event ID\n");
		return RC;
	}

	return XAie_Write32(DevInst, RegAddr, HwEvent);
}

/*****************************************************************************/
/*
*
* This API configures the enable events register with disable event. This
* configuration will be used to check if the core has triggered the disable
* event.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the aie tile.
*
* @return	XAIE_OK on success, Error code on failure.
*
* @note		None.
*
******************************************************************************/
AieRC XAie_CoreConfigureDone(XAie_DevInst *DevInst, XAie_LocType Loc)
{
	u8 TileType;
	const XAie_CoreMod *CoreMod;

	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	CoreMod = DevInst->DevProp.DevMod[TileType].CoreMod;

	return CoreMod->ConfigureDone(DevInst, Loc, CoreMod);
}

/*****************************************************************************/
/*
*
* This API configures the core accumulator control register to specify the
* direction of cascade stream.
*
* @param       DevInst: Device Instance
* @param       Loc: Location of the aie tile.
* @param       InDir: Input direction. Valid values: NORTH, WEST
* @param       OutDir: Output direction. Valid values: SOUTH, EAST
*
* @return      XAIE_OK on success, Error code on failure.
*
* @note                None.
*
******************************************************************************/
AieRC XAie_CoreConfigAccumulatorControl(XAie_DevInst *DevInst,
               XAie_LocType Loc, StrmSwPortType InDir, StrmSwPortType OutDir)
{
	u8 TileType;
	const XAie_CoreMod *CoreMod;
	const XAie_RegCoreAccumCtrl *AccumCtrl;
	u32 RegVal;
	u64 RegAddr;

	if((DevInst == XAIE_NULL) ||
		(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	CoreMod = DevInst->DevProp.DevMod[TileType].CoreMod;
	AccumCtrl = CoreMod->CoreAccumCtrl;

	if (AccumCtrl == XAIE_NULL) {
		XAIE_ERROR("Configure accum control is not supported.\n");
		return XAIE_FEATURE_NOT_SUPPORTED;
	}

	if ((InDir != NORTH && InDir != WEST) ||
		(OutDir != SOUTH && OutDir != EAST)) {
		XAIE_ERROR("Configure accum control failed, invalid direction.\n");
		return XAIE_INVALID_ARGS;
	}

	RegAddr = AccumCtrl->RegOff +
		XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col);

	/*
	 * Here is the directions in the enum sequence:
	 *  * SOUTH, WEST, NORTH, EAST
	 *  * For input , 0 == NORTH, 1 == WEST
	 *  * For output, 0 == SOUTH, 1 == EAST
	 */

	if ((_XAie_CheckPrecisionExceeds(AccumCtrl->CascadeInput.Lsb,
			_XAie_MaxBitsNeeded(((u8)InDir - (u8)SOUTH) % 2U),
			MAX_VALID_AIE_REG_BIT_INDEX))  ||
			(_XAie_CheckPrecisionExceeds(AccumCtrl->CascadeOutput.Lsb,
			_XAie_MaxBitsNeeded(((u8)OutDir - (u8)SOUTH) % 2U),
			MAX_VALID_AIE_REG_BIT_INDEX))) {
		XAIE_ERROR("Check Precision Exceeds Failed\n");
		return XAIE_ERR;
	}
	RegVal = XAie_SetField(((u8)InDir - (u8)SOUTH) % 2U,
			AccumCtrl->CascadeInput.Lsb,
			AccumCtrl->CascadeInput.Mask) |
		XAie_SetField(((u8)OutDir - (u8)SOUTH) % 2U,
			AccumCtrl->CascadeOutput.Lsb,
			AccumCtrl->CascadeOutput.Mask);

	return XAie_Write32(DevInst, RegAddr, RegVal);
}

/*****************************************************************************/
/*
*
* This API clears event occurred status.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the aie tile.
*
* @return	XAIE_OK on success, Error code on failure.
*
* @note		None.
*
******************************************************************************/
AieRC XAie_ClearCoreDisableEventOccurred(XAie_DevInst *DevInst,
		XAie_LocType Loc)
{
	u8 TileType;
	u32 Mask, Value;
	u64 RegAddr;
	const XAie_CoreMod *CoreMod;

	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	CoreMod = DevInst->DevProp.DevMod[TileType].CoreMod;

	RegAddr = CoreMod->CoreEvent->EnableEventOff +
		XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col);

	if ((_XAie_CheckPrecisionExceeds(CoreMod->CoreEvent->DisableEventOccurred.Lsb,
			_XAie_MaxBitsNeeded(1U),MAX_VALID_AIE_REG_BIT_INDEX))) {
		XAIE_ERROR("Check Precision Exceeds Failed\n");
		return XAIE_ERR;
	}

	Mask = CoreMod->CoreEvent->DisableEventOccurred.Mask;
	Value = (u32)(1U << CoreMod->CoreEvent->DisableEventOccurred.Lsb);

	return XAie_MaskWrite32(DevInst, RegAddr, Mask, Value);
}

/*****************************************************************************/
/*
*
* This API configures the core processor bus control register to enable or
* disable core's access processor bus.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the aie tile.
* @param	Enable: XAIE_ENABLE to enable, and XAIE_DISABLE to disable.
*
* @return	XAIE_OK on success, Error code on failure.
*
* @note		Internal only.
*
******************************************************************************/
static AieRC _XAie_CoreProcessorBusConfig(XAie_DevInst *DevInst,
		XAie_LocType Loc, u8 Enable)
{
	u8 TileType;
	u32 RegVal, RegMask;
	u64 RegAddr;
	const XAie_CoreMod *CoreMod;
	const XAie_RegCoreProcBusCtrl *ProcBusCtrl;

	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType != XAIEGBL_TILE_TYPE_AIETILE) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	CoreMod = DevInst->DevProp.DevMod[TileType].CoreMod;

	ProcBusCtrl = CoreMod->ProcBusCtrl;
	if (ProcBusCtrl == XAIE_NULL) {
		XAIE_ERROR("Core processor bus control is not supported.\n");
		return XAIE_FEATURE_NOT_SUPPORTED;
	}

	if ((_XAie_CheckPrecisionExceeds(ProcBusCtrl->CtrlEn.Lsb,
			_XAie_MaxBitsNeeded(Enable),MAX_VALID_AIE_REG_BIT_INDEX))) {
		XAIE_ERROR("Check Precision Exceeds Failed\n");
		return XAIE_ERR;
	}

	RegMask = ProcBusCtrl->CtrlEn.Mask;
	RegVal = XAie_SetField(Enable, ProcBusCtrl->CtrlEn.Lsb, RegMask);
	RegAddr = ProcBusCtrl->RegOff +
			XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col);

	return XAie_MaskWrite32(DevInst, RegAddr, RegMask, RegVal);
}

/*****************************************************************************/
/*
*
* This API enables core's access to the processor bus.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the aie tile.
*
* @return	XAIE_OK on success, Error code on failure.
*
* @note
*
******************************************************************************/
AieRC XAie_CoreProcessorBusEnable(XAie_DevInst *DevInst, XAie_LocType Loc)
{
	return _XAie_CoreProcessorBusConfig(DevInst, Loc, XAIE_ENABLE);
}

/*****************************************************************************/
/*
*
* This API disables core's access to the processor bus.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of the aie tile.
*
* @return	XAIE_OK on success, Error code on failure.
*
* @note
*
******************************************************************************/
AieRC XAie_CoreProcessorBusDisable(XAie_DevInst *DevInst, XAie_LocType Loc)
{
	return _XAie_CoreProcessorBusConfig(DevInst, Loc, XAIE_DISABLE);
}

#endif /* XAIE_FEATURE_CORE_ENABLE */

/** @} */
