/******************************************************************************
* Copyright (C) 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*****************************************************************************/
/**
* @file xaie_io_privilege.c
* @{
*
* This file contains privilege routines for io backends.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date        Changes
* ----- ------  --------    ---------------------------------------------------
* 1.0   Wendy 05/17/2021  Initial creation
*
* </pre>
*
******************************************************************************/
/***************************** Include Files *********************************/
#include <stdlib.h>

#include "xaie_clock.h"
#include "xaie_reset_aie.h"
#include "xaie_feature_config.h"
#include "xaie_helper.h"
#include "xaie_io_privilege.h"
#include "xaie_npi.h"

#if defined(XAIE_FEATURE_PRIVILEGED_ENABLE) && !defined(XAIE_FEATURE_LITE)

/*****************************************************************************/
/***************************** Macro Definitions *****************************/

#define XAIE_ISOLATE_EAST_MASK	(1U << 3)
#define XAIE_ISOLATE_NORTH_MASK	(1U << 2)
#define XAIE_ISOLATE_WEST_MASK	(1U << 1)
#define XAIE_ISOLATE_SOUTH_MASK	(1U << 0)
#define XAIE_ISOLATE_ALL_MASK	((1U << 4) - 1)

#define XAIE_ERROR_NPI_INTR_ID	0x1U
/************************** Function Definitions *****************************/
/*****************************************************************************/
/**
*
* This API set the tile column reset
*
* @param	DevInst: Device Instance
* @param	Loc: Location of AIE SHIM tile
* @param	RstEnable: XAIE_ENABLE to assert reset, XAIE_DISABLE to
*			   deassert reset.
*
* @return       XAIE_OK on success, error code on failure
*
* @note		It is not required to check the DevInst and the Loc tile type
*		as the caller function should provide the correct value.
*		This function is internal to this file.
*
******************************************************************************/
static AieRC _XAie_PrivilegeSetColReset(XAie_DevInst *DevInst,
		XAie_LocType Loc, u8 RstEnable)
{
	u8 TileType;
	u32 FldVal;
	u64 RegAddr;
	const XAie_PlIfMod *PlIfMod;

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	PlIfMod = DevInst->DevProp.DevMod[TileType].PlIfMod;
	RegAddr = PlIfMod->ColRstOff +
		XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col);
	FldVal = XAie_SetField(RstEnable,
			PlIfMod->ColRst.Lsb,
			PlIfMod->ColRst.Mask);

	return XAie_Write32(DevInst, RegAddr, FldVal);
}

/*****************************************************************************/
/**
*
* This API set the tile columns reset for every column in the partition
*
* @param	DevInst: Device Instance
* @param	RstEnable: XAIE_ENABLE to assert reset, XAIE_DISABLE to
*			   deassert reset.
*
* @return       XAIE_OK on success, error code on failure
*
* @note		It is not required to check the DevInst as the caller function
*		should provide the correct value.
*		This function is internal to this file.
*
******************************************************************************/
static AieRC _XAie_PrivilegeSetPartColReset(XAie_DevInst *DevInst,
		u8 RstEnable)
{
	AieRC RC = XAIE_OK;

	for(u8 C = 0; C < DevInst->NumCols; C++) {
		XAie_LocType Loc = XAie_TileLoc(C, 0U);

		RC = _XAie_PrivilegeSetColReset(DevInst, Loc, RstEnable);
		if(RC != XAIE_OK) {
			XAIE_ERROR("Failed to reset columns.\n");
			break;
		}
	}

	return RC;
}
/*****************************************************************************/
/**
*
* This API reset all SHIMs in the AI engine partition
*
* @param	DevInst: Device Instance
*
* @return	XAIE_OK for success, and error value for failure
*
* @note		This function asserts reset, and then deassert it.
*		It is not required to check the DevInst as the caller function
*		should provide the correct value.
*		This function is internal to this file.
*
******************************************************************************/
static AieRC _XAie_PrivilegeRstPartShims(XAie_DevInst *DevInst)
{
	AieRC RC;

	RC = DevInst->DevOps->SetPartColShimReset(DevInst, XAIE_ENABLE);
	if(RC != XAIE_OK) {
		return RC;
	}

	RC = _XAie_NpiSetShimReset(DevInst, XAIE_ENABLE);
	if(RC != XAIE_OK) {
		return RC;
	}

	return _XAie_NpiSetShimReset(DevInst, XAIE_DISABLE);
}

