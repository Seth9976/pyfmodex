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

from __future__ import absolute_import
import os
import subprocess
import sys
from common.constant import PLUGIN_NAME
from common.hook import invoke, POSTBUILD_HOOK
from common.registry import platform_registry

SUPPORTED_BUILD_SYSTEMS = ("Darwin",)

def build(target, config, hooks):

    platform_info = platform_registry.get(target)

    xcode_argsets = {
        "iOS": ("-sdk", "iphoneos"),
        "iOSsim": ("-sdk", "iphonesimulator"),
        "tvOS": ("-sdk", "appletvos"),
        "tvOSsim": ("-sdk", "appletvsimulator"),
        "visionOS": ("-sdk", "xros"),
        "visionOSsim": ("-sdk", "xrsimulator"),
        "Mac": (),
    }

    xcode_arch_args = {
        "arm64": ("ARCHS=arm64", "VALID_ARCHS=arm64" ),
        "x86_64": ("ARCHS=x86_64", "VALID_ARCHS=x86_64"),
        "universal": ("ARCHS=arm64 x86_64", "VALID_ARCHS=arm64 x86_64")
    }

    # (Build target, architecture args)
    build_targets = [(target, None)]
    target_name = target

    # fetch the list of toolchainvers, and add additional environments to set
    if platform_info.build.toolchain_env_script is not None and platform_info.build.toolchain_vers is not None:
        print("Loading toolchain vers file from ", platform_info.build.toolchain_vers)
        with open(platform_info.build.toolchain_vers, 'r') as toolchain_vers_file:
            toolchain_vers_list = toolchain_vers_file.readlines()

        toolchain_envs = list()
        print("Fetching toolchain environment from ", platform_info.build.toolchain_env_script)
        for toolchain_ver in toolchain_vers_list:
            toolchain_ver = toolchain_ver.strip()
            if (toolchain_ver.startswith("#")):
                continue
            toolchain_env = subprocess.check_output([sys.executable, platform_info.build.toolchain_env_script, toolchain_ver], text=True)
            toolchain_env_ex = os.path.expandvars(toolchain_env)
            toolchain_envs.append(toolchain_env_ex)
    elif (platform_info.build.toolchain_env_script is not None) ^ (platform_info.build.toolchain_vers is not None):
        print("WARNING: Only one of toolchain_env_script or toolchain_vers was defined. Both must be defined to fetch requisite toolchain definitions.")
        print("toolchain_env_script: ", str(platform_info.build.toolchain_env_script))
        print("toolchain_vers: ", str(platform_info.build.toolchain_vers))
        toolchain_envs = [""]
    else: # neither the toolchain_env_script nor toolchain_vers is defined
        toolchain_envs = [""]

    if target == "Mac":
        def is_universal_arch():
            # Defining all known architectures is considered "universal"
            return all(bool(arch in platform_info.build.archs) for arch in ("arm64", "x86_64"))

        if is_universal_arch():
            build_targets = [(target, ("-destination", "generic/platform=macOS" , *xcode_arch_args["universal"]))]
        else:
            build_targets = [(target, xcode_arch_args[platform_info.build.archs[0]])]
    elif target in ["iOS", "tvOS", "visionOS"]:
        simulator_target_universal = (target + "sim", xcode_arch_args["universal"])
        if len(platform_info.build.archs) > 1:
            # Multiple architectures: default set includes target + sim
            build_targets.append(simulator_target_universal)
        elif platform_info.build.archs[0] == target + "sim":
            # Specified *sim architecture specifically: build only the simulator
            build_targets = [simulator_target_universal]
        else:
            # Specified the device architecture target, resolve the architecture for simulator
            # Format: [iOS|tvOS|visionOS]_on[Intel|Arm]
            target_parts = platform_info.build.archs[0].split("_")
            target_name = target_parts[0] 
            target_simulator_name = target_parts[0] + "sim"
            if len(target_parts) == 1:
                # Nothing specified, use universal
                build_targets.append((target_simulator_name, xcode_arch_args["universal"]))
            else:
                # Host Platform specified, use the corresponding architecture arguments
                if target_parts[1] == "onIntel":
                    build_targets.append((target_simulator_name, xcode_arch_args["x86_64"]))
                elif target_parts[1] == "onArm":
                    build_targets.append((target_simulator_name, xcode_arch_args["arm64"]))

    for toolchain_env in toolchain_envs:
        if toolchain_env != "":
            for toolchain_env_kv in toolchain_env.split(','):
                key, value = toolchain_env_kv.split('=', maxsplit=1)
                print("Applying toolchain envvar for build command: " + key + "=" + value)
                os.environ[key] = value

        for build_target, arch_args in build_targets:
            build_command = [
                "xcodebuild",
                "-workspace", "{}_{}.xcworkspace".format(PLUGIN_NAME, target_name),
                "-scheme", "All",
                "-configuration", config,
                "-quiet"
            ]
            build_command.extend(xcode_argsets[build_target])
            if arch_args:
                build_command.extend(arch_args)
            print("Build Command: " + str(build_command))
            res = subprocess.Popen(args=build_command).wait()
            if res != 0:
                return res

            invoke(POSTBUILD_HOOK, hooks, target_name, config, build_target)

        # clear the environment that was set
        if toolchain_env != "":
            for toolchain_env_kv in toolchain_env.split(','):
                key, value = toolchain_env_kv.split('=', maxsplit=1)
                os.environ.pop(key)
    return 0
