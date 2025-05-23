// Copyright(C) 2020 - 2021 by Xilinx, Inc. All rights reserved.
// SPDX-License-Identifier: MIT
/**
 * @param file xaiefal-rsc-base.hpp
 * Base classes for AI engine resources management
 */

#include <vector>
#include <xaiengine.h>
#include <xaiefal/rsc/xaiefal-rsc-group.hpp>
#include <xaiefal/common/xaiefal-base.hpp>

#pragma once

namespace xaiefal {
	/**
	 * @class XAieRscGroupRuntime
	 * @brief class to runtime resources functional group
	 * Each element in the group is a resource.
	 */
	class XAieRscGroupRuntime : public XAieRscGroupBase {
	public:
		XAieRscGroupRuntime(std::shared_ptr<XAieDevHandle> DevHd,
				const std::string &Name = ""):
			XAieRscGroupBase(DevHd, Name) {}
		XAieRscGroupRuntime() {}
		~XAieRscGroupRuntime() {}

		/**
		 * This function adds a resource to the resource group.
		 *
		 * @param  AI engine resource shared pointer
		 * @return XAIE_OK for success, error code for failure.
		 */
		AieRC addRsc(std::shared_ptr<XAieRsc> Rsc) {
			bool toAdd = true;
			std::vector<std::weak_ptr<XAieRsc>>::iterator it;

			for (it = vRefs.begin(); it != vRefs.end();) {
				auto lR = it->lock();

				if (lR == nullptr) {
					vRefs.erase(it);
					continue;
				}
				it++;
				if (lR == Rsc) {
					toAdd = false;
					break;
				}
			}
			if (toAdd) {
				vRefs.push_back(Rsc);
			}
			return XAIE_OK;
		}
	private:
		std::vector<std::weak_ptr<XAieRsc>> vRefs; /**< vector of AI engine resource objects */

		XAieRscStat _getRscStat(const std::vector<XAie_LocType> &vLocs,
				XAie_ModuleType Mod, XAieRscType RscType,
				uint32_t RscId) const {
			XAieRscStat RscStat(FuncName);
			std::vector<XAieUserRsc> vRscs;

			for (auto Ref: vRefs) {
				auto Rsc = Ref.lock();

				if (Rsc == nullptr) {
					continue;
				}

				auto RscWrapper = std::make_shared<
					XAieRscGetRscsWrapper>(Rsc, vRscs);
			}

			for (auto Rsc: vRscs) {
				for (auto L: vLocs) {
					if ((L.Col != XAIE_LOC_ANY && Rsc.Loc.Col != L.Col) ||
						(L.Row != XAIE_LOC_ANY && Rsc.Loc.Row != L.Row) ||
						(Mod != static_cast<XAie_ModuleType>(XAIE_MOD_ANY)
						 && Rsc.Mod != Mod) ||
						(RscType != static_cast<XAieRscType>(XAIE_RSC_TYPE_ANY)
						 && Rsc.RscType != RscType) ||
						(RscId != XAIE_RSC_ID_ANY && Rsc.RscId != RscId)) {
						continue;
					}
					RscStat.addRscStat(Rsc.Loc, Rsc.Mod, Rsc.RscType, 1);
				}
			}

			return RscStat;
		}
	};

	/**
	 * @class XAieRscGroupStatic
	 * @brief class to statically allocated resources group
	 * Each element in the group is a resource.
	 */
	class XAieRscGroupStatic : public XAieRscGroupBase {
	public:
		XAieRscGroupStatic(std::shared_ptr<XAieDevHandle> DevHd,
				const std::string &Name = "Static"):
			XAieRscGroupBase(DevHd, Name) {}
		XAieRscGroupStatic() {};
		~XAieRscGroupStatic() {}
	private:
		XAieRscStat _getRscStat(const std::vector<XAie_LocType> &vLocs,
				XAie_ModuleType Mod, XAieRscType RscType,
				uint32_t RscId) const {
			XAieRscStat RscStat(FuncName);
			std::vector<XAieUserRscStat> RscStats;

			(void)RscId;

			if (RscType == XAIE_TRACEEVENT) {
				// Trace events are only allocated at runtime by AIEFAL
				return RscStat;
			}

			auto AieHdPtr = AieHd.lock();
			if (AieHdPtr == nullptr) {
				throw std::invalid_argument("failed to get Aie ptr to get rsc stat for " +
						FuncName);
			}

			if (vLocs[0].Col == XAIE_LOC_ANY) {
				// Any tiles in the AI engine partition
				auto lvLocs = _getAllTilesLocs(AieHdPtr->dev());
				RscStats = _createDrvRscStats(AieHdPtr->dev(),
						lvLocs, Mod, RscType);
			} else {
				RscStats = _createDrvRscStats(AieHdPtr->dev(),
						vLocs, Mod, RscType);
			}

			if (AieHdPtr->rscMgr()->getStaticRscs(RscStats) != XAIE_OK) {
				Logger::log(LogLevel::FAL_ERROR) << "failed to get static resource stat." << std::endl;
			} else {
				for (auto S: RscStats) {
					if (S.NumRscs != 0) {
						RscStat.addRscStat(S.Loc, S.Mod, S.RscType, S.NumRscs);
					}
				}
			}

			return RscStat;
		}
	};

