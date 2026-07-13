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
import platform
import re
from common.util import exit_with_error, index_or_error

WWISE_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", ".."))
PROJECT_ROOT = os.getcwd()

def _get_plugin_name():
    try:
        with open(os.path.join(PROJECT_ROOT, "PremakePlugin.lua")) as f:
            line = f.readline()
            while line:
                if line.startswith("Plugin.name"):
                    return re.search(r"(?<=\")\w+",line).group(0)

                line = f.readline()

        exit_with_error("Missing Plugin.name field in PremakePlugin.lua file at {}".format(PROJECT_ROOT))

    except IOError:
        exit_with_error("Missing PremakePlugin.lua file at {}".format(PROJECT_ROOT))

PLUGIN_NAME = _get_plugin_name()

XZ_UTILS = os.path.join(WWISE_ROOT, "Tools", "Win32", "bin", "xz.exe") if platform.system() == "Windows" else "xz"

PREMAKE = index_or_error({
    "Windows": os.path.join(WWISE_ROOT, "Tools", "Win32", "bin", "premake5.exe"),
    "Darwin": os.path.join(WWISE_ROOT, "Tools", "Mac", "bin", "premake5"),
    "Linux": os.path.join(WWISE_ROOT, "Tools", "Linux", "bin", "premake5")
}, platform.system(), "Premake5 is not supported on {}. Exiting...".format(platform.system()))
