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
from common.constant import WWISE_ROOT, PROJECT_ROOT

POSTBUILD_HOOK = "wp.hooks.postbuild"

def invoke(hook, module, target, config, arch):
    if not module: return
    if hook == POSTBUILD_HOOK and callable(module.postbuild):
        module.postbuild(platform=target, config=config, arch=arch, wwise_root=WWISE_ROOT, project_root=PROJECT_ROOT)
