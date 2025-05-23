/******************************************************************************
* Copyright (C) 2019 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*****************************************************************************/
/**
* @file xaie_timer.c
* @{
*
* This file contains routines for AIE timers
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date        Changes
* ----- ------  --------    ---------------------------------------------------
* 1.0   Dishita 04/06/2020  Initial creation
* 1.1   Dishita 06/10/2020  Add XAie_WaitCycles API
* 1.2   Dishita 06/17/2020  Resolve compiler warning
* 1.3   Tejus   06/10/2020  Switch to new io backend apis.
* 1.4   Nishad  09/14/2020  Add check for invalid XAie_Reset value in
*			    XAie_SetTimerResetEvent() API.
* </pre>
*
******************************************************************************/
/***************************** Include Files *********************************/
#include <stdlib.h>

#include "xaie_events.h"
#include "xaie_feature_config.h"
#include "xaie_helper.h"
#include "xaie_helper_internal.h"
#include "xaie_timer.h"
#include "xaiegbl.h"

#ifdef XAIE_FEATURE_TIMER_ENABLE

/*****************************************************************************/
/***************************** Macro Definitions *****************************/
#define XAIE_TIMER_32BIT_SHIFT		32U
#define XAIE_WAIT_CYCLE_MAX_VAL		0xFFFFFFFFFFFF

