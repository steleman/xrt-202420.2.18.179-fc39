// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2019-2022 Xilinx, Inc
// Copyright (C) 2022-2024 Advanced Micro Devices, Inc. All rights reserved.

// ------ I N C L U D E   F I L E S -------------------------------------------
// Local - Include Files
#include "core/common/system.h"
#include "SubCmd.h"
#include "XBHelpMenusCore.h"
#include "XBUtilitiesCore.h"
#include "XBHelpMenus.h"
#include "XBMain.h"
#include "XBUtilities.h"
namespace XBU = XBUtilities;

// 3rd Party Library - Include Files
#include <boost/format.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

// System - Include Files
#include <filesystem>
#include <iostream>

// ------ Program entry point -------------------------------------------------
void  main_(int argc, char** argv, 
            const std::string & _executable,
            const std::string & _description,
            const SubCmdsCollection &_subCmds,
            const boost::property_tree::ptree& configurations) 
{
  bool isUserDomain = boost::iequals(_executable, "xrt-smi"); 

  // Global options
  bool bVerbose = false;
  bool bTrace = false;
  bool bHelp = false;
  bool bBatchMode = false;
  bool bShowHidden = false;
  bool bForce = false;
  bool bVersion = false;
  std::string sDevice;
  std::string sCmd;

  // Build Options
  po::options_description globalSubCmdOptions("Global Command Options");
  globalSubCmdOptions.add_options()
    ("verbose", boost::program_options::bool_switch(&bVerbose), "Turn on verbosity")
    ("batch",   boost::program_options::bool_switch(&bBatchMode), "Enable batch mode (disables escape characters)")
    ("force",   boost::program_options::bool_switch(&bForce), "When possible, force an operation")
  ;

  po::options_description globalOptions("Global Options");
  globalOptions.add_options()
    ("help",    boost::program_options::bool_switch(&bHelp), "Help to use this application")
    ("version", boost::program_options::bool_switch(&bVersion), "Report the version of XRT and its drivers")
  ;
  globalOptions.add(globalSubCmdOptions);

  // Hidden Options
  const std::string device_default = xrt_core::get_total_devices(false).first == 1 ? "default" : "";
  po::options_description hiddenOptions("Hidden Options");
  hiddenOptions.add_options()
    ("device,d",    boost::program_options::value<decltype(sDevice)>(&sDevice)->default_value(device_default)->implicit_value("default"), "If specified with no BDF value and there is only 1 device, that device will be automatically selected.\n")
    ("trace",       boost::program_options::bool_switch(&bTrace), "Enables code flow tracing")
    ("show-hidden", boost::program_options::bool_switch(&bShowHidden), "Shows hidden options and commands")
    ("subCmd",      po::value<decltype(sCmd)>(&sCmd), "Command to execute")
  ;

  // Merge the options to one common collection
  po::options_description allOptions("All Options");
  allOptions.add(globalOptions).add(hiddenOptions);

  // Create a sub-option command and arguments
  po::positional_options_description positionalCommand;
  positionalCommand.add("subCmd", 1 /* max_count */);

  // Parse the command line arguments
  po::variables_map vm;
  po::command_line_parser parser(argc, argv);
  SubCmd::SubCmdOptions subcmd_options;
  try {
    subcmd_options = XBU::process_arguments(vm, parser, allOptions, positionalCommand, false);
  } catch (po::error& ex) {
    std::cerr << ex.what() << std::endl;
  }

  if(bVersion) {
    std::cout << XBU::get_xrt_pretty_version();
    return;
  }

  // Check that the versions of XRT for build and tool match
  XBU::xrt_version_cmp(isUserDomain);
 
  // -- Enable/Disable helper "global" options
  XBU::disable_escape_codes( bBatchMode );
  XBU::setVerbose( bVerbose );
  XBU::setTrace( bTrace );
  XBU::setShowHidden( bShowHidden );
  XBU::setForce( bForce );

  // Was default device requested?
  if (boost::iequals(sDevice, "default")) {
    sDevice.clear();
    boost::property_tree::ptree available_devices = XBU::get_available_devices(isUserDomain);

    // DRC: Are there any devices
    if (available_devices.empty()) 
      throw std::runtime_error("No devices found.");

    // DRC: Are there multiple devices, if so then no default device can be found.
    if (available_devices.size() > 1) {
      std::cerr << "\nERROR: Multiple devices found. Please specify a single device using the --device option\n\n";
      std::cerr << XBUtilities::str_available_devs(isUserDomain) << std::endl;

      std::cout << std::endl;
      throw xrt_core::error(std::errc::operation_canceled);
    }

    // We have only 1 item in the array, get it
    for (const auto &kd : available_devices) 
      sDevice = kd.second.get<std::string>("bdf"); // Exit after the first item

  }

  // If there is a device value, parse for valid subcommands for this device.
  SubCmdsCollection devSubCmds;
  if (!sDevice.empty()) {
    const std::string deviceClass = XBU::get_device_class(sDevice, isUserDomain);
    const auto& configs = JSONConfigurable::parse_configuration_tree(configurations);
    for (auto & subCmdEntry : _subCmds) {
      auto it = configs.find(subCmdEntry->getName());
      for (const auto& [device_name, device_config] : it->second) {
        if (device_name.compare(deviceClass) == 0)
          devSubCmds.emplace_back(subCmdEntry);
      }
    }
  }

  const SubCmdsCollection parsedSubCmds = sDevice.empty() ? _subCmds : devSubCmds;

  // Search for the subcommand (case sensitive)
  std::shared_ptr<SubCmd> subCommand;
  for (auto & subCmdEntry : parsedSubCmds) {
    if (sCmd.compare(subCmdEntry->getName()) == 0) {
      subCommand = subCmdEntry;
      break;
    }
  }

  if (!subCommand) {
    if (!bHelp && !sCmd.empty())
      std::cerr << "ERROR: " << "Unknown command: '" << sCmd << "'" << std::endl;
    XBU::report_commands_help( _executable, _description, globalOptions, hiddenOptions, parsedSubCmds);
    if (!bHelp && !sCmd.empty())
      throw xrt_core::error(std::errc::operation_canceled);
    return;
  }

  // -- Prepare the data
  subcmd_options.erase(subcmd_options.begin());

  if (bHelp)
    subcmd_options.push_back("--help");

  // If there is a device value, pass it to the sub commands.
  if (!sDevice.empty()) {
    subcmd_options.push_back("-d");
    subcmd_options.push_back(sDevice);
  }

  subCommand->setGlobalOptions(globalSubCmdOptions);

  // -- Execute the sub-command
  subCommand->execute(subcmd_options);
}



