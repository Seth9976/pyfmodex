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
from common.command.vs import build, SUPPORTED_BUILD_SYSTEMS
from common.registry import PlatformInfo, PremakeInfo, BuildInfo, PackageInfo, PlatformTriplet, platform_registry

name = "Authoring_Windows"
toolsets = ("vc160", "vc170")
archs = ("x64",)

def get_wwise_platform_to_triplet():
    results = {}

    for arch in archs:
        triplets = []

        for toolset in toolsets:
            triplets.append(PlatformTriplet(name, arch, toolset))

        results[f"Windows_{arch}"] = triplets

    return results

PLATFORM_INFO = PlatformInfo(
    name=name,
    premake=PremakeInfo(
        actions=("vs2019", "vs2022"),
        platform="windows"
    ),
    build=BuildInfo(
        command=build,
        configurations=("Debug", "Profile", "Release", "Debug_StaticCRT", "Profile_StaticCRT", "Release_StaticCRT"),
        archs=archs,
        toolsets=toolsets,
        on=SUPPORTED_BUILD_SYSTEMS,
        require_configuration=True,
        require_toolset=True,
        wwise_platform_to_triplet=get_wwise_platform_to_triplet()
    ),
    package=PackageInfo(
        artifacts=[
            os.path.join("Authoring", "x64", "*", "bin", "Plugins", "{plugin_name}", "*"),
            os.path.join("Authoring", "x64", "*", "bin", "Plugins", "{plugin_name}.dll"),
            os.path.join("Authoring", "x64", "*", "bin", "Plugins", "{plugin_name}.xml"),
        ]
    )
)

platform_registry[name] = PLATFORM_INFO