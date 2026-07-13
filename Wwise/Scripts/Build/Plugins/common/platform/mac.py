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
from common.constant import WWISE_ROOT
from common.command.xcode import build, SUPPORTED_BUILD_SYSTEMS
from common.command.common import exec_test_locally
from common.command.interfaces import TestExecutablePathProvider
from common.registry import PlatformInfo, PremakeInfo, BuildInfo, TestInfo, PackageInfo, PlatformTriplet, platform_registry

toolchain_vers_path = os.path.join(WWISE_ROOT, "Scripts/ToolchainSetup/Mac/ToolchainVers.txt")

name = "Mac"
archs = ("arm64", "x86_64")

def get_wwise_platform_to_triplet():
    results = {}

    # build univerval binaries
    results[name] = [PlatformTriplet(name)]

    return results

class MacTestPathProvider(TestExecutablePathProvider):
    def get(self, wwise_root, target, arch, config, plugin_name):
        platform_info = platform_registry.get(target)

        test_paths = []
        with open(platform_info.build.toolchain_vers, 'r') as reader:
            lines = reader.readlines()
            for line in lines:
                test_paths.append(os.path.join(wwise_root, "SDK", f"Mac_{line.strip()}", config, "bin", f"{plugin_name}Test"))
        return test_paths

platform_registry[name] = PlatformInfo(
    name=name,
    premake=PremakeInfo(
        actions=("xcode4",),
        platform="macosx"
    ),
    build=BuildInfo(
        command=build,
        configurations=("Debug", "Profile", "Release"),
        archs=archs,  # if unspecified, defaults to universal
        toolchain_env_script=os.path.join(WWISE_ROOT, "Scripts/ToolchainSetup/Mac/GetToolchainEnv.py"),
        toolchain_vers=toolchain_vers_path,
        on=SUPPORTED_BUILD_SYSTEMS,
        require_configuration=True,
        wwise_platform_to_triplet=get_wwise_platform_to_triplet()
    ),
    test=TestInfo(
        command=exec_test_locally,
        on=SUPPORTED_BUILD_SYSTEMS,
        exec_path_provider=MacTestPathProvider(),
    ),
    package=PackageInfo(
        artifacts=[
            os.path.join("SDK", "Mac_*", "*", "bin", "lib{plugin_name}.dylib"),
            os.path.join("SDK", "Mac_*", "*", "lib", "lib{plugin_name}*.a"),
        ]
    )
)