/*****************************************************************************/
/**
*
* This API sets to block NSU AXI MM slave error and decode error based on user
* inputs. If NSU errors is blocked, it will only generate error events.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of AIE SHIM tile
* @param	BlockSlvEnable: XAIE_ENABLE to block NSU AXI MM Slave errors,
*				or XAIE_DISABLE to unblock.
* @param	BlockDecEnable: XAIE_ENABLE to block NSU AXI MM Decode errors,
*				or XAIE_DISABLE to unblock.
*
* @return       XAIE_OK on success, error code on failure
*
* @note		It is not required to check the DevInst and the Loc tile type
*		as the caller function should provide the correct value.
*		This function is internal to this file.
*
******************************************************************************/
static AieRC _XAie_PrivilegeSetBlockAxiMmNsuErr(XAie_DevInst *DevInst,
		XAie_LocType Loc, u8 BlockSlvEnable, u8 BlockDecEnable)
{
	u8 TileType;
	u32 FldVal;
	u64 RegAddr;
	const XAie_PlIfMod *PlIfMod;
	const XAie_ShimNocAxiMMConfig *ShimNocAxiMM;

	TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
	PlIfMod = DevInst->DevProp.DevMod[TileType].PlIfMod;
	ShimNocAxiMM = PlIfMod->ShimNocAxiMM;
	RegAddr = ShimNocAxiMM->RegOff +
		XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col);
	FldVal = XAie_SetField(BlockSlvEnable,
			ShimNocAxiMM->NsuSlvErr.Lsb,
			ShimNocAxiMM->NsuSlvErr.Mask);
	FldVal |= XAie_SetField(BlockDecEnable,
			ShimNocAxiMM->NsuDecErr.Lsb,
			ShimNocAxiMM->NsuDecErr.Mask);

	return XAie_Write32(DevInst, RegAddr, FldVal);
}

/*****************************************************************************/
/**
*
* This API sets to block the NSU AXI MM slave error and decode error config
* for all the SHIM NOCs in the full partition based on user inputs.
*
* @param	DevInst: Device Instance
* @param	BlockSlvEnable: XAIE_ENABLE to block NSU AXI MM Slave errors,
*				or XAIE_DISABLE to unblock.
* @param	BlockDecEnable: XAIE_ENABLE to block NSU AXI MM Decode errors,
*				or XAIE_DISABLE to unblock.
*
* @return       XAIE_OK on success, error code on failure
*
* @note		It is not required to check the DevInst as the caller function
*		should provide the correct value.
*		This function will do the following steps:
*		 * set AXI MM registers NSU errors fields in all SHIM NOC tiles
*		This function is internal to this file.
*
******************************************************************************/
static
AieRC _XAie_PrivilegeSetPartBlockAxiMmNsuErr(XAie_DevInst *DevInst,
		u8 BlockSlvEnable, u8 BlockDecEnable)
{
	AieRC RC = XAIE_OK;

	for(u8 C = 0; C < DevInst->NumCols; C++) {
		XAie_LocType Loc = XAie_TileLoc(C, 0U);
		u8 TileType;

		TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
		if(TileType != XAIEGBL_TILE_TYPE_SHIMNOC) {
			continue;
		}
		RC = _XAie_PrivilegeSetBlockAxiMmNsuErr(DevInst, Loc,
				BlockSlvEnable, BlockDecEnable);
		if(RC != XAIE_OK) {
			XAIE_ERROR("Failed to set SHIM NOC AXI MM Errors.");
			return RC;
		}
	}

	return RC;
}

/*****************************************************************************/
/**
* This API sets partition NPI protected register enabling
*
* @param	DevInst: AI engine partition device instance pointer
* @param	Enable: XAIE_ENABLE to enable access to protected register.
*			XAIE_DISABLE to disable access.
*
* @return       XAIE_OK on success, error code on failure
*
* @note		This function is internal to this file.
*
*******************************************************************************/
static AieRC _XAie_PrivilegeSetPartProtectedRegs(XAie_DevInst *DevInst,
		u8 Enable)
{
	AieRC RC;
	XAie_NpiProtRegReq NpiProtReq = {0};

	NpiProtReq.NumCols = DevInst->NumCols;
	NpiProtReq.Enable = Enable;
	RC = _XAie_NpiSetProtectedRegEnable(DevInst, &NpiProtReq);
	if(RC != XAIE_OK) {
		XAIE_ERROR("Failed to set protected registers.\n");
	}

	return RC;
}

