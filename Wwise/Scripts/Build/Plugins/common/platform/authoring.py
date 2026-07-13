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

import platform
from common.registry import platform_registry

try:
    if platform.system() == "Darwin":
        from common.platform.authoring_mac import PLATFORM_INFO
    elif platform.system() == "Linux":
        from common.platform.authoring_linux import PLATFORM_INFO
    elif platform.system() == "Windows":
        from common.platform.authoring_windows import PLATFORM_INFO

    platform_registry["Authoring"] = PLATFORM_INFO
except:
    # If none is packaged, ignore
    pass