	/**
	 * @class XAieRscGroupAvail
	 * @brief class to runtime resources functional group
	 * Each element in the group is a resource.
	 */
	class XAieRscGroupAvail : public XAieRscGroupBase {
	public:
		XAieRscGroupAvail(std::shared_ptr<XAieDevHandle> DevHd,
				const std::string &Name = "Avail"):
			XAieRscGroupBase(DevHd, Name) {}
		XAieRscGroupAvail() {}

		/**
		 * This function adds a resource to the resource group.
		 *
		 * @param  AI engine resource shared pointer
		 * @return XAIE_OK for success, error code for failure.
		 *
		 * Only the resources availability should should be managed by
		 * AIEFAL should be added to this group. Only trace controller
		 * should be added to this groups, for other resources, it will
		 * go to the lower level driver to get the resource
		 * availability.
		 */
		AieRC addRsc(std::shared_ptr<XAieRsc> Rsc) {
			bool toAdd = true;
			std::vector<std::weak_ptr<XAieRsc>>::iterator it;

			if (Rsc->getRscType() != XAIE_TRACECTRL) {
				throw std::invalid_argument(
					"failed to add rsc to avail group, only trace controller is allowed");
			}
			for (it = vRefs.begin(); it != vRefs.end();) {
				auto lR = it->lock();

				if (lR == nullptr) {
					vRefs.erase(it);
					continue;
				}
				it++;
				if (lR == Rsc) {
					toAdd = false;
					break;
				}
			}
			if (toAdd) {
				vRefs.push_back(Rsc);
			}
			return XAIE_OK;
		}
	private:
		std::vector<std::weak_ptr<XAieRsc>> vRefs; /**< vector of AI engine resource objects which
								manage the resource availability in AIEFAL
							    */
	private:
		XAieRscStat _getRscStat(const std::vector<XAie_LocType> &vLocs,
				XAie_ModuleType Mod, XAieRscType RscType,
				uint32_t RscId) const {
			std::vector<XAieUserRscStat> RscStats;
			XAieRscStat RscStat(FuncName);
			XAieRscType lRscType = RscType;

			(void)RscId;
			auto AieHdPtr = AieHd.lock();
			if (AieHdPtr == nullptr) {
				throw std::invalid_argument("failed to get Aie ptr to get rsc stat for " +
						FuncName);
			}

			if (RscType == XAIE_TRACEEVENT) {
				// If user wants to know the trace events availability,
				// we also needs to know the trace control availability as
				// the trace control is managed by the lower level driver
				// while the trace events avaialability is managed by
				// the AIEFAL.
				lRscType = XAIE_TRACECTRL;
			}

			if (vLocs[0].Col == XAIE_LOC_ANY) {
				// Any tiles in the AI engine partition
				auto lvLocs = _getAllTilesLocs(AieHdPtr->dev());
				RscStats = _createDrvRscStats(AieHdPtr->dev(),
						lvLocs, Mod, lRscType);
			} else {
				RscStats = _createDrvRscStats(AieHdPtr->dev(),
						vLocs, Mod, lRscType);
			}

			if (AieHdPtr->rscMgr()->getAvailRscs(RscStats) != XAIE_OK) {
				Logger::log(LogLevel::FAL_ERROR) << "failed to get avail resource stat." << std::endl;
			} else {
				for (auto S: RscStats) {
					if (S.NumRscs == 0) {
						continue;
					}
					RscStat.addRscStat(S.Loc, S.Mod, S.RscType, S.NumRscs);
				}
			}

			/* Handle resoure whose availability is managed by AIE FAL layer */
			for (auto Ref: vRefs) {
				uint32_t NumRscs;
				XAie_LocType Loc;
				XAie_ModuleType Mod;
				XAieRscType ManagedRscType;

				auto Rsc = Ref.lock();
				if (Rsc == nullptr) {
					continue;
				}

				ManagedRscType = Rsc->getManagedRscsType();
				if (RscType != XAIE_RSC_TYPE_ANY &&
					ManagedRscType != RscType) {
					continue;
				}
				NumRscs = Rsc->getAvailManagedRscs();
				if (NumRscs == 0) {
					continue;
				}

				Loc = std::static_pointer_cast<XAieSingleTileRsc>(Rsc)->loc();
				Mod = std::static_pointer_cast<XAieSingleTileRsc>(Rsc)->mod();
				if (RscStat.getNumRsc(Loc, Mod, Rsc->getRscType()) == 0 &&
					Rsc->isReserved() == false) {
					// If the parent resource is occupied
					// but not by the current app, number of
					// AIEFAL managed child resources is not
					// available.
					continue;
				}
				RscStat.addRscStat(std::static_pointer_cast<XAieSingleTileRsc>(Rsc)->loc(),
						std::static_pointer_cast<XAieSingleTileRsc>(Rsc)->mod(),
						ManagedRscType, NumRscs);
			}
			return RscStat;
		}
	};
}