/*****************************************************************************/
/**
*
* This API sets NoC interrupt ID to which the interrupt from second level
* interrupt controller shall be driven to.
*
* @param	DevInst: Device Instance
* @param	Loc: Location of AIE Tile
* @param	NoCIrqId: NoC IRQ index on which the interrupt shall be
*			  driven.
*
* @return	XAIE_OK on success, error code on failure.
*
* @note		None.
*
******************************************************************************/
static AieRC _XAie_PrivilegeSetL2IrqId(XAie_DevInst *DevInst, XAie_LocType Loc,
		u8 NoCIrqId)
{
	u64 RegAddr;
	u32 RegOffset;
	const XAie_L2IntrMod *IntrMod;

	IntrMod = DevInst->DevProp.DevMod[XAIEGBL_TILE_TYPE_SHIMNOC].L2IntrMod;
	RegOffset = IntrMod->IrqRegOff;
	RegAddr = XAie_GetTileAddr(DevInst, Loc.Row, Loc.Col) + RegOffset;
	return XAie_Write32(DevInst, RegAddr, NoCIrqId);
}

/*****************************************************************************/
/**
*
* This API sets NoC interrupt ID to which the error interrupts from second
* level interrupt controller shall be driven to. All the second level interrupt
* controllers with a given partition are configured.
*
* @param	DevInst: Device Instance
*
* @return	XAIE_OK on success, error code on failure.
*
* @note		None.
*
******************************************************************************/
static AieRC _XAie_PrivilegeSetL2ErrIrq(XAie_DevInst *DevInst)
{
	AieRC RC;
	XAie_LocType Loc = XAie_TileLoc(0, DevInst->ShimRow);

	for (Loc.Col = 0; Loc.Col < DevInst->NumCols; Loc.Col++) {
		u8 TileType = DevInst->DevOps->GetTTypefromLoc(DevInst, Loc);
		if (TileType != XAIEGBL_TILE_TYPE_SHIMNOC) {
			continue;
		}

		RC = _XAie_PrivilegeSetL2IrqId(DevInst, Loc,
				XAIE_ERROR_NPI_INTR_ID);
		if(RC != XAIE_OK) {
			XAIE_ERROR("Failed to configure L2 error IRQ channel\n");
			return RC;
		}
	}

	return XAIE_OK;
}

/*****************************************************************************/
/**
*
* This API Enable/Disable interleaving mode for all MemTiles of the partition.
*
* @param	DevInst: Device Instance
* @param	Enable: 0/1 to disable/enable memory interleaving mode
*
* @return       XAIE_OK on success, error code on failure
*
* @note		It is not required to check the DevInst as the caller function
*		should provide the correct value.
*		Internal API only.
*
******************************************************************************/
static AieRC _XAie_PrivilegeConfigMemInterleaving(XAie_DevInst *DevInst, u8 Enable)
{
	AieRC RC;
	u64 RegAddr;
	u32 FldVal;
	const XAie_MemCtrlMod *MCtrlMod;
	u8 C, R;

	for(C = 0; C < DevInst->NumCols; C++) {
		for(R = DevInst->MemTileRowStart;
		    R < (DevInst->MemTileRowStart + DevInst->MemTileNumRows);
		    R++) {
			MCtrlMod = DevInst->DevProp.DevMod[XAIEGBL_TILE_TYPE_MEMTILE].MemCtrlMod;
			RegAddr = MCtrlMod->MemCtrlRegOff +
					XAie_GetTileAddr(DevInst, R, C);
			FldVal = XAie_SetField(Enable,
					       MCtrlMod->MemInterleaving.Lsb,
					       MCtrlMod->MemInterleaving.Mask);
			RC = XAie_MaskWrite32(DevInst, RegAddr,
					      MCtrlMod->MemInterleaving.Mask,
					      FldVal);
			if(RC != XAIE_OK) {
				XAIE_ERROR("Failed to config memory interleaving"
						" for partition.\n");
				return RC;
			}
		}
	}
	return RC;
}

