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
import json
import os
import codecs
from common.util import exit_with_error, strip_comments

def template_file(file, variables):
    """
    Searches a file for templated parts formatted as ${var_name} and replaces
    them with a given value.

    :param file: path to the file to template
    :type file: str
    :param variables: dictionary of variable names to values, the values will be stringified
    :type variables: dict[str, Any]
    """
    with codecs.open(file, "r+", "utf-8") as f:
        content = f.read()

        for var_name, var_value in variables.items():
            content = content.replace("${" + var_name + "}", str(var_value))

        f.seek(0)
        f.write(content)
        f.truncate()

def get_bundle_template(project_path, variables=None):
    """
    Retrieves the bundle template of the given project.
    This will also strip any comment and replace templated parts of the file,
    much like the template_file function.

    :param project_path: path to the root of the project.
    :type project_path: str
    :param variables: dictionary of variable names to values, the values will be stringified
    :type variables: dict[str, Any]
    """
    variables = variables or {}
    try:
        with codecs.open(os.path.join(project_path, "bundle_template.json"), "r", "utf-8") as f:
            content = strip_comments(f.read())

            for var_name, var_value in variables.items():
                content = content.replace("${" + var_name + "}", str(var_value))

            return json.loads(content)

    except IOError:
        exit_with_error("Missing bundle_template.json file at {}".format(project_path))

    except ValueError:
        exit_with_error("Invalid bundle_template.json file at {}".format(project_path))