/************************** Function Definitions *****************************/
/*****************************************************************************/
/**
*
* This API sets the timer trigger events value. Timer low event will be
* generated if the timer low reaches the specified low event value. Timer high
* event will be generated if the timer high reaches the specified high event
* value.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of AIE tile
* @param	Module: Module of tile.
*			For AIE Tile - XAIE_MEM_MOD or XAIE_CORE_MOD,
*			For Pl or Shim tile - XAIE_PL_MOD,
*			For Mem tile - XAIE_MEM_MOD.
*
* @param	LowEventValue: Value to set for the timer to trigger timer low
*                              event.
* @param	HighEventValue: Value to set for the timer to trigger timer
*                               high event.
*
* @return	XAIE_OK on success.
* 		XAIE_INVALID_ARGS if any argument is invalid
*		XAIE_INVALID_TILE if tile type from Loc is invalid
*
* @note
*
*******************************************************************************/
AieRC XAie_SetTimerTrigEventVal(XAie_DevInst *DevInst, XAie_LocType Loc,
		XAie_ModuleType Module, u32 LowEventValue, u32 HighEventValue)
{
	u64 RegAddr;
	u8 TileType, RC;
	const XAie_TimerMod *TimerMod;

	if((DevInst == XAIE_NULL) ||
		(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType == XAIEGBL_TILE_TYPE_MAX) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	/* check for module and tiletype combination */
	RC = XAie_CheckModule(DevInst, Loc, Module);
	if(RC != XAIE_OK) {
		return XAIE_INVALID_ARGS;
	}

	if(Module == XAIE_PL_MOD) {
		TimerMod = &DevInst->DevProp.DevMod[TileType].TimerMod[0U];
	}

	else {
		TimerMod = &DevInst->DevProp.DevMod[TileType].TimerMod[Module];
	}

	/* Set up Timer low event value */
	RegAddr = XAie_GetTileAddr(DevInst, Loc.Row ,Loc.Col) +
		TimerMod->TrigEventLowValOff;
	RC = XAie_Write32(DevInst, RegAddr, LowEventValue);
	if(RC != XAIE_OK) {
		return RC;
	}

	/* Set up Timer high event value */
	RegAddr = XAie_GetTileAddr(DevInst, Loc.Row ,Loc.Col) +
		TimerMod->TrigEventHighValOff;
	return XAie_Write32(DevInst, RegAddr, HighEventValue);
}

/*****************************************************************************/
/**
*
* This API resets the timer
*
* @param	DevInst - Device Instance.
* @param	Loc - Location of tile.
* @param	Module - Module of the tile
*			 For AIE Tile - XAIE_MEM_MOD or XAIE_CORE_MOD,
*			 For Pl or Shim tile - XAIE_PL_MOD,
*			 For Mem tile - XAIE_MEM_MOD.
*
* @return	XAIE_OK on success.
*		XAIE_INVALID_ARGS if any argument is invalid
*		XAIE_INVALID_TILE if tile type from Loc is invalid

* @note
*
*******************************************************************************/
AieRC XAie_ResetTimer(XAie_DevInst *DevInst, XAie_LocType Loc,
		XAie_ModuleType Module)
{
	u32 RegVal, Mask;
	u64 RegAddr;
	u8 TileType, RC;
	const XAie_TimerMod *TimerMod;

	if((DevInst == XAIE_NULL) ||
		(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType == XAIEGBL_TILE_TYPE_MAX) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	/* check for module and tiletype combination */
	RC = XAie_CheckModule(DevInst, Loc, Module);
	if(RC != XAIE_OK) {
		return XAIE_INVALID_ARGS;
	}

	if(Module == XAIE_PL_MOD) {
		TimerMod = &DevInst->DevProp.DevMod[TileType].TimerMod[0U];
	}

	else {
		TimerMod = &DevInst->DevProp.DevMod[TileType].TimerMod[Module];
	}

	RegAddr = XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col) +
		TimerMod->CtrlOff;
	Mask = TimerMod->CtrlReset.Mask;

	if ((_XAie_CheckPrecisionExceeds(TimerMod->CtrlReset.Lsb,
			_XAie_MaxBitsNeeded(XAIE_RESETENABLE),MAX_VALID_AIE_REG_BIT_INDEX))) {
		XAIE_ERROR("Check Precision Exceeds Failed\n");
		return XAIE_ERR;
	}
	RegVal = XAie_SetField(XAIE_RESETENABLE, TimerMod->CtrlReset.Lsb, Mask);

	return XAie_MaskWrite32(DevInst, RegAddr, Mask, RegVal);
}

/*****************************************************************************/
/**
*
* This API sets the timer reset event. The timer will reset when the event
* is raised.
*
* @param	DevInst - Device Instance.
* @param	Loc - Location of tile.
* @param	Module - Module of the tile
*                         For AIE Tile - XAIE_MEM_MOD or XAIE_CORE_MOD,
*                         For Pl or Shim tile - XAIE_PL_MOD,
*                         For Mem tile - XAIE_MEM_MOD.
* @param	Event - Reset event.
* @param	Reset - Indicate if reset is also required in this call.
*                       (XAIE_RESETENABLE, XAIE_RESETDISABLE)
*
* @return	XAIE_OK on success.
*		XAIE_INVALID_ARGS if any argument is invalid
* 		XAIE_INVALID_TILE if tile type from Loc is invalid
*
* @note
*
*******************************************************************************/
AieRC XAie_SetTimerResetEvent(XAie_DevInst *DevInst, XAie_LocType Loc,
		XAie_ModuleType Module, XAie_Events Event,
		XAie_Reset Reset)
{
	u32 RegVal;
	u64 RegAddr;
	u8 TileType, RC;
	u16 IntEvent;
	const XAie_TimerMod *TimerMod;
	const XAie_EvntMod *EvntMod;

	if((DevInst == XAIE_NULL) ||
		(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	if (Reset > XAIE_RESETENABLE) {
		XAIE_ERROR("Invalid reset value\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType == XAIEGBL_TILE_TYPE_MAX) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	/* check for module and tiletype combination */
	RC = XAie_CheckModule(DevInst, Loc, Module);
	if(RC != XAIE_OK) {
		return XAIE_INVALID_ARGS;
	}

	if(Module == XAIE_PL_MOD) {
		TimerMod = &DevInst->DevProp.DevMod[TileType].TimerMod[0U];
		EvntMod = &DevInst->DevProp.DevMod[TileType].EvntMod[0U];
	}

	else {
		TimerMod = &DevInst->DevProp.DevMod[TileType].TimerMod[Module];
		EvntMod = &DevInst->DevProp.DevMod[TileType].EvntMod[Module];
	}

	/* check if the event passed as input is corresponding to the module */
	if(Event < EvntMod->EventMin || Event > EvntMod->EventMax) {
		XAIE_ERROR("Invalid Event id\n");
		return XAIE_INVALID_ARGS;
	}

	/* Subtract the module offset from event number */
	Event -= EvntMod->EventMin;

	/* Getting the true event number from the enum to array mapping */
	IntEvent = EvntMod->XAie_EventNumber[Event];

	/*checking for valid true event number */
	if(IntEvent == XAIE_EVENT_INVALID) {
		XAIE_ERROR("Invalid Event id\n");
		return XAIE_INVALID_ARGS;
	}

	if ((_XAie_CheckPrecisionExceeds( TimerMod->CtrlResetEvent.Lsb,
			_XAie_MaxBitsNeeded(IntEvent),MAX_VALID_AIE_REG_BIT_INDEX))) {
		XAIE_ERROR("Check Precision Exceeds Failed\n");
		return XAIE_ERR;
	}
	RegVal = XAie_SetField(IntEvent, TimerMod->CtrlResetEvent.Lsb,
			TimerMod->CtrlResetEvent.Mask);

	if ((_XAie_CheckPrecisionExceeds(TimerMod->CtrlReset.Lsb,
			_XAie_MaxBitsNeeded((u32)Reset),MAX_VALID_AIE_REG_BIT_INDEX))) {
		XAIE_ERROR("Check Precision Exceeds Failed\n");
		return XAIE_ERR;
	}
	RegVal |= XAie_SetField(Reset, TimerMod->CtrlReset.Lsb,
			TimerMod->CtrlReset.Mask);

	RegAddr = XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col) +
		TimerMod->CtrlOff;

	return XAie_Write32(DevInst, RegAddr, RegVal);
}

/*****************************************************************************/
/**
*
* This API returns the current value of the module's 64-bit timer.
*
* @param	DevInst - Device Instance.
* @param	Loc - Location of tile.
* @param	Module - Module of the tile
*                         For AIE Tile - XAIE_MEM_MOD or XAIE_CORE_MOD,
*                         For Pl or Shim tile - XAIE_PL_MOD,
*                         For Mem tile - XAIE_MEM_MOD.
* @param	TimerVal - Pointer to store Timer Value.
*
* @return	XAIE_OK on success
*		XAIE_INVALID_ARGS if any argument is invalid
*		XAIE_INVALID_TILE if tile type from Loc is invalid
*
* @note		None.
*
********************************************************************************/
AieRC XAie_ReadTimer(XAie_DevInst *DevInst, XAie_LocType Loc,
		XAie_ModuleType Module, u64 *TimerVal)
{
	AieRC RC;
	u32 CurValHigh, CurValLow;
	u8 TileType;
	const XAie_TimerMod *TimerMod;

	if((DevInst == XAIE_NULL) || (TimerVal == XAIE_NULL) ||
		(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance or TimerVal\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType == XAIEGBL_TILE_TYPE_MAX) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	/* check for module and tiletype combination */
	RC = XAie_CheckModule(DevInst, Loc, Module);
        if(RC != XAIE_OK) {
                return XAIE_INVALID_ARGS;
	}

	if(Module == XAIE_PL_MOD) {
		TimerMod = &DevInst->DevProp.DevMod[TileType].TimerMod[0U];
	}

	else {
		TimerMod = &DevInst->DevProp.DevMod[TileType].TimerMod[Module];
	}

	/* Read the timer high and low values before wait */
	RC = XAie_Read32(DevInst, XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col) +
			TimerMod->LowOff, &CurValLow);
	if(RC != XAIE_OK) {
		return RC;
	}

	RC = XAie_Read32(DevInst, XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col) +
			TimerMod->HighOff, &CurValHigh);
	if(RC != XAIE_OK) {
		return RC;
	}

	*TimerVal = ((u64)CurValHigh << XAIE_TIMER_32BIT_SHIFT) | CurValLow;

	return XAIE_OK;
}

/*****************************************************************************/
/**
* This API implements a blocking wait function until the specified clock cyles
* are elapsed in the given module's 64-bit counter.
*
* @param        DevInst - Device Instance.
* @param        Loc - Location of tile.
* @param        Module - Module of the tile
*                        For AIE Tile - XAIE_MEM_MOD or XAIE_CORE_MOD,
*                        For Pl or Shim tile - XAIE_PL_MOD,
*                        For Mem tile - XAIE_MEM_MOD.
* @param        CycleCnt - No. of timer clock cycles to elapse.
*
* @return       XAIE_OK on success
*               XAIE_INVALID_ARGS if any argument is invalid
*               XAIE_INVALID_TILE if tile type from Loc is invalid
*
* @note         CycleCnt has an upper limit of 0xFFFFFFFFFFFF or 300 trillion
*		cycles to prevent overflow.
*
******************************************************************************/
AieRC XAie_WaitCycles(XAie_DevInst *DevInst, XAie_LocType Loc,
		XAie_ModuleType Module, u64 CycleCnt)
{

	u64 StartVal, EndVal, CurVal = 0U;
	u32 CurHigh, CurLow;
	u8 TileType;
	AieRC RC;
	const XAie_TimerMod *TimerMod;

	if((DevInst == XAIE_NULL) ||
		(DevInst->IsReady != XAIE_COMPONENT_IS_READY)) {
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	if(TileType == XAIEGBL_TILE_TYPE_MAX) {
		XAIE_ERROR("Invalid Tile Type\n");
		return XAIE_INVALID_TILE;
	}

	/* check for module and tiletype combination */
	RC = XAie_CheckModule(DevInst, Loc, Module);
	if(RC != XAIE_OK) {
		return XAIE_INVALID_ARGS;
	}

	if(CycleCnt > XAIE_WAIT_CYCLE_MAX_VAL) {
		XAIE_ERROR("CycleCnt above max value\n");
		return XAIE_INVALID_ARGS;
	}

	if(Module == XAIE_PL_MOD) {
		TimerMod = &DevInst->DevProp.DevMod[TileType].TimerMod[0U];
	} else {
		TimerMod = &DevInst->DevProp.DevMod[TileType].TimerMod[Module];
	}

	/* Read the timer high and low values before wait */
	RC = XAie_Read32(DevInst, XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col) +
			TimerMod->LowOff, &CurLow);
	if(RC != XAIE_OK) {
		return RC;
	}

	RC = XAie_Read32(DevInst, XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col) +
			TimerMod->HighOff, &CurHigh);
	if(RC != XAIE_OK) {
		return RC;
	}

	StartVal = ((u64)CurHigh << XAIE_TIMER_32BIT_SHIFT) | CurLow;
	EndVal = StartVal + CycleCnt;

	while(CurVal < EndVal) {
		/* Read the timer high and low values */
		RC = XAie_Read32(DevInst,
				XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col) +
				TimerMod->LowOff, &CurLow);
		if(RC != XAIE_OK) {
			return RC;
		}

		RC = XAie_Read32(DevInst,
				XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col) +
				TimerMod->HighOff, &CurHigh);
		if(RC != XAIE_OK) {
			return RC;
		}

		CurVal = ((u64)CurHigh << XAIE_TIMER_32BIT_SHIFT) | CurLow;
	}

	return XAIE_OK;
}

