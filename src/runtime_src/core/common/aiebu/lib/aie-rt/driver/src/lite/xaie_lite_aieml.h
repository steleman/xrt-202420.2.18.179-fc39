/******************************************************************************
* Copyright (C) 2023, Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*****************************************************************************/
/**
* @file xaie_lite_aieml.h
* @{
*
* This header file defines a lightweight version of AIEML specific register
* operations.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------  -------- -----------------------------------------------------
* 1.0   Wendy   09/06/2021  Initial creation
* </pre>
*
******************************************************************************/
#ifndef XAIE_LITE_AIEML_H
#define XAIE_LITE_AIEML_H

/***************************** Include Files *********************************/
#include "xaie_lite_hwcfg.h"
#include "xaie_lite_io.h"
#include "xaie_lite_npi.h"
#if ((XAIE_DEV_SINGLE_GEN == XAIE_DEV_GEN_AIE2P) || \
	(XAIE_DEV_SINGLE_GEN == XAIE_DEV_GEN_AIE2P_STRIX_B0) || \
	(XAIE_DEV_SINGLE_GEN == XAIE_DEV_GEN_AIE2P_STRIX_A0))
#include "xaie_lite_regdef_aie2p.h"
#else
#include "xaie_lite_regdef_aieml.h"
#endif
#include "xaiegbl_defs.h"
#include "xaiegbl.h"
#include "xaie_lite_util.h"

/* AIE core registers step size */
#define AIE_CORE_REGS_STEP              0x10

/************************** Constant Definitions *****************************/
/************************** Function Prototypes  *****************************/
#if defined(XAIE_FEATURE_LITE_UTIL)
/*****************************************************************************/
/**
*
* This API returns the Aie Tile Core status for a particular column and row.
*
* @param	DevInst: Device Instance
* @param	Status: Pointer to user defined column status buffer.
* @param	Col: Column number.
* @param	Row: Row number.
*
* @return	None.
*
* @note		Internal only.
*
******************************************************************************/
__FORCE_INLINE__
static inline void _XAie_LCoreStatus(XAie_DevInst *DevInst, XAie_Col_Status *Status, u32 Col, u32 Row)
{
	u64 RegAddr;

	/* core status addr */
	RegAddr = _XAie_LGetTileAddr(Row + XAIE_AIE_TILE_ROW_START, Col + DevInst->StartCol)
		+ XAIE_AIE_TILE_CORE_STATUS_REGOFF;

	/* core status */
	Status[Col].CoreTile[Row].CoreStatus =
		(_XAie_LPartRead32(DevInst, RegAddr)
		 & XAIE_AIE_TILE_CORE_STATUS_MASK);

	/* core program counter addr */
	RegAddr = _XAie_LGetTileAddr(Row + XAIE_AIE_TILE_ROW_START, Col + DevInst->StartCol)
		+ XAIE_AIE_TILE_CORE_PC_REGOFF;

	/* program counter */
	Status[Col].CoreTile[Row].ProgramCounter =
		(_XAie_LPartRead32(DevInst, RegAddr)
		 & XAIE_AIE_TILE_CORE_PC_MASK);

	/* core stack pointer addr */
	RegAddr = _XAie_LGetTileAddr(Row + XAIE_AIE_TILE_ROW_START, Col + DevInst->StartCol)
		+ XAIE_AIE_TILE_CORE_SP_REGOFF;

	/* stack pointer */
	Status[Col].CoreTile[Row].StackPtr =
		(_XAie_LPartRead32(DevInst, RegAddr)
		 & XAIE_AIE_TILE_CORE_SP_MASK);

	/* core link addr */
	RegAddr = _XAie_LGetTileAddr(Row + XAIE_AIE_TILE_ROW_START, Col + DevInst->StartCol)
		+ XAIE_AIE_TILE_CORE_LR_REGOFF;

	/* link register */
	Status[Col].CoreTile[Row].LinkReg =
		(_XAie_LPartRead32(DevInst, RegAddr)
		 & XAIE_AIE_TILE_CORE_LR_MASK);
}

/*****************************************************************************/
/**
*
* This API returns the Aie Tile DMA status for a particular column and row.
*
* @param	DevInst: Device Instance
* @param	Status: Pointer to user defined column status buffer.
* @param	Col: Column number.
* @param	Row: Row number.
*
* @return	None.
*
* @note		Internal only.
*
******************************************************************************/
__FORCE_INLINE__
static inline void _XAie_LCoreDMAStatus(XAie_DevInst *DevInst, XAie_Col_Status *Status, u32 Col, u32 Row)
{
	u64 RegAddr;

	/* iterate all tile dma channels */
	for (u32 Chan = 0; Chan < XAIE_TILE_DMA_NUM_CH; Chan++) {

		/* s2mm channel address */
		RegAddr = _XAie_LGetTileAddr(Row + XAIE_AIE_TILE_ROW_START, Col + DevInst->StartCol)
			+ Chan * XAIE_TILE_DMA_S2MM_CHANNEL_STATUS_IDX + XAIE_TILE_DMA_S2MM_CHANNEL_STATUS_REGOFF;

		/* read s2mm channel status */
		Status[Col].CoreTile[Row].dma[Chan].S2MMStatus =
			(_XAie_LPartRead32(DevInst, RegAddr) & XAIE_TILE_DMA_S2MM_CHANNEL_VALID_BITS_MASK);

		/* mm2s channel address */
		RegAddr = _XAie_LGetTileAddr(Row + XAIE_AIE_TILE_ROW_START, Col + DevInst->StartCol)
			+ Chan * XAIE_TILE_DMA_MM2S_CHANNEL_STATUS_IDX + XAIE_TILE_DMA_MM2S_CHANNEL_STATUS_REGOFF;

		/* read mm2s channel status */
		Status[Col].CoreTile[Row].dma[Chan].MM2SStatus =
			(_XAie_LPartRead32(DevInst, RegAddr) & XAIE_TILE_DMA_MM2S_CHANNEL_VALID_BITS_MASK);
	}

}

