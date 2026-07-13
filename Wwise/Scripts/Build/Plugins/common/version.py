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

import argparse
import re

class Version:
    def __init__(self, year, major, minor, build):
        """
        :type year: int
        :type major: int
        :type minor: int
        :type build: int
        """
        self.year = year
        self.major = major
        self.minor = minor
        self.build = build

class VersionArgParser(argparse.Action):
    def __call__(self, parser, namespace, values, option_strings=None):
        match = re.match(r"^(\d+)\.(\d+)\.(\d+)\.(\d+)$", values)
        if not match:
            raise argparse.ArgumentError(self, "'{}' is not a valid version (it needs to be formatted as 'year.major.minor.build')".format(values))

        values = Version(int(match.group(1)),
                         int(match.group(2)),
                         int(match.group(3)),
                         int(match.group(4)))
        setattr(namespace, self.dest, values)
