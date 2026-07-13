#!/usr/bin/env python3

"""
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2026 Audiokinetic Inc.
"""

import argparse
import platform
import sys
from common.constant import PLUGIN_NAME
from common.platform import *
from common.registry import platform_registry, get_supported_platforms
from build import support_legacy_arch_nomenclature, is_arch_valid, is_toolset_valid, is_configuration_valid, filter_platform_info_from_args

SUPPORTED_PLATFORMS = get_supported_platforms("test", platform.system())

def run(argv):
    # parse the command line
    parser = argparse.ArgumentParser(description="Audiokinetic Inc. test tool for plugins")
    parser.add_argument("platform", metavar="platform", choices=SUPPORTED_PLATFORMS, help=f"platform to test ({', '.join(SUPPORTED_PLATFORMS)})")
    parser.add_argument("-c", "--configuration", help="configuration to test (Debug, Release, Profile, ...).")
    parser.add_argument("-x", "--arch", help="architecture to test (x32, x64, ...).")
    parser.add_argument("-t", "--toolset", help="toolset used to test on Windows platforms (vc150, vc160, vc170).")
    parser.add_argument("--toolchain-vers", help="Path to a \'ToolchainVers\' text file, containing a list of supported toolchain versions to pass to the platform's toolchain_setup script, to setup and define a set of env-vars to re-run each build step with.")

    args = parser.parse_args(argv)

    # handle platform-specific command line specificites
    platform_info = platform_registry.get(args.platform)

    support_legacy_arch_nomenclature(args)

    if args.arch != None and not is_arch_valid(args.arch, platform_info):
        return 1

    if args.toolset != None and not is_toolset_valid(args.toolset, platform_info):
        return 1

    if not args.configuration:
        print(f"Running {PLUGIN_NAME} tests for {platform_info.name}.")
    else:
        if not is_configuration_valid(args.configuration, platform_info):
            return 1

        print(f"Running {PLUGIN_NAME} tests for {platform_info.name} in {args.configuration}...")

    filter_platform_info_from_args(platform_info, args)

    if args.configuration:
        # test for the requested configuration instead of testing them all
        platform_info.build.configurations = (args.configuration,)

    if args.toolchain_vers:
        # specify an override location for the toolchain vers file
        platform_info.build.toolchain_vers = args.toolchain_vers

    # run the test
    return platform_info.test.command(platform_info.name)

if __name__ == "__main__":
    sys.exit(run(sys.argv[1:]))