/*****************************************************************************/
/**
*
* This API returns the Aie Lock status for a particular column and row.
*
* @param	DevInst: Device Instance
* @param	Status: Pointer to user defined column status buffer.
* @param	Col: Column number.
* @param    Row: Row number.
*
* @return	None.
*
* @note		Internal only.
*
******************************************************************************/
__FORCE_INLINE__
static inline void _XAie_LCoreLockValue(XAie_DevInst *DevInst, XAie_Col_Status *Status, u32 Col, u32 Row)
{
	u64 RegAddr;

	/* iterate all lock value registers */
	for(u32 Lock = 0; Lock < XAIE_TILE_NUM_LOCKS; Lock++) {

		/* lock value address */
		RegAddr = _XAie_LGetTileAddr(Row + XAIE_AIE_TILE_ROW_START, Col + DevInst->StartCol)
			+ Lock * XAIE_AIE_TILE_LOCK_VALUE_IDX + XAIE_AIE_TILE_LOCK_VALUE_REGOFF;

		/* read lock value */
		Status[Col].CoreTile[Row].LockValue[Lock] =
			(u8)(_XAie_LPartRead32(DevInst, RegAddr)
			& XAIE_AIE_TILE_LOCK_VALUE_MASK);
	}
}

/*****************************************************************************/
/**
*
* This API returns the Core Tile Event Status for a particular column and row.
*
* @param	DevInst: Device Instance
* @param	Status: Pointer to user defined column status buffer.
* @param	Col: Column number.
* @param    Row: Row number.
*
* @return	None.
*
* @note		Internal only.
*
******************************************************************************/
__FORCE_INLINE__
static inline void _XAie_LCoreEventStatus(XAie_DevInst *DevInst, XAie_Col_Status *Status, u32 Col, u32 Row)
{
	u64 RegAddr;

	/* iterate all event status registers */
	for(u32 EventReg = 0; EventReg < XAIE_CORE_TILE_NUM_EVENT_STATUS_REGS; EventReg++) {

		/* event status address */
		RegAddr = _XAie_LGetTileAddr(Row + XAIE_AIE_TILE_ROW_START, Col + DevInst->StartCol)
			+ EventReg * XAIE_AIE_TILE_CORE_MOD_EVENT_STATUS_IDX + XAIE_AIE_TILE_CORE_MOD_EVENT_STATUS_REGOFF;

		/* read event status register and store in output buffer */
		Status[Col].CoreTile[Row].EventCoreModStatus[EventReg] =
			(u32)(_XAie_LPartRead32(DevInst, RegAddr) & XAIE_AIE_TILE_CORE_MOD_EVENT_STATUS_MASK);

		/* event status address */
		RegAddr = _XAie_LGetTileAddr(Row + XAIE_AIE_TILE_ROW_START, Col + DevInst->StartCol)
			+ EventReg * XAIE_AIE_TILE_MEM_MOD_EVENT_STATUS_IDX + XAIE_AIE_TILE_MEM_MOD_EVENT_STATUS_REGOFF;

		/* read event status register and store in output buffer */
		Status[Col].CoreTile[Row].EventMemModStatus[EventReg] =
			(u32)(_XAie_LPartRead32(DevInst, RegAddr) & XAIE_AIE_TILE_MEM_MOD_EVENT_STATUS_MASK);
	}
}

/*****************************************************************************/
/**
*
* This API returns the Mem Tile DMA status for a particular column and row.
*
* @param	DevInst: Device Instance
* @param	Status: Pointer to user defined column status buffer.
* @param	Col: Column number.
* @param	Row: Row number.
*
* @return	None.
*
* @note		Internal only.
*
******************************************************************************/
__FORCE_INLINE__
static inline void _XAie_LMemDMAStatus(XAie_DevInst *DevInst, XAie_Col_Status *Status, u32 Col, u32 Row)
{
	u64 RegAddr;

	/* mem tile dma status */
	for(u32 Chan = 0; Chan < XAIE_MEM_TILE_DMA_NUM_CH; Chan++) {

		/* s2mm channel address */
		RegAddr = _XAie_LGetTileAddr(Row + XAIE_MEM_TILE_ROW_START, Col + DevInst->StartCol)
			+ Chan * XAIE_MEM_TILE_DMA_S2MM_CHANNEL_STATUS_IDX + XAIE_MEM_TILE_DMA_S2MM_CHANNEL_STATUS_REGOFF;

		/* read s2mm channel status */
		Status[Col].MemTile[Row].dma[Chan].S2MMStatus =
			(_XAie_LPartRead32(DevInst, RegAddr) & XAIE_MEM_TILE_DMA_S2MM_CHANNEL_VALID_BITS_MASK);

		/* mm2s channel address */
		RegAddr = _XAie_LGetTileAddr(Row + XAIE_MEM_TILE_ROW_START, Col + DevInst->StartCol)
			+ Chan * XAIE_MEM_TILE_DMA_MM2S_CHANNEL_STATUS_IDX + XAIE_MEM_TILE_DMA_MM2S_CHANNEL_STATUS_REGOFF;

		/* read s2mm channel status */
		Status[Col].MemTile[Row].dma[Chan].MM2SStatus =
			(_XAie_LPartRead32(DevInst, RegAddr) & XAIE_MEM_TILE_DMA_MM2S_CHANNEL_VALID_BITS_MASK);
	}
}

