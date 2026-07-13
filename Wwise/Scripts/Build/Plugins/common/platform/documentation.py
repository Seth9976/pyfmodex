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
from common.command.documentation import build, SUPPORTED_BUILD_SYSTEMS
from common.registry import PlatformInfo, BuildInfo, PackageInfo, PlatformTriplet, platform_registry

name = "Documentation"

def get_wwise_platform_to_triplet():
    results = {}

    results[name] = [PlatformTriplet(name)]

    return results

platform_registry[name] = PlatformInfo(
    name=name,
    build=BuildInfo(
        command=build,
        on=SUPPORTED_BUILD_SYSTEMS,
        require_configuration=False,
        wwise_platform_to_triplet=get_wwise_platform_to_triplet()
    ),
    package=PackageInfo(
        artifacts=[
            os.path.join("Authoring", "Data", "Plugins", "{plugin_name}", "Html", "*", "*"),
        ]
    )
)