/*****************************************************************************/
/**
* This API returns the broadcast event enum given a resource id from the
* event map.
*
* @param        DevInst: Device Instance
* @param        Loc: Location of Tile
* @param        Mod: Module type
* @param        RscId: Specific resource to be requested
*
* @return       Event enum on success.
*
* @note         Internal only.
*
*******************************************************************************/
static XAie_Events _XAie_GetBroadcastEventfromRscId(XAie_DevInst *DevInst,
		XAie_LocType Loc, XAie_ModuleType Mod, u8 RscId)
{
	u8 TileType;
	const XAie_EvntMod *EvntMod;

	TileType =  DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);

	if(Mod == XAIE_PL_MOD)
		EvntMod = &DevInst->DevProp.DevMod[TileType].EvntMod[0U];
	else
		EvntMod = &DevInst->DevProp.DevMod[TileType].EvntMod[Mod];

	return EvntMod->BroadcastEventMap->Event + RscId;
}

/*****************************************************************************/
/**
 * This API will setup broadcast configuration for all tiles for the timer sync
 * sequence
 *
 * @param        DevInst: Device Instance
 * @param        NumTiles: Size of Locs array
 * @param        Locs: Array of locations to clear
 * @param        BcastId: Channel ID of broadcast channel for timer reset
 *
 * @return       XAIE_OK on success, error code for failure
 *
 * @note         Internal only.
 *
 *******************************************************************************/
