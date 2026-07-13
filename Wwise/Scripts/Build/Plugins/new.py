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
import re
import shutil
import sys
import json

from pprint import pformat
from common.template import template_file

SCAFFOLDING_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "scaffolding"))
VSCODE_FOLDER = os.path.abspath(os.path.join(os.path.dirname(__file__), "common", "vscode"))

DISABLED_SECTION_BEGIN_TAG = "--[[DISABLED SECTION BEGIN"
DISABLED_SECTION_END_TAG = "DISABLED SECTION END--]]"

plugin_types_available = set(
    re.match(r"^(\S+).*$", f).group(1)
    for f in os.listdir(SCAFFOLDING_ROOT)
    if os.path.isdir(os.path.join(SCAFFOLDING_ROOT, f)) and f != "test"
)


def input_noexcept(prompt):
    while True:  # Reattempt until successful
        try:
            res = input(prompt)
            json.loads("{\"a\":\"" + res + "\"}")  # Validate as JSON string
            return res
        except EOFError:
            sys.stderr.write("[ERROR]: Could not decode provided text\n")
        except ValueError:
            sys.stderr.write("[ERROR]: Provided text does not yield valid JSON\n")

class ProjectNameArgValidator(argparse.Action):
    @staticmethod
    def validate(name):
        match = re.match(r"^[A-Za-z_]+[A-Za-z0-9_]*$", name)
        if not match:
            raise argparse.ArgumentError(None, "'{}' is not a valid name (it needs to be made of alphanumeric characters / underscores only and cannot start with a number)".format(name))

        if os.path.exists(os.path.join(os.getcwd(), name)):
            raise argparse.ArgumentError(None, "'{}' is not a valid name (a project already exists with that name)".format(name))

    def __call__(self, parser, namespace, values, option_strings=None):
        self.validate(values)
        setattr(namespace, self.dest, values)

class TestProjectArgValidator(argparse.Action):
    def __call__(self, parser, namespace, values, option_strings=None):
        if "test-project" == values:
            setattr(namespace, self.dest, True)
        elif "none" == values:
            setattr(namespace, self.dest, False)


class ProjectConfig:
    def __init__(self):
        self.type = None
        self.name = None
        self.display_name = None
        self.author = None
        self.description = None
        self.out_of_place = None
        self.no_prompt = None
        self.plugin_id = None
        self.plugin_attachment_id = None
        self.test_project = None

    def __str__(self):
        properties = vars(self)
        properties.pop("no_prompt")
        return pformat(properties)

    def fill_interactive(self):
        plugin_type_help = ", ".join(plugin_types_available)
        while not self.type:
            input_type = input_noexcept("plug-in type {{{}}}: ".format(plugin_type_help))
            if not input_type:
                print("'plug-in type' field is mandatory")
            elif input_type not in plugin_types_available:
                print("'{}' is not a valid type, please choose one of {{{}}}".format(input_type, plugin_type_help))
            else:
                self.type = input_type

        if (self.type == "effect" or self.type == "object_processor") and not self.no_prompt:
            if not self.out_of_place :
                res = input_noexcept("Do you need to do out-of-place processing? (no) ")
                self.out_of_place = bool(res and res.lower() != "no")

        while not self.name:
            input_name = input_noexcept("project name: ")
            if not input_name:
                print("'project name' field is mandatory")
            else:
                try:
                    ProjectNameArgValidator.validate(input_name)
                except Exception as e:
                    print(e)
                else:
                    self.name = input_name
        self.plugin_id = hash(self.name) % (2 ** 15 - 1)

        if not self.display_name:
            self.display_name = input_noexcept("display name: ({}) ".format(self.name))
        if not self.display_name:
            self.display_name = self.name

        while not self.author:
            input_author = input_noexcept("author: ")
            if not input_author:
                print("'author' field is mandatory")
            else:
                self.author = input_author

        if not self.description:
            self.description = input_noexcept("description: ")

        if self.test_project is None:
            res = input_noexcept("Add unit test infrastructure? (yes)")
            self.test_project = not res or res.lower() == "yes"

        if not self.no_prompt:
            print("\nAbout to create project with:")
            print(self)
            res = input_noexcept("Is this OK? (yes) ")
            return not res or res.lower() == "yes"

        return True