/*****************************************************************************/
/**
*
* This API returns the Mem Lock status for a particular column and row.
*
* @param	DevInst: Device Instance
* @param	Status: Pointer to user defined column status buffer.
* @param	Col: Column number.
* @param    Row: Row number.
*
* @return	None.
*
* @note		Internal only.
*
******************************************************************************/
__FORCE_INLINE__
static inline void _XAie_LMemLockValue(XAie_DevInst *DevInst, XAie_Col_Status *Status, u32 Col, u32 Row)
{
	u64 RegAddr;

	/* iterate all lock value registers */
	for(u32 Lock = 0; Lock < XAIE_MEM_TILE_NUM_LOCKS; Lock++) {

		/* lock value address */
		RegAddr = _XAie_LGetTileAddr(Row + XAIE_MEM_TILE_ROW_START, Col + DevInst->StartCol)
			+ Lock * XAIE_MEM_TILE_LOCK_VALUE_IDX + XAIE_MEM_TILE_LOCK_VALUE_REGOFF;

		/* read lock value */
		Status[Col].MemTile[Row].LockValue[Lock] =
			(u8)(_XAie_LPartRead32(DevInst, RegAddr)
			& XAIE_MEM_TILE_LOCK_VALUE_MASK);
	}
}

/*****************************************************************************/
/**
*
* This API returns the Mem Event status for a particular column and row.
*
* @param	DevInst: Device Instance
* @param	Status: Pointer to user defined column status buffer.
* @param	Col: Column number.
* @param    Row: Row number.
*
* @return	None.
*
* @note		Internal only.
*
******************************************************************************/
__FORCE_INLINE__
static inline void _XAie_LMemEventStatus(XAie_DevInst *DevInst, XAie_Col_Status *Status, u32 Col, u32 Row)
{
	u64 RegAddr;

	/* iterate all Event Status registers */
	for(u32 EventReg = 0; EventReg < XAIE_MEM_TILE_NUM_EVENT_STATUS_REGS; EventReg++)
	{
		/* Event Status address */
		RegAddr = _XAie_LGetTileAddr(Row + XAIE_MEM_TILE_ROW_START, Col + DevInst->StartCol)
			+ EventReg * XAIE_MEM_TILE_EVENT_STATUS_IDX + XAIE_MEM_TILE_EVENT_STATUS_REGOFF;

		/* read Event Status register */
		Status[Col].MemTile[Row].EventStatus[EventReg] =
			(u32)(_XAie_LPartRead32(DevInst, RegAddr) & XAIE_MEM_TILE_EVENT_STATUS_MASK);
	}
}

/*****************************************************************************/
/**
*
* This API calls the Mem Tile and Core Tile sub-modules.
*
* @param	DevInst: Device Instance
* @param	Status: Pointer to user defined column status buffer.
* @param	Col: Column number.
*
* @return	None.
*
* @note		Internal only.
*
******************************************************************************/
__FORCE_INLINE__
static inline void _XAie_LTileStatus(XAie_DevInst *DevInst, XAie_Col_Status *Status, u32 Col)
{
	/* iterate all mem tile rows */
	for(u32 Row = 0; Row < XAIE_MEM_TILE_NUM_ROWS; Row++) {
		_XAie_LMemDMAStatus(DevInst, Status, Col, Row);
		_XAie_LMemLockValue(DevInst, Status, Col, Row);
		_XAie_LMemEventStatus(DevInst, Status, Col, Row);
	}

	/* iterate all aie tile rows */
	for(u32 Row = 0; Row < XAIE_AIE_TILE_NUM_ROWS; Row++) {
		_XAie_LCoreStatus(DevInst, Status, Col, Row);
		_XAie_LCoreDMAStatus(DevInst, Status, Col, Row);
		_XAie_LCoreLockValue(DevInst, Status, Col, Row);
		_XAie_LCoreEventStatus(DevInst, Status, Col, Row);
	}
}

/*****************************************************************************/
/**
*
* This API returns the Shim Lock status for a particular column.
*
* @param	DevInst: Device Instance
* @param	Status: Pointer to user defined column status buffer.
* @param	Col: Column number.
*
* @return	None.
*
* @note		Internal only.
*
******************************************************************************/
__FORCE_INLINE__
static inline void _XAie_LShimLockValue(XAie_DevInst *DevInst, XAie_Col_Status *Status, u32 Col)
{
	u64 RegAddr;

	/* iterate all lock value registers for shim tile*/
	for(u32 Lock = 0; Lock < XAIE_SHIM_NUM_LOCKS; Lock++) {

		/* lock value address */
		RegAddr = _XAie_LGetTileAddr(XAIE_SHIM_ROW, Col + DevInst->StartCol)
			+ Lock * XAIE_SHIM_TILE_LOCK_VALUE_IDX + XAIE_SHIM_TILE_LOCK_VALUE_REGOFF;

		/* read lock value */
		Status[Col].ShimTile[XAIE_SHIM_ROW].LockValue[Lock] =
			(u8)(_XAie_LPartRead32(DevInst, RegAddr)
			& XAIE_SHIM_TILE_LOCK_VALUE_MASK);
	}
}