static AieRC _XAie_SetupBroadcastConfig(XAie_DevInst *DevInst, u32 NumTiles,
		XAie_LocType *Locs, u8 BcastId)
{
	AieRC RC = XAIE_OK;
	u8 TileType;

	for(u32 i = 0; i < NumTiles; i++) {
		TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Locs[i]);

		/* Blocking unncessary broadcasting */
		if(TileType == XAIEGBL_TILE_TYPE_AIETILE) {
			if(DevInst->DevProp.DevGen == XAIE_DEV_GEN_AIE) {
				/* Checkerboard structure for AIE1 */
				if((Locs[i].Row % 2) == 0) {
					RC |= XAie_EventBroadcastBlockDir(
							DevInst, Locs[i],
							XAIE_MEM_MOD,
							XAIE_EVENT_SWITCH_A,
							BcastId,
							XAIE_EVENT_BROADCAST_WEST);
					RC |= XAie_EventBroadcastBlockDir(
							DevInst, Locs[i],
							XAIE_CORE_MOD,
							XAIE_EVENT_SWITCH_A,
							BcastId,
							XAIE_EVENT_BROADCAST_EAST);
				} else {
					RC |= XAie_EventBroadcastBlockDir(
							DevInst, Locs[i],
							XAIE_MEM_MOD,
							XAIE_EVENT_SWITCH_A,
							BcastId,
							XAIE_EVENT_BROADCAST_EAST);
					RC |= XAie_EventBroadcastBlockDir(
							DevInst, Locs[i],
							XAIE_CORE_MOD,
							XAIE_EVENT_SWITCH_A,
							BcastId,
							XAIE_EVENT_BROADCAST_WEST);
				}
			} else {
				RC |= XAie_EventBroadcastBlockDir(
						DevInst, Locs[i],
						XAIE_MEM_MOD,
						XAIE_EVENT_SWITCH_A,
						BcastId,
						XAIE_EVENT_BROADCAST_EAST);
				RC |= XAie_EventBroadcastBlockDir(
						DevInst, Locs[i],
						XAIE_CORE_MOD,
						XAIE_EVENT_SWITCH_A,
						BcastId,
						XAIE_EVENT_BROADCAST_WEST);
			}
		} else if(Locs[i].Row != 0) {
			RC |= XAie_EventBroadcastBlockDir(
					DevInst, Locs[i],
					XAIE_MEM_MOD,
					XAIE_EVENT_SWITCH_A,
					BcastId,
					XAIE_EVENT_BROADCAST_WEST |
					XAIE_EVENT_BROADCAST_EAST);
		}

		if(RC != XAIE_OK) {
			return RC;
		}
	}

	return XAIE_OK;
}