def get_template_dir(plugin_type, out_of_place):
    template_dir = plugin_type

    if plugin_type == "effect" or plugin_type == "object_processor":
        template_dir += " ({})".format("out-of-place" if out_of_place else "in-place")

    return template_dir

def mkdirs_from_file(file):
    with open(file) as f:
        for directory in f:
            directory = directory.rstrip() # remove newlines and other trailing whitespace
            if directory:
                os.makedirs(directory)

def remove_disabled_commented_sections(text):
    text = text.replace(DISABLED_SECTION_BEGIN_TAG, "")
    text = text.replace(DISABLED_SECTION_END_TAG, "")
    return text

def run(argv):
    # parse the command line
    parser = argparse.ArgumentParser(description="Audiokinetic Inc. plug-in scaffolding tool")
    group = parser.add_mutually_exclusive_group()
    for plugin_type in plugin_types_available:
        group.add_argument("--{}".format(plugin_type),
                           action="store_const",
                           dest="type",
                           const=plugin_type,
                           help="create {} plug-in".format(plugin_type))
    group = parser.add_mutually_exclusive_group()
    group.add_argument("--out-of-place", action="store_true", help="specify that the plug-in will do out-of-place processing (effects only)")
    parser.add_argument("-a", "--author", help="name of your organization")
    parser.add_argument("-d", "--description", default="", help="description of the project")
    parser.add_argument("-t", "--display-name", help="display name (title) of the project")
    parser.add_argument("-n", "--name", action=ProjectNameArgValidator, help="name of the project")
    parser.add_argument("--with", choices=["test-project", "none"], action=TestProjectArgValidator, dest="test_project", help="test-project : a preconfigured unit test project")
    parser.add_argument("--no-prompt", action="store_true", help="do not prompt for confirmation")

    # check if current path contains only ASCII characters.
    match = re.match(r"^[\x00-\x7F]*$", os.getcwd())
    if match is None:
        print("Your current working directory contains unicode characters in its path. Premake may not be able to generate your project files.")

    args = ProjectConfig()
    parser.parse_args(argv, namespace=args)
    if not args.fill_interactive():
        print("Aborted")
        return 0

    # generate the project
    print("Generating project structure for {}".format(args.name))
    shutil.copytree(os.path.join(SCAFFOLDING_ROOT, get_template_dir(args.type, args.out_of_place)), args.name)
    if args.test_project:
        shutil.copytree(os.path.join(os.path.join(SCAFFOLDING_ROOT, "test"), get_template_dir(args.type, args.out_of_place)), args.name, dirs_exist_ok=True)
        fileToSearch = os.path.join(args.name, "PremakePlugin.lua")
        with open(fileToSearch, 'r') as tempFile:
            fileData = tempFile.read()

        fileData = remove_disabled_commented_sections(fileData)

        with open(fileToSearch, 'w') as tempFile:
            tempFile.write(fileData)

    # copy vscode integration folder
    shutil.copytree(VSCODE_FOLDER, os.path.join(args.name, ".vscode"))

    os.chdir(args.name)

    for file in os.listdir(SCAFFOLDING_ROOT):
        filepath = os.path.join(SCAFFOLDING_ROOT, file)
        if os.path.isfile(filepath):
            if file == "directories":
                mkdirs_from_file(filepath)
            else:
                shutil.copy(filepath, file)

    # substitute templated parts for known file types
    for root, _, files in os.walk(os.getcwd()):
        for file in files:
            if re.match(r"^.*\.(?:lua|xml|json|h|cpp|def)$", file):
                # the name of the file itself may be templated
                if "ProjectName" in file:
                    oldfile = file
                    file = file.replace("ProjectName", args.name)
                    shutil.move(os.path.join(root, oldfile), os.path.join(root, file))

                template_file(os.path.join(root, file), vars(args))

    return 0

if __name__ == "__main__":
    sys.exit(run(sys.argv[1:]))