/*****************************************************************************/
/**
*
* This API returns the Shim DMA status for a particular column.
*
* @param	DevInst: Device Instance
* @param	Status: Pointer to user defined column status buffer.
* @param	Col: Column number.
*
* @return	None.
*
* @note		Internal only.
*
******************************************************************************/
__FORCE_INLINE__
static inline void _XAie_LShimDMAStatus(XAie_DevInst *DevInst, XAie_Col_Status *Status, u32 Col)
{
	u64 RegAddr;

	/* shim dma status - fixed at row XAIE_SHIM_ROW */
	for(u32 Chan = 0; Chan < XAIE_SHIM_DMA_NUM_CH; Chan++) {
		RegAddr = _XAie_LGetTileAddr(XAIE_SHIM_ROW, Col + DevInst->StartCol) + Chan * XAIE_SHIM_DMA_S2MM_CHANNEL_STATUS_IDX +
			XAIE_SHIM_DMA_S2MM_CHANNEL_STATUS_REGOFF;

		/* read s2mm channel status */
		Status[Col].ShimTile[XAIE_SHIM_ROW].dma[Chan].S2MMStatus =
			(_XAie_LPartRead32(DevInst, RegAddr) & XAIE_SHIM_DMA_S2MM_CHANNEL_VALID_BITS_MASK);

		/* mm2s channel address */
		RegAddr = _XAie_LGetTileAddr(XAIE_SHIM_ROW, Col + DevInst->StartCol) + Chan * XAIE_SHIM_DMA_MM2S_CHANNEL_STATUS_IDX +
			XAIE_SHIM_DMA_MM2S_CHANNEL_STATUS_REGOFF;

		/* read mm2s channel status */
		Status[Col].ShimTile[XAIE_SHIM_ROW].dma[Chan].MM2SStatus =
			(_XAie_LPartRead32(DevInst, RegAddr) & XAIE_SHIM_DMA_MM2S_CHANNEL_VALID_BITS_MASK);
	}
}

/*****************************************************************************/
/**
*
* This API returns the Shim Event status for a particular column.
*
* @param	DevInst: Device Instance
* @param	Status: Pointer to user defined column status buffer.
* @param	Col: Column number.
*
* @return	None.
*
* @note		Internal only.
*
******************************************************************************/
__FORCE_INLINE__
static inline void _XAie_LShimEventStatus(XAie_DevInst *DevInst, XAie_Col_Status *Status, u32 Col)
{
	u64 RegAddr;

	/* iterate all event status registers for shim tile */
	for (u32 EventReg = 0; EventReg < XAIE_SHIM_TILE_NUM_EVENT_STATUS_REGS; EventReg++) {

		/* event status register address */
		RegAddr = _XAie_LGetTileAddr(XAIE_SHIM_ROW, Col+DevInst->StartCol)
			+ EventReg * XAIE_SHIM_TILE_EVENT_STATUS_IDX + XAIE_SHIM_TILE_EVENT_STATUS_REGOFF;

		/* read event status value */
		Status[Col].ShimTile[XAIE_SHIM_ROW].EventStatus[EventReg] =
			(u32)(_XAie_LPartRead32(DevInst, RegAddr) & XAIE_SHIM_TILE_EVENT_STATUS_MASK);
	}
}


/*****************************************************************************/
/**
*
* This API returns the Shim Tile status for a particular column. It calls
* further sub-apis.
*
* @param	DevInst: Device Instance
* @param	Status: Pointer to user defined column status buffer.
* @param	Col: Column number.
*
* @return	None.
*
* @note		Internal only.
*
******************************************************************************/
__FORCE_INLINE__
static inline void _XAie_LShimTileStatus(XAie_DevInst *DevInst, XAie_Col_Status *Status, u32 Col)
{
    _XAie_LShimDMAStatus(DevInst, Status, Col);
    _XAie_LShimLockValue(DevInst, Status, Col);
    _XAie_LShimEventStatus(DevInst, Status, Col);
}

/*****************************************************************************/
/**
*
* This API returns the column status for N number of colums.
*
* @param	DevInst: Device Instance
* @param	Status: Pointer to user defined column status buffer.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
__FORCE_INLINE__
static inline void XAie_LGetColRangeStatus(XAie_DevInst *DevInst, XAie_Col_Status *Status)
{
	u32 NumCols  = (u32)(DevInst->NumCols);

	/* iterate specified columns */
	for(u32 Col = 0; Col < NumCols; Col++) {
		_XAie_LShimTileStatus(DevInst, Status, Col);
		_XAie_LTileStatus(DevInst, Status, Col);
	}
}

#endif      /* end of #if defined(XAIE_FEATURE_LITE_UTIL) */
/*****************************************************************************/
/**
*
* This API checks if an AI engine array tile is in use.
*
* @param	DevInst: Device Instance.
* @param	Loc: Tile location.
*
* @return	XAIE_ENABLE if a tile is in use, otherwise XAIE_DISABLE.
*
* @note		Internal only.
*
******************************************************************************/
static inline u8 _XAie_LPmIsArrayTileRequested(XAie_DevInst *DevInst,
			XAie_LocType Loc)
{
	(void) DevInst;
	(void) Loc.Col;
	(void) Loc.Row;

	return XAIE_ENABLE;
}

/*****************************************************************************/
/**
*
* This API set SHIM reset in the AI engine partition
*
* @param	DevInst: Device Instance
* @param	Loc: SHIM tile location
* @param	Reset: XAIE_ENABLE to enable reset,
* 			XAIE_DISABLE to disable reset
*
* @return	XAIE_OK for success, and error value for failure
*
* @note		This function is internal.
*		This function does nothing in AIEML.
*
******************************************************************************/
static inline void _XAie_LSetPartColShimReset(XAie_DevInst *DevInst,
		XAie_LocType Loc, u8 Reset)
{
	(void)DevInst;
	(void)Loc;
	(void)Reset;
}

