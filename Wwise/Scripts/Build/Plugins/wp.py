#!/usr/bin/env python3

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
import os
import sys

# scan the current directory for subcommands
subcommands_available = [
    os.path.basename(f)[:-3].replace("_", "-")
    for f in os.listdir(os.path.dirname(os.path.abspath(__file__)))
    if f.endswith(".py") and not f.startswith("_") and f != os.path.basename(__file__)
]

# parse the command line
parser = argparse.ArgumentParser(description="Audiokinetic Inc. tools for plugin development",
                                 epilog="Use the 'new' command to create a new plugin project in the current working directory. "
                                        "Other commands ('premake', 'build', ...) must be run inside the plugin project folder.")
parser.add_argument("command", choices=subcommands_available, help="command to run")
args = parser.parse_args(sys.argv[1:2])

# run the subcommand
subcommand = __import__(args.command.replace("-", "_"))
sys.exit(subcommand.run(sys.argv[2:]))
