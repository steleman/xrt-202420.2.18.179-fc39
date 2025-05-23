/******************************************************************************
* Copyright (C) 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*****************************************************************************/
/**
* @file xaie_io.c
* @{
*
* This file contains the data structures and routines for low level IO
* operations.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------  -------- -----------------------------------------------------
* 1.0   Tejus   06/09/2020 Initial creation.
* 1.1   Tejus   06/10/2020 Add ess simulation backend.
* 1.2   Tejus   06/10/2020 Add cdo backend.
* 1.3   Tejus   06/10/2020 Add helper function to get backend pointer.
* </pre>
*
******************************************************************************/
/***************************** Include Files *********************************/
#include "xaie_feature_config.h"
#include "xaie_helper.h"
#include "xaie_io.h"
#include "xaie_io_internal.h"

/************************** Constant Definitions *****************************/
#if defined (__AIESIM__)
	#define XAIE_DEFAULT_BACKEND XAIE_IO_BACKEND_SIM
#elif defined (__AIECDO__)
	#define XAIE_DEFAULT_BACKEND XAIE_IO_BACKEND_CDO
#elif defined (__AIECONTROLCODE__)
	#define XAIE_DEFAULT_BACKEND XAIE_IO_BACKEND_CONTROLCODE
#elif defined (__AIEBAREMETAL__)
	#define XAIE_DEFAULT_BACKEND XAIE_IO_BACKEND_BAREMETAL
#elif defined (__AIEIPU__)
	#define XAIE_DEFAULT_BACKEND XAIE_IO_BACKEND_IPU
#elif defined (__AIESOCKET__)
	#define XAIE_DEFAULT_BACKEND XAIE_IO_BACKEND_SOCKET
#else
	#define __AIEDEBUG__
	#define XAIE_DEFAULT_BACKEND XAIE_IO_BACKEND_DEBUG
#endif

#if defined (__AIESIM__) || defined (__AIEDEBUG__)
	#define SIMBACKEND &SimBackend
#else
	#define SIMBACKEND NULL
#endif
#if defined (__AIECDO__)
	#define CDOBACKEND &CdoBackend
#else
	#define CDOBACKEND NULL
#endif
#if defined (__AIECONTROLCODE__)
	#define CONTROLCODEBACKEND &ControlCodeBackend
#else
	#define CONTROLCODEBACKEND NULL
#endif
#if defined (__AIEBAREMETAL__)
	#define BAREMETALBACKEND &BaremetalBackend
#else
	#define BAREMETALBACKEND NULL
#endif
#if defined (__AIEIPU__)
	#define IPUBACKEND &IpuBackend
#else
	#define IPUBACKEND NULL
#endif
#if defined (__AIESOCKET__)
	#define SOCKETBACKEND &SocketBackend
#else
	#define SOCKETBACKEND NULL
#endif
#if defined (__AIEDEBUG__)
	#define DEBUGBACKEND &DebugBackend
#else
	#define DEBUGBACKEND NULL
#endif

/************************** Variable Definitions *****************************/
extern const XAie_Backend SimBackend;
extern const XAie_Backend CdoBackend;
extern const XAie_Backend BaremetalBackend;
extern const XAie_Backend DebugBackend;
extern const XAie_Backend IpuBackend;
extern const XAie_Backend SocketBackend;
extern const XAie_Backend ControlCodeBackend;

static const XAie_Backend *IOBackend[XAIE_IO_BACKEND_MAX] =
{
	SIMBACKEND,
	CDOBACKEND,
	BAREMETALBACKEND,
	DEBUGBACKEND,
	IPUBACKEND,
	SOCKETBACKEND,
	CONTROLCODEBACKEND,
};

/************************** Function Definitions *****************************/
/*****************************************************************************/
/**
*
* This is the api initialize global IO instance. The default IO backend is
* debug backend.
*
* @param	DevInst - Device instance pointer.
*
* @return	XAIE_OK on success and error code on failure.
*
* @note		Internal Only.
*
******************************************************************************/
AieRC XAie_IOInit(XAie_DevInst *DevInst)
{
	AieRC RC;
	const XAie_Backend *Backend = IOBackend[XAIE_DEFAULT_BACKEND];

	RC = Backend->Ops.Init(DevInst);
	if(RC != XAIE_OK) {
		return RC;
	}

	DevInst->Backend = Backend;

	XAIE_DBG("Initialized with backend %d\n", Backend->Type);

	return XAIE_OK;
}

/*****************************************************************************/
/**
*
* This is the api to get the backend pointer for a given Backend
*
* @param	Backend - Backend type.
*
* @return	Backend pointer.
*
* @note		Internal Only.
*
******************************************************************************/
const XAie_Backend* _XAie_GetBackendPtr(XAie_BackendType Backend)
{
	return IOBackend[Backend];
}

/** @} */