/*****************************************************************************/
/**
*
* This API sets isolation boundry of an AI engine partition after reset
*
* @param	DevInst: Device Instance
*
* @note		Internal API only.
*
******************************************************************************/
static inline void _XAie_LSetPartIsolationAfterRst(XAie_DevInst *DevInst)
{
	for(u8 C = 0; C < DevInst->NumCols; C++) {
		u64 RegAddr;
		u32 RegVal = 0;

		if(C == 0) {
			RegVal = XAIE_TILE_CNTR_ISOLATE_WEST_MASK;
		} else if(C == (u8)(DevInst->NumCols - 1)) {
			RegVal = XAIE_TILE_CNTR_ISOLATE_EAST_MASK;
		}

		/* Isolate boundrary of SHIM tiles */
		RegAddr = _XAie_LGetTileAddr(0, C) +
			XAIE_PL_MOD_TILE_CNTR_REGOFF;
		_XAie_LPartWrite32(DevInst, RegAddr, RegVal);

		/* Isolate boundrary of MEM tiles */
		for (u8 R = XAIE_MEM_TILE_ROW_START;
			R < XAIE_AIE_TILE_ROW_START; R++) {
			RegAddr = _XAie_LGetTileAddr(R, C) +
				XAIE_MEM_TILE_MOD_TILE_CNTR_REGOFF;
			_XAie_LPartWrite32(DevInst, RegAddr, RegVal);
		}

		/* Isolate boundrary of CORE tiles */
		for (u8 R = XAIE_AIE_TILE_ROW_START;
			R < XAIE_NUM_ROWS; R++) {
			RegAddr = _XAie_LGetTileAddr(R, C) +
				XAIE_CORE_MOD_TILE_CNTR_REGOFF;
			_XAie_LPartWrite32(DevInst, RegAddr, RegVal);
		}
	}
}

/*****************************************************************************/
/**
*
* This API initialize the memories of the partition to zero.
*
* @param	DevInst: Device Instance
*
* @return       XAIE_OK on success, error code on failure
*
* @note		Internal API only.
*
******************************************************************************/
static inline void  _XAie_LPartMemZeroInit(XAie_DevInst *DevInst)
{
	u64 RegAddr;

	for(u8 C = 0; C < DevInst->NumCols; C++) {
		for (u8 R = XAIE_MEM_TILE_ROW_START;
			R < XAIE_AIE_TILE_ROW_START; R++) {
			RegAddr = _XAie_LGetTileAddr(R, C) +
				XAIE_MEM_TILE_MOD_MEM_CNTR_REGOFF;
			_XAie_LPartMaskWrite32(DevInst, RegAddr,
				XAIE_MEM_TILE_MEM_CNTR_ZEROISATION_MASK,
				XAIE_MEM_TILE_MEM_CNTR_ZEROISATION_MASK);
		}

		/* Isolate boundrary of CORE tiles */
		for (u8 R = XAIE_AIE_TILE_ROW_START;
			R < XAIE_NUM_ROWS; R++) {
			RegAddr = _XAie_LGetTileAddr(R, C) +
				XAIE_CORE_MOD_MEM_CNTR_REGOFF;
			_XAie_LPartMaskWrite32(DevInst, RegAddr,
				XAIE_CORE_MOD_MEM_CNTR_ZEROISATION_MASK,
				XAIE_CORE_MOD_MEM_CNTR_ZEROISATION_MASK);
			RegAddr = _XAie_LGetTileAddr(R, C) +
				XAIE_MEM_MOD_MEM_CNTR_REGOFF;
			_XAie_LPartMaskWrite32(DevInst, RegAddr,
				XAIE_MEM_MOD_MEM_CNTR_ZEROISATION_MASK,
				XAIE_MEM_MOD_MEM_CNTR_ZEROISATION_MASK);
		}
	}

	/* Poll last mem module and last mem tile mem module */
	RegAddr = _XAie_LGetTileAddr(XAIE_NUM_ROWS - 1,
			DevInst->NumCols - 1) +
			XAIE_MEM_MOD_MEM_CNTR_REGOFF;
	_XAie_LPartPoll32(DevInst, RegAddr,
			XAIE_MEM_MOD_MEM_CNTR_ZEROISATION_MASK, 0, 800);

	RegAddr = _XAie_LGetTileAddr(XAIE_AIE_TILE_ROW_START - 1,
			DevInst->NumCols - 1) +
			XAIE_MEM_TILE_MOD_MEM_CNTR_REGOFF;
	_XAie_LPartPoll32(DevInst, RegAddr,
			XAIE_MEM_TILE_MEM_CNTR_ZEROISATION_MASK, 0, 800);

}

static inline void _XAie_LCertMemZeroInit(XAie_DevInst *DevInst)
{
	(void)DevInst;
}