/*****************************************************************************/
/**
 * This API will setup timer reset events for all tiles for the timer sync
 * sequence
 *
 * @param        DevInst: Device Instance
 * @param        NumTiles: Size of Locs array
 * @param        Locs: Array of locations to clear
 * @param        BcastId: Channel ID of broadcast channel for timer reset
 *
 * @return       XAIE_OK on success, error code for failure
 *
 * @note         Internal only.
 *
 *******************************************************************************/
static AieRC _XAie_SetupTimerConfig(XAie_DevInst *DevInst, u32 NumTiles,
		XAie_LocType *Locs, u8 BcastId)
{
	XAie_Events BcastEvent;
	AieRC RC = XAIE_OK;
	u8 TileType;

	for(u32 i = 0; i < NumTiles; i++) {
		TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Locs[i]);

		if(TileType == XAIEGBL_TILE_TYPE_AIETILE) {
			BcastEvent = _XAie_GetBroadcastEventfromRscId(
					DevInst, Locs[i], XAIE_MEM_MOD,
					BcastId);
			RC = XAie_SetTimerResetEvent(DevInst, Locs[i],
					XAIE_MEM_MOD, BcastEvent,
					XAIE_RESETDISABLE);
			if(RC != XAIE_OK) {
				return RC;
			}

			BcastEvent = _XAie_GetBroadcastEventfromRscId(
					DevInst, Locs[i], XAIE_CORE_MOD,
					BcastId);
			RC = XAie_SetTimerResetEvent(DevInst, Locs[i],
					XAIE_CORE_MOD, BcastEvent,
					XAIE_RESETDISABLE);
		} else if(TileType == XAIEGBL_TILE_TYPE_MEMTILE) {
			BcastEvent = _XAie_GetBroadcastEventfromRscId(
					DevInst, Locs[i], XAIE_MEM_MOD,
					BcastId);
			RC = XAie_SetTimerResetEvent(DevInst, Locs[i],
					XAIE_MEM_MOD, BcastEvent,
					XAIE_RESETDISABLE);
		} else {
			BcastEvent = _XAie_GetBroadcastEventfromRscId(
					DevInst, Locs[i], XAIE_PL_MOD,
					BcastId);
			RC = XAie_SetTimerResetEvent(DevInst, Locs[i],
					XAIE_PL_MOD, BcastEvent,
					XAIE_RESETDISABLE);
		}
		if(RC != XAIE_OK) {
			return RC;
		}
	}

	return XAIE_OK;
}

