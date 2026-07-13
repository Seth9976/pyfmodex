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

import sys
import argparse
import os
import re

from common.clean import remove_folders
from common.md_to_html import create_help

def build_property_help():
    # Clean output folders
    remove_folders()

    # Generate property help.  The call below will take care of:
    #   converting .md to .html
    #   report any missing property .md files, per .xml file
    create_help()