/*****************************************************************************/
/**
*
* This API checks if all the Tile DMA and Mem Tile DMA channels in a partition
* are idle.
*
* @param	DevInst: Device Instance
*
* @return       XAIE_OK if all channels are idle, XAIE_ERR otherwise.
*
* @note		Internal API only. Checks for AIE Tile DMAs and Mem Tile DMAs
*
******************************************************************************/
static inline AieRC _XAie_LPartIsDmaIdle(XAie_DevInst *DevInst)
{
	for (u8 C = 0U; C < DevInst->NumCols; C++) {
		u64 RegAddr;
		u32 RegVal;

		/* AIE TILE DMAs */
		for(u8 R = XAIE_AIE_TILE_ROW_START; R < XAIE_NUM_ROWS; R++) {
			for (u32 Ch = 0; Ch < XAIE_TILE_DMA_NUM_CH; Ch++) {
				/* S2MM Channel */
				RegAddr = _XAie_LGetTileAddr(R, C) + Ch * 4 +
					XAIE_TILE_DMA_S2MM_CHANNEL_STATUS_REGOFF;
				RegVal = _XAie_LPartRead32(DevInst, RegAddr);
				if(RegVal &
				   (XAIE_TILE_DMA_S2MM_CHANNEL_STATUS_MASK |
				    XAIE_TILE_DMA_S2MM_CHANNEL_RUNNING_MASK))
					return XAIE_ERR;

				/* MM2S Channel */
				RegAddr = _XAie_LGetTileAddr(R, C) + Ch * 4 +
					XAIE_TILE_DMA_MM2S_CHANNEL_STATUS_REGOFF;
				RegVal = _XAie_LPartRead32(DevInst, RegAddr);
				if(RegVal &
				   (XAIE_TILE_DMA_MM2S_CHANNEL_STATUS_MASK |
				    XAIE_TILE_DMA_MM2S_CHANNEL_RUNNING_MASK))
					return XAIE_ERR;
			}

		}

		/* MEM TILE DMAs */
		for(u8 R = XAIE_MEM_TILE_ROW_START; R < XAIE_AIE_TILE_ROW_START;
				R++) {
			for(u32 Ch = 0; Ch < XAIE_MEM_TILE_DMA_NUM_CH; Ch++) {
				/* S2MM Channel */
				RegAddr = _XAie_LGetTileAddr(R, C) + Ch * 4 +
					XAIE_MEM_TILE_DMA_S2MM_CHANNEL_STATUS_REGOFF;
				RegVal = _XAie_LPartRead32(DevInst, RegAddr);
				if(RegVal &
				   (XAIE_MEM_TILE_DMA_S2MM_CHANNEL_STATUS_MASK |
				    XAIE_MEM_TILE_DMA_S2MM_CHANNEL_RUNNING_MASK))
					return XAIE_ERR;

				/* MM2S Channel */
				RegAddr = _XAie_LGetTileAddr(R, C) + Ch * 4 +
					XAIE_MEM_TILE_DMA_MM2S_CHANNEL_STATUS_REGOFF;
				RegVal = _XAie_LPartRead32(DevInst, RegAddr);
				if(RegVal &
				   (XAIE_MEM_TILE_DMA_MM2S_CHANNEL_STATUS_MASK |
				    XAIE_MEM_TILE_DMA_MM2S_CHANNEL_RUNNING_MASK))
					return XAIE_ERR;
			}
		}
	}

	return XAIE_OK;
}

/*****************************************************************************/
/**
*
* This API checks if all the DMA channels in a SHIM NOC tile are idle.
*
* @param	DevInst: Device Instance
* @param	Loc: ShimDma location
*
* @return       XAIE_OK if all channels are idle, XAIE_ERR otherwise.
*
* @note		Internal API only. Checks for AIE Tile DMAs and Mem Tile DMAs
*
******************************************************************************/
static inline AieRC _XAie_LIsShimDmaIdle(XAie_DevInst *DevInst,
		XAie_LocType Loc)
{
	u64 RegAddr;
	u32 RegVal;

	for(u32 Ch = 0; Ch < XAIE_SHIM_DMA_NUM_CH; Ch++) {
		/* S2MM Channel */
		RegAddr = _XAie_LGetTileAddr(0, Loc.Col) + Ch * 4 +
			XAIE_SHIM_DMA_S2MM_CHANNEL_STATUS_REGOFF;
		RegVal = _XAie_LPartRead32(DevInst, RegAddr);
		if(RegVal & XAIE_SHIM_DMA_S2MM_CHANNEL_STATUS_MASK)
			return XAIE_ERR;

		/* MM2S Channel */
		RegAddr = _XAie_LGetTileAddr(0, Loc.Col) + Ch * 4 +
			XAIE_SHIM_DMA_MM2S_CHANNEL_STATUS_REGOFF;
		RegVal = _XAie_LPartRead32(DevInst, RegAddr);
		if(RegVal & XAIE_SHIM_DMA_MM2S_CHANNEL_STATUS_MASK)
			return XAIE_ERR;
	}

	return XAIE_OK;
}

static inline int _XAie_LMemBarrier(XAie_DevInst *DevInst, XAie_LocType Loc)
{
	u64 RegAddr;
	u32 Value = 0xBEEF;
	int Ret;

	RegAddr = _XAie_LGetTileAddr(0, Loc.Col) + XAIE_PL_MODULE_SPARE_REG;
	_XAie_LPartWrite32(DevInst, RegAddr, Value);
	Ret = _XAie_LPartPoll32(DevInst, RegAddr, 0xFFFF, Value, 800);
	if (Ret < 0)
		return Ret;

	_XAie_LPartWrite32(DevInst, RegAddr, 0U);
	return 0;
}

