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
import re
import subprocess
import sys
from common.constant import PLUGIN_NAME, PREMAKE, PROJECT_ROOT, WWISE_ROOT
from common.platform import *
from common.registry import platform_registry, get_supported_platforms, is_authoring_target

SUPPORTED_PLATFORMS = get_supported_platforms("premake")

def run(argv):
    # parse the command line
    parser = argparse.ArgumentParser(description="Audiokinetic Inc. premake tool for plugins")
    parser.add_argument("platform", metavar="platform", choices=SUPPORTED_PLATFORMS, help="platform to premake ({})".format(", ".join(SUPPORTED_PLATFORMS)))
    parser.add_argument("-t", "--toolset", help="toolset used to build on Windows platforms (vc160, vc170).")
    parser.add_argument("--debugger", help="Enable lua debugger for premake scripts", action="store_true")
    parser.add_argument("--disable-codesign", help="Disable codesign post-build steps", action="store_true")
    parser.add_argument("--signtool-path", help="Path to the SignTool.exe executable to use on Windows")
    args = parser.parse_args(argv)

    # prepare premake parameters
    platform_info = platform_registry.get(args.platform)
    pm_actions = platform_info.premake.actions

    pm_scripts = "--scripts={};{};{}".format(
        os.path.join(WWISE_ROOT, "SDK", "source", "Build"),
        os.path.join(WWISE_ROOT, "Scripts", "Premake"),
        os.path.join(WWISE_ROOT, "Scripts", "Build"))

    pm_file = "--file={}".format(
        os.path.join(WWISE_ROOT, "Scripts", "Build", "Plugins", "premakePlugins.lua"))

    # Default to PlatformInfo name if not specified
    pm_platform = "--os={}".format(
        platform_info.premake.platform or
        re.search(r"\w+(?=(_))|\w+", platform_info.name).group(0).lower()
    )

    pm_plugin_dir = "--plugindir={}".format(PROJECT_ROOT)

    pm_is_authoring = "--authoring={}".format("yes" if is_authoring_target(platform_info.name) else "no")

    pm_codesign = "--codesign={}".format("no" if args.disable_codesign else "yes")

    pm_signtool_path = "--signtool-path=\"{}\"".format(args.signtool_path) if args.signtool_path else ""

    if args.toolset:
        if args.toolset not in platform_info.build.toolsets:
            print("error: Invalid toolset '{}' for target {}.".format(args.toolset, platform_info.name))
            print("supported toolsets: {}".format(', '.join(platform_info.build.toolsets)))
            return 1
        # Assume the toolset is indexed with the premake actions
        pm_actions = [platform_info.premake.actions[platform_info.build.toolsets.index(args.toolset)]]

    # run premake
    for pm_action in pm_actions:
        cmd = [PREMAKE, pm_scripts, pm_file, pm_action, pm_platform, pm_plugin_dir, pm_is_authoring, pm_codesign]
        if pm_signtool_path:
            cmd.append(pm_signtool_path)

        if args.debugger:
            cmd.append("--debugger")

        print("Premake Command: " + str(cmd))
        res = subprocess.Popen(cmd).wait()
        if res != 0:
            sys.exit(res)

if __name__ == "__main__":
    run(sys.argv[1:])