/*****************************************************************************/
/**
* This API clears broadcast configuration for shim tiles till the mentioned
* column.
*
*
* @param        DevInst: Device Instance
* @param        StartCol: Start column from where broadcast conf needs to be
* 			  cleared
* @param        EndCol: End column till where broadcast conf needs to be cleared
* @param        BcastChannelId: ID of broadcast channel
*
* @return       None
*
* @note         Internal only.
*
*******************************************************************************/
static void _XAie_ClearShimBroadcast(XAie_DevInst *DevInst, u8 StartCol,
		u8 EndCol, u32 BcastChannelId)
{
	AieRC RC;
	if(BcastChannelId > UCHAR_MAX) {
		XAIE_ERROR("Invalid BcastChannelId Value \n");
		return;
	}
	for(u32 i = StartCol; i < EndCol; i++) {
		XAie_LocType Loc = XAie_TileLoc((u8)i, 0);
		RC = XAie_EventBroadcast(DevInst, Loc, XAIE_PL_MOD,
			(u8)BcastChannelId, XAIE_EVENT_NONE_PL);
		if(RC != XAIE_OK) {
			return;
		}
	}
}

/*****************************************************************************/
/**
 * This API clears timer configuration for all the locations
 *
 * @param        DevInst: Device Instance
 * @param        NumTiles: Size of Locs array
 * @param        Locs: Array of locations to clear
 *
 * @note         Internal only.
 *******************************************************************************/
static void _XAie_ClearTimerConfig(XAie_DevInst *DevInst, u32 NumTiles,
		XAie_LocType *Locs)
{
	const XAie_EvntMod *EvntMod;
	u8 TType;

	for(u32 i = 0; i < NumTiles; i++) {
		TType = DevInst->DevOps->GetTTypefromLoc(DevInst, Locs[i]);

		if(TType == XAIEGBL_TILE_TYPE_AIETILE) {
			EvntMod = &DevInst->DevProp.DevMod[TType].EvntMod[0U];
			if((EvntMod->EventMin > XAIE_EVENT_LAST))
				return;

			XAie_SetTimerResetEvent(DevInst, Locs[i], XAIE_MEM_MOD,
					(XAie_Events)EvntMod->EventMin, XAIE_RESETDISABLE);

			EvntMod = &DevInst->DevProp.DevMod[TType].EvntMod[1U];
			if((EvntMod->EventMin > XAIE_EVENT_LAST))
				return;

			XAie_SetTimerResetEvent(DevInst, Locs[i], XAIE_CORE_MOD,
					(XAie_Events)EvntMod->EventMin, XAIE_RESETDISABLE);
		} else if (TType == XAIEGBL_TILE_TYPE_MEMTILE) {
			EvntMod = &DevInst->DevProp.DevMod[TType].EvntMod[0U];

            if((EvntMod->EventMin > XAIE_EVENT_LAST))
				return;
			XAie_SetTimerResetEvent(DevInst, Locs[i], XAIE_MEM_MOD,
					(XAie_Events)EvntMod->EventMin, XAIE_RESETDISABLE);
		} else {
			EvntMod = &DevInst->DevProp.DevMod[TType].EvntMod[0U];
            if((EvntMod->EventMin > XAIE_EVENT_LAST))
            	return;

			XAie_SetTimerResetEvent(DevInst, Locs[i], XAIE_PL_MOD,
					(XAie_Events)EvntMod->EventMin, XAIE_RESETDISABLE);
		}
	}
}

/*****************************************************************************/
/**
 * This API clears broadcast configuration for all the locations
 *
 * @param        DevInst: Device Instance
 * @param        NumTiles: Size of Locs array
 * @param        Locs: Array of locations to clear
 * @param        BCastId: Id of broadcast channel to clear configuration.
 *
 * @note         Internal only.
 *******************************************************************************/
