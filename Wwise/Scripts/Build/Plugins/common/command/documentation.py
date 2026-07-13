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
from common.build_property_help import build_property_help
from common.hook import invoke, POSTBUILD_HOOK

SUPPORTED_BUILD_SYSTEMS = ("Windows", "Darwin")

def build(target, config, hooks):
    build_property_help()
    invoke(POSTBUILD_HOOK, hooks, target, config, None)
    return 0