static inline AieRC _XAie_LPartDataMemZeroInit(XAie_DevInst *DevInst)
{
	u64 RegAddr;
	int Ret;

	if(DevInst->L2PreserveMem == 0) {
		for(u8 C = 0; C < DevInst->NumCols; C++) {
			for (u8 R = XAIE_MEM_TILE_ROW_START;
					R < XAIE_AIE_TILE_ROW_START; R++) {
				RegAddr = _XAie_LGetTileAddr(R, C) +
					XAIE_MEM_TILE_MOD_MEM_CNTR_REGOFF;
				_XAie_LPartMaskWrite32(DevInst, RegAddr,
						XAIE_MEM_TILE_MEM_CNTR_ZEROISATION_MASK,
						XAIE_MEM_TILE_MEM_CNTR_ZEROISATION_MASK);
			}
		}
	}
	for(u8 C = 0; C < DevInst->NumCols; C++) {
		for (u8 R = XAIE_AIE_TILE_ROW_START;
			R < XAIE_NUM_ROWS; R++) {
			RegAddr = _XAie_LGetTileAddr(R, C) +
				XAIE_MEM_MOD_MEM_CNTR_REGOFF;
			_XAie_LPartMaskWrite32(DevInst, RegAddr,
				XAIE_MEM_MOD_MEM_CNTR_ZEROISATION_MASK,
				XAIE_MEM_MOD_MEM_CNTR_ZEROISATION_MASK);
		}
	}

	for(u8 C = 0; C < DevInst->NumCols; C++) {
		Ret = _XAie_LMemBarrier(DevInst, XAie_TileLoc(C, 0));
		if (Ret < 0)
			return XAIE_ERR;

		/* Poll last mem module and last mem tile mem module */
		RegAddr = _XAie_LGetTileAddr(XAIE_NUM_ROWS - 1, C) +
				XAIE_MEM_MOD_MEM_CNTR_REGOFF;
		Ret = _XAie_LPartPoll32(DevInst, RegAddr,
				XAIE_MEM_MOD_MEM_CNTR_ZEROISATION_MASK, 0, 800);
		if (Ret < 0)
			return XAIE_ERR;
		
		if(DevInst->L2PreserveMem == 0) {
			RegAddr = _XAie_LGetTileAddr(XAIE_AIE_TILE_ROW_START - 1, C) +
				XAIE_MEM_TILE_MOD_MEM_CNTR_REGOFF;
			Ret = _XAie_LPartPoll32(DevInst, RegAddr,
					XAIE_MEM_TILE_MEM_CNTR_ZEROISATION_MASK, 0, 800);
			if (Ret < 0)
				return XAIE_ERR;
		}
	}
	return XAIE_OK;
}

/*****************************************************************************/
/**
*
* This is function to setup the protected register configuration value.
*
* @param	DevInst : AI engine partition device pointer
* @param	Enable: Enable partition
*
* @note		None
*
*******************************************************************************/
static inline void _XAie_LNpiSetPartProtectedReg(XAie_DevInst *DevInst,
		u8 Enable)
{
	u32 StartCol, EndCol, RegVal;

	StartCol = DevInst->StartCol;
	EndCol = DevInst->NumCols + StartCol - 1;

	RegVal = XAie_SetField(Enable, XAIE_NPI_PROT_REG_CNTR_EN_LSB,
			       XAIE_NPI_PROT_REG_CNTR_EN_MSK);

	RegVal |= XAie_SetField(StartCol, XAIE_NPI_PROT_REG_CNTR_FIRSTCOL_LSB,
				XAIE_NPI_PROT_REG_CNTR_FIRSTCOL_MSK);
	RegVal |= XAie_SetField(EndCol, XAIE_NPI_PROT_REG_CNTR_LASTCOL_LSB,
				XAIE_NPI_PROT_REG_CNTR_LASTCOL_MSK);

	_XAie_LNpiSetLock(XAIE_DISABLE);
	_XAie_LNpiWriteCheck32(XAIE_NPI_PROT_REG_CNTR_REG, RegVal);
	_XAie_LNpiSetLock(XAIE_ENABLE);
}

#if ((XAIE_DEV_SINGLE_GEN == XAIE_DEV_GEN_AIE) || \
     (XAIE_DEV_SINGLE_GEN == XAIE_DEV_GEN_AIEML) || \
     (XAIE_DEV_SINGLE_GEN == XAIE_DEV_GEN_AIE2IPU))

/*****************************************************************************/
/**
 *
 * Disable TLAST is not supported on AIEML
 *
 * @param	DevInst: Device Instance
 *
 * @return	None.
 *
 * @note	None.
 *
 *****************************************************************************/
static inline void _XAie_DisableTlast(XAie_DevInst *DevInst)
{
	(void)DevInst;
}
#else
/*****************************************************************************/
/**
 *
 * This API Disable TLAST Error Enable Field in Module Clock Control register.
 * By disabling this bit, control packet can be processed without the need
 * for TLAST to be present after each packet.
 *
 * @param	DevInst: Device Instance
 *
 * @return	None.
 *
 * @note	None.
 *
 *****************************************************************************/