/*****************************************************************************/
/**
* This API initializes the AI engine partition
*
* @param	DevInst: AI engine partition device instance pointer
* @param	Opts: Initialization options
*
* @return       XAIE_OK on success, error code on failure
*
* @note		This operation does the following steps to initialize an AI
*		engine partition:
*		- Clock gate all columns
*		- Reset Columns
*		- Ungate all Columns
*		- Remove columns reset
*		- Reset shims
*		- Setup AXI MM not to return errors for AXI decode or slave
*		  errors, raise events instead.
*		- ungate all columns
*		- Setup partition isolation.
*		- zeroize memory if it is requested
*
*******************************************************************************/
AieRC _XAie_PrivilegeInitPart(XAie_DevInst *DevInst, XAie_PartInitOpts *Opts)
{
	u32 OptFlags;
	AieRC RC;

	if(Opts != NULL) {
		OptFlags = Opts->InitOpts;
	} else {
		OptFlags = XAIE_PART_INIT_OPT_DEFAULT;
	}

	RC = _XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_ENABLE);
	if(RC != XAIE_OK) {
		XAIE_ERROR("Failed to initialize partition, enable protected registers failed.\n");
		return RC;
	}

	if((OptFlags & XAIE_PART_INIT_OPT_COLUMN_RST) != 0U) {
		/* Gate all tiles before resetting columns to quiet traffic*/
		RC = _XAie_PmSetPartitionClock(DevInst, XAIE_DISABLE);
		if(RC != XAIE_OK) {
			_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
			return RC;
		}

		RC = _XAie_PrivilegeSetPartColReset(DevInst, XAIE_ENABLE);
		if(RC != XAIE_OK) {
			_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
			return RC;
		}

		/* Enable clock buffer before removing column reset */
		RC = _XAie_PmSetPartitionClock(DevInst, XAIE_ENABLE);
		if(RC != XAIE_OK) {
			_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
			return RC;
		}

		RC = _XAie_PrivilegeSetPartColReset(DevInst, XAIE_DISABLE);
		if(RC != XAIE_OK) {
			_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
			return RC;
		}

	}

	if((OptFlags & XAIE_PART_INIT_OPT_SHIM_RST) != 0U) {
		RC = _XAie_PrivilegeRstPartShims(DevInst);
		if(RC != XAIE_OK) {
			_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
			return RC;
		}
	}

	if((OptFlags & XAIE_PART_INIT_OPT_BLOCK_NOCAXIMMERR) != 0U) {
		RC = _XAie_PrivilegeSetPartBlockAxiMmNsuErr(DevInst,
			XAIE_ENABLE, XAIE_ENABLE);
		if(RC != XAIE_OK) {
			_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
			return RC;
		}
	}

	RC = DevInst->DevOps->SetPartColClockAfterRst(DevInst, XAIE_ENABLE);
	if(RC != XAIE_OK) {
		_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
		return RC;
	}

	if ((OptFlags & XAIE_PART_INIT_OPT_ISOLATE) != 0U) {
		RC = DevInst->DevOps->SetPartIsolationAfterRst(DevInst);
		if(RC != XAIE_OK) {
			_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
		}
	}

	if ((OptFlags & XAIE_PART_INIT_OPT_ZEROIZEMEM) != 0U) {
		RC = DevInst->DevOps->PartMemZeroInit(DevInst);
		if(RC != XAIE_OK) {
			_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
			return RC;
		}
	}

	if ((OptFlags & XAIE_PART_INIT_OPT_DISABLE_MEMINTERLEAVING) != 0U) {
		RC = _XAie_PrivilegeConfigMemInterleaving(DevInst, XAIE_DISABLE);
		if(RC != XAIE_OK) {
			_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
			return RC;
		}
	}

	RC = _XAie_PrivilegeSetL2ErrIrq(DevInst);
	if(RC != XAIE_OK) {
		XAIE_ERROR("Failed to configure L2 error IRQ channels\n");
		return RC;
	}

	/*
	 * This is a temporary workaround to unblock rel-v2023.1 and make
	 * XAie_PartitionInitialize() consistent with XAie_ResetPartition().
	 */
	if (DevInst->DevProp.DevGen == XAIE_DEV_GEN_AIE) {
		RC = _XAie_PmSetPartitionClock(DevInst, XAIE_DISABLE);
		if (RC != XAIE_OK) {
			_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
			return RC;
		}
	}

	/* Enable only the tiles requested in Opts parameter */
	if(Opts != NULL) {
		XAie_BackendTilesArray TilesArray;

		TilesArray.NumTiles = Opts->NumUseTiles;
		TilesArray.Locs = Opts->Locs;

		RC = XAie_RunOp(DevInst, XAIE_BACKEND_OP_REQUEST_TILES,
		(void *)&TilesArray);

		if(RC != XAIE_OK) {
			_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
			return RC;
		}
	}

	RC = _XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
	return RC;
}