static void _XAie_ClearBroadcastConfig(XAie_DevInst *DevInst, u32 NumTiles,
		XAie_LocType *Locs, u8 BcastId)
{
	u8 TileType;

	for(u32 i = 0; i < NumTiles; i++) {
		TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Locs[i]);

		if(TileType == XAIEGBL_TILE_TYPE_AIETILE) {
			XAie_EventBroadcastUnblockDir(DevInst, Locs[i],
					XAIE_MEM_MOD,
					XAIE_EVENT_SWITCH_A,
					BcastId,
					XAIE_EVENT_BROADCAST_WEST |
					XAIE_EVENT_BROADCAST_EAST);
			XAie_EventBroadcastUnblockDir(DevInst, Locs[i],
					XAIE_CORE_MOD,
					XAIE_EVENT_SWITCH_A,
					BcastId,
					XAIE_EVENT_BROADCAST_WEST |
					XAIE_EVENT_BROADCAST_EAST);
		} else if(Locs[i].Row != 0) {
			XAie_EventBroadcastUnblockDir(DevInst, Locs[i],
					XAIE_MEM_MOD,
					XAIE_EVENT_SWITCH_A,
					BcastId,
					XAIE_EVENT_BROADCAST_WEST |
					XAIE_EVENT_BROADCAST_EAST);
		}
	}
}

/*****************************************************************************/
/**
 * This API synchronizes timer for all tiles for all modules in the partition.
 *
 * @param        DevInst - Device Instance.
 * @param        BcastChannelId - Broadcast channel id for timer sync.
 *
 * @return       XAIE_OK on success
 *               XAIE_INVALID_ARGS if any argument is invalid
 *               XAIE_INVALID_TILE if tile type from Loc is invalid
 ******************************************************************************/
AieRC XAie_SyncTimer(XAie_DevInst *DevInst, u8 BcastChannelId)
{
	AieRC RC;
	u32 NumTiles;
	XAie_LocType *Locs;
	XAie_Events ShimBcastEvent;

	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)){
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	/* Get all ungated tiles to broadcast to */
	NumTiles = (u32)((u32)DevInst->NumCols * (u32)DevInst->NumRows);
	Locs = (XAie_LocType *)malloc(NumTiles * sizeof(XAie_LocType));
	if(Locs == NULL) {
		XAIE_ERROR("Unable to allocate memory for tile locations\n");
		return XAIE_ERR;
	}

	RC = XAie_GetUngatedLocsInPartition(DevInst, &NumTiles, Locs);
	if(RC != XAIE_OK) {
		free(Locs);
		return RC;
	}

	RC = _XAie_SetupBroadcastConfig(DevInst, NumTiles, Locs, BcastChannelId);
	if(RC != XAIE_OK) {
		XAIE_ERROR("Failed to setup broadcast network for timer sync\n");
		free(Locs);
		return RC;
	}

	if(BcastChannelId >= UCHAR_MAX) {
		XAIE_ERROR("Invalid BcastChannelId Value \n");
		free(Locs);
		return XAIE_ERR;
	}
	ShimBcastEvent = _XAie_GetBroadcastEventfromRscId(DevInst,
			XAie_TileLoc(0, 0), XAIE_PL_MOD, BcastChannelId + 1);

	for(u32 i = 0; i < DevInst->NumCols; i++) {
		XAie_LocType Loc = XAie_TileLoc((u8)i, 0);

		RC = XAie_EventBroadcast(DevInst, Loc, XAIE_PL_MOD,
				BcastChannelId, ShimBcastEvent);
		if (i == 0) {
			RC = XAie_EventBroadcast(DevInst, Loc, XAIE_PL_MOD,
					BcastChannelId + 1 , ShimBcastEvent);
		}
		if(RC != XAIE_OK) {
			XAIE_ERROR("Unable to configure shim broadcast event for timer sync\n");
			free(Locs);
			return RC;
		}
	}

	/* Configure the timer control with the trigger event */
	RC = _XAie_SetupTimerConfig(DevInst, NumTiles, Locs, BcastChannelId);
	if(RC != XAIE_OK) {
		XAIE_ERROR("Failed to setup timer reset events\n");
		free(Locs);
		return RC;
	}

	/* Trigger Event */
	RC = XAie_EventGenerate(DevInst, XAie_TileLoc(0, 0), XAIE_PL_MOD,
			ShimBcastEvent);
	if(RC != XAIE_OK) {
		XAIE_ERROR("Unable to trigger event\n");
		free(Locs);
		return RC;
	}

	/* Clear timer reset event register */
	_XAie_ClearTimerConfig(DevInst, NumTiles, Locs);

	/* Clear broadcast setting */
	_XAie_ClearBroadcastConfig(DevInst, NumTiles, Locs, BcastChannelId);

	/* Clear shim broadcast configuration */
	_XAie_ClearShimBroadcast(DevInst, 0, DevInst->NumCols, BcastChannelId);

	free(Locs);
	return XAIE_OK;
}

