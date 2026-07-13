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

from abc import ABC, abstractmethod

class TestExecutablePathProvider(ABC):
    @abstractmethod
    def get(self, wwise_root, target, arch, config, plugin_name):
        # Concrete method implementation should return a list of paths to test executables.
        raise NotImplementedError