/*****************************************************************************/
/**
* This API tears down the AI engine partition
*
* @param	DevInst: AI engine partition device instance pointer
* @param	Opts: Initialization options
*
* @return       XAIE_OK on success, error code on failure
*
* @note		This operation does the following steps to initialize an AI
*		engine partition:
*		- Clock gate all columns
*		- Reset Columns
*		- Ungate all columns
*		- Reset shims
*		- Remove columns reset
*		- Ungate all columns
*		- Zeroize memories
*		- Clock gate all columns
*
*******************************************************************************/
AieRC _XAie_PrivilegeTeardownPart(XAie_DevInst *DevInst)
{
	AieRC RC;

	RC = _XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_ENABLE);
	if(RC != XAIE_OK) {
		XAIE_ERROR("Failed to teardown partition, enable protected registers failed.\n");
		return RC;
	}

	RC = _XAie_PmSetPartitionClock(DevInst, XAIE_DISABLE);
	if(RC != XAIE_OK) {
		_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
		return RC;
	}

	RC = _XAie_PrivilegeSetPartColReset(DevInst, XAIE_ENABLE);
	if(RC != XAIE_OK) {
		_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
		return RC;
	}

	RC = _XAie_PmSetPartitionClock(DevInst, XAIE_ENABLE);
	if(RC != XAIE_OK) {
		_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
		return RC;
	}

	RC = _XAie_PrivilegeSetPartColReset(DevInst, XAIE_DISABLE);
	if(RC != XAIE_OK) {
		_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
		return RC;
	}

	RC = _XAie_PrivilegeRstPartShims(DevInst);
	if(RC != XAIE_OK) {
		_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
		return RC;
	}

	RC = DevInst->DevOps->SetPartColClockAfterRst(DevInst, XAIE_ENABLE);
	if(RC != XAIE_OK) {
		_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
		return RC;
	}

	RC = DevInst->DevOps->PartMemZeroInit(DevInst);
	if (RC != XAIE_OK) {
		_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
		return RC;
	}

	RC = _XAie_PrivilegeConfigMemInterleaving(DevInst, XAIE_ENABLE);
	if(RC != XAIE_OK) {
		_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
		return RC;
	}

	RC = _XAie_PmSetPartitionClock(DevInst, XAIE_DISABLE);
	if(RC != XAIE_OK) {
		_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
		return RC;
	}

	return _XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
}

/*****************************************************************************/
/**
* This API enables clock for all the tiles passed as argument to this API.
*
* @param	DevInst: AI engine partition device instance pointer
* @param	Args: Backend tile args
*
* @return       XAIE_OK on success, error code on failure
*
* @note		Internal only.
*
*******************************************************************************/
AieRC _XAie_PrivilegeRequestTiles(XAie_DevInst *DevInst,
		XAie_BackendTilesArray *Args)
{
	AieRC RC;
	/* TODO: Configure previlege registers only for non-AIE devices. */
	if(DevInst->DevProp.DevGen != XAIE_DEV_GEN_AIE) {
		RC = _XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_ENABLE);
		if(RC != XAIE_OK) {
			XAIE_ERROR("Failed to initialize partition, enable"
					" protected registers failed.\n");
			 return RC;
		}
	}

	RC = DevInst->DevOps->RequestTiles(DevInst, Args);
	if(RC != XAIE_OK) {
		XAIE_ERROR("Request tiles failed\n");
	}

	if (DevInst->DevProp.DevGen != XAIE_DEV_GEN_AIE) {
		_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
	}

	return RC;
}