/*****************************************************************************/
/**
 * This API synchronizes timer for all tiles for all modules in the partition.
 *
 * @param        DevInst - Device Instance.
 * @param        BcastChannelId - Broadcast channel id for timer sync.
 *
 * @return       XAIE_OK on success
 *               XAIE_INVALID_ARGS if any argument is invalid
 *               XAIE_INVALID_TILE if tile type from Loc is invalid
 ******************************************************************************/
AieRC XAie_SyncTimerWithTwoBcstChannel(XAie_DevInst *DevInst, u8 BcastChannelId1,
	u8 BcastChannelId2)
{
	AieRC RC;
	u32 NumTiles;
	XAie_LocType *Locs;
	XAie_Events ShimBcastEvent;

	if((DevInst == XAIE_NULL) ||
			(DevInst->IsReady != XAIE_COMPONENT_IS_READY)){
		XAIE_ERROR("Invalid Device Instance\n");
		return XAIE_INVALID_ARGS;
	}

	/* Get all ungated tiles to broadcast to */
	NumTiles = (u32)((u32)DevInst->NumCols * (u32)DevInst->NumRows);
	Locs = (XAie_LocType *)malloc(NumTiles * sizeof(XAie_LocType));
	if(Locs == NULL) {
		XAIE_ERROR("Unable to allocate memory for tile locations\n");
		return XAIE_ERR;
	}

	RC = XAie_GetUngatedLocsInPartition(DevInst, &NumTiles, Locs);
	if(RC != XAIE_OK) {
		free(Locs);
		return RC;
	}

	RC = _XAie_SetupBroadcastConfig(DevInst, NumTiles, Locs, BcastChannelId1);
	if(RC != XAIE_OK) {
		XAIE_ERROR("Failed to setup broadcast network for timer sync\n");
		free(Locs);
		return RC;
	}

	ShimBcastEvent = _XAie_GetBroadcastEventfromRscId(DevInst,
			XAie_TileLoc(0, 0), XAIE_PL_MOD, BcastChannelId2);

	for(u32 i = 0; i < DevInst->NumCols; i++) {
		XAie_LocType Loc = XAie_TileLoc((u8)i, 0);

		RC = XAie_EventBroadcast(DevInst, Loc, XAIE_PL_MOD,
				BcastChannelId1, ShimBcastEvent);
		if (i == 0) {
			RC = XAie_EventBroadcast(DevInst, Loc, XAIE_PL_MOD,
					BcastChannelId2 , ShimBcastEvent);
		}
		if(RC != XAIE_OK) {
			XAIE_ERROR("Unable to configure shim broadcast event for timer sync\n");
			free(Locs);
			return RC;
		}
	}

	/* Configure the timer control with the trigger event */
	RC = _XAie_SetupTimerConfig(DevInst, NumTiles, Locs, BcastChannelId1);
	if(RC != XAIE_OK) {
		XAIE_ERROR("Failed to setup timer reset events\n");
		free(Locs);
		return RC;
	}

	/* Trigger Event */
	RC = XAie_EventGenerate(DevInst, XAie_TileLoc(0, 0), XAIE_PL_MOD,
			ShimBcastEvent);
	if(RC != XAIE_OK) {
		XAIE_ERROR("Unable to trigger event\n");
		free(Locs);
		return RC;
	}

	/* Clear timer reset event register */
	_XAie_ClearTimerConfig(DevInst, NumTiles, Locs);

	/* Clear broadcast setting */
	_XAie_ClearBroadcastConfig(DevInst, NumTiles, Locs, BcastChannelId1);

	/* Clear shim broadcast configuration for Chan1 */
	_XAie_ClearShimBroadcast(DevInst, 0, DevInst->NumCols, BcastChannelId1);

	/* Clear shim broadcast configuration for Chan2 */
	_XAie_ClearShimBroadcast(DevInst, 0, DevInst->NumCols, BcastChannelId2);

	free(Locs);
	return XAIE_OK;
}
#endif /* XAIE_FEATURE_TIMER_ENABLE */
