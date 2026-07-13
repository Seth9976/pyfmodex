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
import os
import platform
import sys
import re
from common.constant import PLUGIN_NAME, PROJECT_ROOT
from common.platform import *
from common.registry import platform_registry, get_supported_platforms
from common.util import exit_with_error

SUPPORTED_PLATFORMS = get_supported_platforms("build", platform.system())

def support_legacy_arch_nomenclature(args):
    # Deprecated: Support for legacy arch nomenclature. Use -x and -t flags instead.
    if args.arch and "_vc" in args.arch:
        print("Deprecated: legacy arch nomenclature. Use -x and -t flags instead.")
        args.toolset = re.search(r"vc.+", args.arch).group(0)
        args.arch = re.search(r".+(?=(_))", args.arch).group(0)

def is_arch_valid(arch, platform_info):
    if arch not in platform_info.build.archs:
        print(f"error: Invalid architecture '{arch}' for target {platform_info.name}.")
        print(f"supported architectures: {', '.join(platform_info.build.archs)}")
        return False
    return True

def is_toolset_valid(toolset, platform_info):
    if toolset not in platform_info.build.toolsets:
        print(f"error: Invalid toolset '{toolset}' for target {platform_info.name}.")
        print(f"supported toolsets: {', '.join(platform_info.build.toolsets)}")
        return False
    return True

def is_configuration_valid(configuration, platform_info):
    if configuration not in platform_info.build.configurations:
        print(f"error: Invalid configuration '{configuration}' for target {platform_info.name}.")
        print(f"supported configurations: {', '.join(platform_info.build.configurations)}")
        return False
    return True

def filter_platform_info_from_args(platform_info, args):
    if args.arch:
        # build for the requested architecture instead of building them all
        platform_info.build.archs = (args.arch,)

    if args.toolset:
        # build for the requested toolset instead of building them all
        platform_info.build.toolsets = (args.toolset,)

def run(argv):
    # parse the command line
    parser = argparse.ArgumentParser(description="Audiokinetic Inc. build tool for plugins")
    parser.add_argument("platform", metavar="platform", choices=SUPPORTED_PLATFORMS, help=f"platform to build ({', '.join(SUPPORTED_PLATFORMS)})")
    parser.add_argument("-c", "--configuration", help="configuration to build (Debug, Release, Profile, ...).")
    parser.add_argument("-x", "--arch", help="architecture to build (x32, x64, ...).")
    parser.add_argument("-t", "--toolset", help="toolset used to build on Windows platforms (vc160, vc170).")
    parser.add_argument("-f", "--build-hooks-file", help="path to a Python file defining one or more of the supported hooks (postbuild) to be called at various step during the build process")
    parser.add_argument("--toolchain-vers", help="Path to a \'ToolchainVers\' text file, containing a list of supported toolchain versions to pass to the platform's toolchain_setup script, to setup and define a set of env-vars to re-run each build step with.")
    parser.add_argument("--toolchain-env-script", help="Path to a \'GetToolchainEnv\' script, which, when executed with a version provided by the toolchain-vers file, returns a comma separated list of environment variables to apply for a build step.")
    args = parser.parse_args(argv)

    # import the build hooks
    build_hooks = None
    if args.build_hooks_file:
        sys.path.append(os.path.join(PROJECT_ROOT))
        if not os.path.exists(args.build_hooks_file):
            exit_with_error(f"Missing build hooks file {args.build_hooks_file} at {PROJECT_ROOT}")
        try:
            build_hooks = __import__(os.path.splitext(args.build_hooks_file)[0])
        except Exception as e:
            exit_with_error(f"Invalid build hooks file {args.build_hooks_file} at {PROJECT_ROOT}\n{str(e)}")

    # handle platform-specific command line specificites
    platform_info = platform_registry.get(args.platform)

    support_legacy_arch_nomenclature(args)

    if not args.arch:
        if platform_info.build.require_arch:
            print(f"error: 'Arch' argument for target {platform_info.name} needs to be specified using -x or --arch, -h for more details.")
            print(f"supported architectures: {', '.join(platform_info.build.archs)}")
            return 1
    elif not is_arch_valid(args.arch, platform_info):
        return 1

    if not args.toolset:
        if platform_info.build.require_toolset:
            print(f"error: 'Toolset' argument for target {platform_info.name} needs to be specified using -t or --toolset, -h for more details.")
            print(f"supported toolsets: {', '.join(platform_info.build.toolsets)}")
            return 1
    elif not is_toolset_valid(args.toolset, platform_info):
        return 1

    if not args.configuration:
        if platform_info.build.require_configuration:
            exit_with_error(f"'Configuration' argument for target {platform_info.name} needs to be specified using -c or --configuration, -h for more details.")

        print(f"Building {PLUGIN_NAME} for {platform_info.name}.")
    else:
        if not is_configuration_valid(args.configuration, platform_info):
            return 1

        print(f"Building {PLUGIN_NAME} for {platform_info.name} in {args.configuration}...")

    filter_platform_info_from_args(platform_info, args)

    if args.toolchain_vers:
        # specify an override location for the toolchain vers file
        platform_info.build.toolchain_vers = args.toolchain_vers

    if args.toolchain_env_script:
        # specify an override location for the toolchain env script
        platform_info.build.toolchain_env_script = args.toolchain_env_script

    # run the build
    return platform_info.build.command(platform_info.name, args.configuration, build_hooks)

if __name__ == "__main__":
    sys.exit(run(sys.argv[1:]))