/*****************************************************************************/
/**
* This API enables column clock and module clock control register for requested
* tiles passed as argument to this API.
*
* @param	DevInst: AI engine partition device instance pointer
* @param	Args: Backend tile args
*
* @return       XAIE_OK on success, error code on failure
*
* @note		Internal only.
*
*******************************************************************************/
AieRC _XAie_PrivilegeSetColumnClk(XAie_DevInst *DevInst,
		XAie_BackendColumnReq *Args)
{
	AieRC RC;
	/* TODO: Configure previlege registers only for non-AIE devices. */
	if(DevInst->DevProp.DevGen != XAIE_DEV_GEN_AIE) {
		RC = _XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_ENABLE);
		if(RC != XAIE_OK) {
			XAIE_ERROR("Failed to initialize partition, enable"
					" protected registers failed.\n");
			 return RC;
		}
	}

	RC = DevInst->DevOps->SetColumnClk(DevInst, Args);
	if(RC != XAIE_OK) {
		XAIE_ERROR("Set Column Clock failed\n");
	}

	if (DevInst->DevProp.DevGen != XAIE_DEV_GEN_AIE) {
		_XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
	}

	return RC;
}

/*****************************************************************************/
/**
 *
 * This API Enable/Disable interleaving mode for all MemTiles of the partition.
 *
 * @param	DevInst: Device Instance
 * @param	Enable: 0/1 to disable/enable memory interleaving mode
 *
 * @return       XAIE_OK on success, error code on failure
 *
 * @note		It is not required to check the DevInst as the caller function
 *		should provide the correct value.
 *		Internal API only.
 *
 ******************************************************************************/
AieRC _XAie_PrivilegeConfigMemInterleavingLoc(XAie_DevInst *DevInst,
		XAie_BackendTilesEnableArray *Args)
{
	AieRC RC;
	u64 RegAddr;
	u32 FldVal;
	const XAie_MemCtrlMod *MCtrlMod;
	u32 i;

	RC = _XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_ENABLE);
	if(RC != XAIE_OK) {
		XAIE_ERROR("Failed to configure memory interleaving,"
				" enable protected registers failed.\n");
		return RC;
	}

	MCtrlMod = DevInst->DevProp.DevMod[XAIEGBL_TILE_TYPE_MEMTILE].MemCtrlMod;
	for (i = 0; i < Args->NumTiles; i++) {
		RegAddr = MCtrlMod->MemCtrlRegOff +
			XAie_GetTileAddr(DevInst, Args->Locs[i].Row, Args->Locs[i].Col);
		FldVal = XAie_SetField(Args->Enable ? XAIE_ENABLE : XAIE_DISABLE,
				MCtrlMod->MemInterleaving.Lsb,
				MCtrlMod->MemInterleaving.Mask);
		RC = XAie_MaskWrite32(DevInst, RegAddr,
				MCtrlMod->MemInterleaving.Mask,
				FldVal);
		if(RC != XAIE_OK) {
			XAIE_ERROR("Failed to config memory interleaving, Loc (%d, %d)\n",
					Args->Locs[i].Row, Args->Locs[i].Col);
			_XAie_PrivilegeSetPartProtectedRegs(DevInst,
					XAIE_DISABLE);
			return RC;
		}
	}

	return _XAie_PrivilegeSetPartProtectedRegs(DevInst, XAIE_DISABLE);
}
#else /* XAIE_FEATURE_PRIVILEGED_ENABLE */
AieRC _XAie_PrivilegeInitPart(XAie_DevInst *DevInst, XAie_PartInitOpts *Opts)
{
	(void)DevInst;
	(void)Opts;
	return XAIE_FEATURE_NOT_SUPPORTED;
}

AieRC _XAie_PrivilegeTeardownPart(XAie_DevInst *DevInst)
{
	(void)DevInst;
	return XAIE_FEATURE_NOT_SUPPORTED;
}

AieRC _XAie_PrivilegeRequestTiles(XAie_DevInst *DevInst,
		XAie_BackendTilesArray *Args)
{
	(void)DevInst;
	(void)Args;
	return XAIE_FEATURE_NOT_SUPPORTED;
}


AieRC _XAie_PrivilegeSetColumnClk(XAie_DevInst *DevInst,
		XAie_BackendColumnReq *Args)
{
	(void)DevInst;
	(void)Args;
	return XAIE_FEATURE_NOT_SUPPORTED;
}

AieRC _XAie_PrivilegeConfigMemInterleavingLoc(XAie_DevInst *DevInst,
		XAie_BackendTilesEnableArray *Args)
{
	(void)DevInst;
	(void)Args;
	return XAIE_FEATURE_NOT_SUPPORTED;
}
#endif /* XAIE_FEATURE_PRIVILEGED_ENABLE && !XAIE_FEATURE_LITE */
/** @} */
