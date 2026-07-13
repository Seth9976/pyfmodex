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
import subprocess
import sys
from common.constant import PLUGIN_NAME
from common.hook import invoke, POSTBUILD_HOOK
from common.registry import platform_registry
from common.util import exit_with_error

SUPPORTED_BUILD_SYSTEMS = ("Windows",)

class VSNotFoundException(Exception):
    pass

class EnvStrategy:
    def __init__(self, env):
        self.env = env

    def execute(self):
        if os.environ.get(self.env):
            return os.environ[self.env]

class LocationStrategy:
    def __init__(self, year):
        self.year = year

    def execute(self):
        vs_root = os.path.join(os.environ["ProgramFiles(x86)"], "Microsoft Visual Studio", self.year)
        for install_type in ("Enterprise", "Professional", "Community", "BuildTools"):
            common_tools = os.path.join(vs_root, install_type, "Common7", "Tools")
            if os.path.exists(common_tools):
                return common_tools


class VSWhereStrategy:
    def __init__(self, min_version, max_version=None):
        max_version = max_version or min_version + 1
        assert(min_version < max_version)

        self.is_legacy = min_version <= 14

        version_range = "[{}.0,{}.0)".format(min_version, max_version)
        self.flags = ["-version " + version_range]

        if self.is_legacy:
            self.flags.append("-legacy")

    def requires(self, *components):
        assert(not self.is_legacy)
        for c in [components] if isinstance(components, str) else components:
            self.flags.append("-requires {}".format(c))
        return self

    def execute(self):
        vswhere = os.path.join(os.environ["ProgramFiles(x86)"], "Microsoft Visual Studio", "Installer", "vswhere.exe")
        if self.flags and os.path.exists(vswhere):
            output = subprocess.getoutput("\"{}\" {} -format json".format(vswhere, " ".join(self.flags)))
            try:
                products = json.loads(output)
                for product in products:
                    common_tools = os.path.join(product.get("installationPath"), "Common7", "Tools")
                    if os.path.exists(common_tools):
                        return common_tools
            except json.decoder.JSONDecodeError:
                print("Invalid JSON returned by VSWhere")
                pass


def find_common_tools(vs_version):
    msbuild = "Microsoft.Component.MSBuild"

    strategies = {
        "vc160": [
            LocationStrategy("2019"),
            VSWhereStrategy(16).requires(msbuild)
        ],
        "vc170": [
            LocationStrategy("2022"),
            VSWhereStrategy(17).requires(msbuild)
        ]
    }.get(vs_version, [])

    for strategy in strategies:
        common_tools = strategy.execute()
        if common_tools:
            return common_tools

    raise VSNotFoundException("Could not find the Visual Studio Build Tools for {}".format(vs_version))

def build(target, config, hooks):
    platform_info = platform_registry.get(target)

    num_processors = os.environ['NUMBER_OF_PROCESSORS']

    # fetch the list of toolchainvers, and add additional environments to set
    if platform_info.build.toolchain_env_script is not None and platform_info.build.toolchain_vers is not None:
        print("Loading toolchain vers file from ", platform_info.build.toolchain_vers)
        with open(platform_info.build.toolchain_vers, 'r') as toolchain_vers_file:
            toolchain_vers_list = toolchain_vers_file.readlines()

        toolchain_envs = list()
        print("Fetching toolchain environment from ", platform_info.build.toolchain_env_script)
        for toolchain_ver in toolchain_vers_list:
            toolchain_ver = toolchain_ver.strip()
            if (toolchain_ver.startswith("#")):
                continue
            toolchain_env = subprocess.check_output(sys.executable + " \"" + platform_info.build.toolchain_env_script + "\" " + toolchain_ver).decode("utf-8")
            toolchain_env_ex = os.path.expandvars(toolchain_env)
            toolchain_envs.append(toolchain_env_ex)
    elif (platform_info.build.toolchain_env_script is not None) ^ (platform_info.build.toolchain_vers is not None):
        print("WARNING: Only one of toolchain_env_script or toolchain_vers was defined. Both must be defined to fetch requisite toolchain definitions.")
        print("toolchain_env_script: ", str(platform_info.build.toolchain_env_script))
        print("toolchain_vers: ", str(platform_info.build.toolchain_vers))
        toolchain_envs = [""]
    else: # neither the toolchain_env_script nor toolchain_vers is defined
        toolchain_envs = [""]

    for arch in platform_info.build.archs:
        for vs_version in platform_info.build.toolsets:

            project_name = "{}_{}".format(PLUGIN_NAME, target)

            # Deprecated: Support for legacy platform nomenclature. toolset should be removed from platform name (Windows_vcXXX).
            if project_name.endswith("_{}".format(vs_version)):
                project_name = project_name[0:project_name.index("_{}".format(vs_version))]

            try:
                setup_command = os.path.join(find_common_tools(vs_version), "VsMSBuildCmd.bat")
                build_tool = "\"{}\" && msbuild.exe".format(setup_command)
            except VSNotFoundException as ex:
                exit_with_error(ex.message)

            solutions = []
            if os.path.isfile( project_name + "_" + vs_version + ".sln" ):
                solutions.append( project_name + "_" + vs_version + ".sln" )
            else:
                solutions.append( project_name + "_" + vs_version + "_static.sln" )
                if "StaticCRT" not in config:
                    solutions.append( project_name + "_" + vs_version + "_shared.sln" )

            for toolchain_env in toolchain_envs:
                if toolchain_env != "":
                    for toolchain_env_kv in toolchain_env.split(','):
                        key, value = toolchain_env_kv.split('=', maxsplit=1)
                        print("Applying toolchain envvar for build command: " + key + "=" + value)
                        os.environ[key] = value

                for solution in solutions:
                    build_command = build_tool + " " + solution \
                        + " /t:Build" \
                        + " /p:Configuration=\"" + config + "\"" \
                        + " /p:Platform=\"" + arch + "\" /m /verbosity:minimal" \
                        + " /p:CL_MPCount=\"" + num_processors + "\""

                    print("Building {} in {} using {}. Build Command:\n{}".format(
                        target, config, vs_version, build_command))
                    res = subprocess.Popen(build_command).wait()
                    if res != 0:
                        return res

                invoke(POSTBUILD_HOOK, hooks, target, config, "{}_{}".format(arch, vs_version))

                # clear the environment that was set
                if toolchain_env != "":
                    for toolchain_env_kv in toolchain_env.split(','):
                        key, value = toolchain_env_kv.split('=', maxsplit=1)
                        os.environ.pop(key)

    return 0
