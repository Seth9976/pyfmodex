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
from common.command.common import exec_test_locally
from common.command.interfaces import TestExecutablePathProvider
from common.command.vs import build, SUPPORTED_BUILD_SYSTEMS
from common.registry import PlatformInfo, PremakeInfo, BuildInfo, TestInfo, PackageInfo, PlatformTriplet, platform_registry

class Windows170TestPathProvider(TestExecutablePathProvider):
    def get(self, wwise_root, target, arch, config, plugin_name):
        return [os.path.join(wwise_root, "SDK", f"{arch}_vc170", config, "bin", f"{plugin_name}Test.exe")]


name = "Windows_vc170"
toolsets = ("vc170",)
archs = ("Win32", "x64", "ARM64")

def get_wwise_platform_to_triplet():
    results = {}

    for arch in archs:
        triplets = []

        for toolset in toolsets:
            triplets.append(PlatformTriplet(name, arch, toolset))

        results[f"{arch}_{toolset}"] = triplets

    return results

platform_registry[name] = PlatformInfo(
    name=name,
    premake=PremakeInfo(
        actions=("vs2022",)
    ),
    build=BuildInfo(
        command=build,
        configurations=("Debug", "Profile", "Release", "Debug(StaticCRT)", "Profile(StaticCRT)", "Release(StaticCRT)"),
        archs=archs,
        toolsets=toolsets,
        on=SUPPORTED_BUILD_SYSTEMS,
        require_configuration=True,
        wwise_platform_to_triplet=get_wwise_platform_to_triplet()
    ),
    test=TestInfo(
        command=exec_test_locally,
        on=SUPPORTED_BUILD_SYSTEMS,
        exec_path_provider=Windows170TestPathProvider(),
    ),
    package=PackageInfo(
        artifacts=[
            os.path.join("SDK", "Win32_vc170", "*", "bin", "{plugin_name}.dll"),
            os.path.join("SDK", "Win32_vc170", "*", "bin", "{plugin_name}.pdb"),
            os.path.join("SDK", "Win32_vc170", "*", "lib", "{plugin_name}*.lib"),
            os.path.join("SDK", "x64_vc170", "*", "bin", "{plugin_name}.dll"),
            os.path.join("SDK", "x64_vc170", "*", "bin", "{plugin_name}.pdb"),
            os.path.join("SDK", "x64_vc170", "*", "lib", "{plugin_name}*.lib"),
            os.path.join("SDK", "ARM64_vc170", "*", "bin", "{plugin_name}.dll"),
            os.path.join("SDK", "ARM64_vc170", "*", "bin", "{plugin_name}.pdb"),
            os.path.join("SDK", "ARM64_vc170", "*", "lib", "{plugin_name}*.lib"),
        ]
    )
)
