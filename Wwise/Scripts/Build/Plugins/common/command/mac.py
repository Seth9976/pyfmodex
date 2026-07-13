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
import multiprocessing
import os
import platform
import subprocess
from common.constant import PLUGIN_NAME
from common.hook import invoke, POSTBUILD_HOOK
from common.registry import platform_registry

SUPPORTED_BUILD_SYSTEMS = ("Darwin",)

def build(target, config, hooks):

    platform_info = platform_registry.get(target)
    for arch in platform_info.build.archs:
        build_command = (
            "make",
            "-f", "{}_{}.make".format(PLUGIN_NAME, target),
            "-j", str(multiprocessing.cpu_count())
        )
        build_env = {}
        build_env["config"] = "{}_{}".format(config.lower(), arch)
        print("Build Command: " + str(build_command))
        print("Build Environment: " + str(build_env))
        merged_build_env = os.environ.copy()
        merged_build_env.update(build_env) # Inherit rest of environment from caller

        res = subprocess.Popen(args=build_command, env=merged_build_env).wait()
        if res != 0:
            return res

        invoke(POSTBUILD_HOOK, hooks, target, config, arch)

    return 0