static inline void _XAie_DisableTlast(XAie_DevInst *DevInst)
{
	u8 MemTileStart, MemTileEnd, AieRowStart, AieRowEnd;
	u64 RegAddr;
	u32 Mask;

	MemTileStart = XAIE_MEM_TILE_ROW_START;
	MemTileEnd = XAIE_MEM_TILE_ROW_START + XAIE_MEM_TILE_NUM_ROWS;
	AieRowStart = XAIE_AIE_TILE_ROW_START;
	AieRowEnd = XAIE_AIE_TILE_ROW_START + XAIE_AIE_TILE_NUM_ROWS;

	for(u8 Col = 0; Col < DevInst->NumCols; Col++) {
		for(u8 Row = AieRowStart; Row < AieRowEnd; Row++) {
			RegAddr = XAIE_AIE_TILE_CLOCK_CONTROL_REGOFF + _XAie_LGetTileAddr(Row, Col);
			Mask = XAIE_AIE_TILE_CLOCK_CONTROL_STREAM_SWITCH_MASK;
			_XAie_LPartMaskWrite32(DevInst, RegAddr, Mask, XAIE_DISABLE);
		}
		for(u8 MemRow = MemTileStart; MemRow < MemTileEnd; MemRow++) {
			RegAddr = XAIE_MEM_TILE_CLOCK_CONTROL_REGOFF + _XAie_LGetTileAddr(MemRow, Col);
			Mask = XAIE_MEM_TILE_CLOCK_CONTROL_STREAM_SWITCH_MASK;
			_XAie_LPartMaskWrite32(DevInst, RegAddr, Mask, XAIE_DISABLE);
		}
		/*
		 * Shim tile Clock Control TLAST Error disabled
		 */
		RegAddr = XAIE_SHIM_TILE_MOD_CLOCK_CONTROL_0_REGOFF + _XAie_LGetTileAddr(0, Col);
		Mask = XAIE_SHIM_TILE_MOD_CLOCK_CONTROL_0_STREAM_SWITCH_MASK;
		_XAie_LPartMaskWrite32(DevInst, RegAddr, Mask, XAIE_DISABLE);

	}
}

#endif /* GEN == AIE || AIEML || AIE2IPU */

/*****************************************************************************/
/**
 *
 * This API runs loop through all start and end of core registers
 *
 * @param	DevInst: Device Instance
 *		soff   : Start Address of Core Register
 *		eoff   : End Address of Core Register
 *		Row   :  Row number of Tile
 *		Col   :  Column Number
 *
 * @return	None.
 *
 * @note	None.
 *
 *****************************************************************************/
static inline void _XAie_WriteCoreReg(XAie_DevInst *DevInst, u32 soff, u32 eoff,
				      u8 Row, u8 Col, u8 width)
{
	u32 RegAddr;
	u8 j;
	for(u32 reg = soff; reg <= eoff; reg+= AIE_CORE_REGS_STEP) {
		for(j = 0; j < width; j++) {
			RegAddr = reg + _XAie_LGetTileAddr(Row, Col);
			_XAie_LPartWrite32(DevInst, (RegAddr + j * 4), 0);
		}
	}
}

/*****************************************************************************/
/**
 *
 * This API Clears Core register as a part of Clear context.
 *
 * @param	DevInst: Device Instance
 *
 * @return	None.
 *
 * @note	None.
 *
 *****************************************************************************/
static inline void _XAie_ClearCoreReg(XAie_DevInst *DevInst)
{
	u32 soff, eoff;
	u8 Row, Col;

	for(Row = XAIE_AIE_TILE_ROW_START; Row < DevInst->NumRows; Row++) {
		for(Col = DevInst->StartCol; Col < DevInst->NumCols; Col++) {

                        /* Below registers are 128-bit. So need to write in 4 cycles */
			soff = XAIE_AIE_TILE_CORE_LL_REGOFF;
			eoff = XAIE_AIE_TILE_CORE_HH_REGOFF;
			_XAie_WriteCoreReg(DevInst, soff, eoff, Row, Col, 4);

			soff = XAIE_CORE_MODULE_CORE_WL0_PART1_REGOFF;
			eoff = XAIE_CORE_MODULE_CORE_WH11_PART2_REGOFF;
			_XAie_WriteCoreReg(DevInst, soff, eoff, Row, Col, 4);

#if ((XAIE_DEV_SINGLE_GEN == XAIE_DEV_GEN_AIE2P) || \
        (XAIE_DEV_SINGLE_GEN == XAIE_DEV_GEN_AIE2P_STRIX_B0) || \
        (XAIE_DEV_SINGLE_GEN == XAIE_DEV_GEN_AIE2P_STRIX_A0))

			soff = XAIE_CORE_MODULE_CORE_Q0_REGOFF;
			eoff = XAIE_CORE_MODULE_CORE_LDFIFOH1_PART4_REGOFF;
			_XAie_WriteCoreReg(DevInst, soff, eoff, Row, Col, 4);

			soff = XAIE_CORE_MODULE_CORE_E0_REGOFF;
			eoff = XAIE_CORE_MODULE_CORE_E11_REGOFF;
			/* Below registers are 64-bit. So need to write in 2 cycles */
			_XAie_WriteCoreReg(DevInst, soff, eoff, Row, Col, 2);

#else
                        soff = XAIE_CORE_MODULE_CORE_Q0_REGOFF;
                        eoff = XAIE_CORE_MODULE_CORE_Q3_REGOFF;
                        /* Below registers are 128-bit. So need to write in 4 cycles */
                        _XAie_WriteCoreReg(DevInst, soff, eoff, Row, Col, 4);

#endif
                        soff = XAIE_CORE_MODULE_CORE_R0_REGOFF;
                        eoff = XAIE_CORE_MODULE_CORE_S3_REGOFF;
                        /* Below registers are 32-bit. So need to write in 1 cycles */
                        _XAie_WriteCoreReg(DevInst, soff, eoff, Row, Col, 1);
	
                        soff = XAIE_CORE_MODULE_CORE_PC_START_REGOFF;
                        eoff = XAIE_CORE_MODULE_CORE_PC_END_REGOFF;
                        /* Below registers are 32-bit. So need to write in 1 cycles */
                        _XAie_WriteCoreReg(DevInst, soff, eoff, Row, Col, 1);

		}
	}
}

#endif		/* end of protection macro */
/** @} */
