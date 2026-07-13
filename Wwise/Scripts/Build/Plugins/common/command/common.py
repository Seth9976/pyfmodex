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

import os
import platform
import subprocess
from common.constant import PLUGIN_NAME, WWISE_ROOT
from common.registry import platform_registry

def exec_test_locally(target):
    platform_info = platform_registry.get(target)
    for arch in platform_info.build.archs:
        if ((arch.lower() in ["win32", "x64", "x86_64"] and platform.machine().lower() not in ["x86_64", "amd64"]) or
            (arch.lower() in ["arm64", "aarch64"] and platform.machine().lower() not in ["arm64", "aarch64"])):
            print(f"Architecture [{arch}] is incompatible with local machine architecture [{platform.machine()}], skipping test execution.")
            continue

        for config in platform_info.build.configurations:
            test_commands = platform_info.test.exec_path_provider.get(WWISE_ROOT, target, arch, config, PLUGIN_NAME)
            for test_command in test_commands:
                if os.path.exists(test_command):
                    print(f"Running {test_command}.")
                    completed_process = subprocess.run(test_command)
                    if completed_process.returncode != 0:
                        return completed_process.returncode
                else:
                    print(f"Application:{test_command} does not exist, run the build command first or use -c, -x, -t arguments to select configuration, architecture and or toolset to run.")
                    return -1

    return 0