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

import re
import sys

def exit_with_error(err):
    """
    Print a message to stderr and exit with an error code.

    :param err: the message that will be printed
    :type err: str
    """
    sys.stderr.write("ERROR: {}\n".format(err))
    sys.exit(1)


def index_or_error(dictionary, key, err):
    """
    Try to retrieve the key of a dictionary and call exit_with_error if it fails.

    :param dictionary: the dictionary to index
    :type dictionary: dict[Any, Any]
    :type key: Any
    :param err: the message that will be printed
    :type err: str
    """
    val = dictionary.get(key)
    if val is None:
        exit_with_error(err)
    return val

def strip_comments(string):
    """
    Strips c-style // commments from a string.

    :param string: the string to transform
    :type string: str
    """
    return re.sub(r"\"[^\"]*\"|(//.*$)",
                  lambda m: "" if m.group(1) else m.group(0),
                  string, flags=re.M)
