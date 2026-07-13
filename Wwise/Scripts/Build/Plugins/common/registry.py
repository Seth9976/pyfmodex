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

import os

# when a platform is imported, it automatically gets registered into this.
platform_registry = {}

class PremakeInfo:
    def __init__(self, actions=None, platform=None):
        """
        :param actions: name of the actions that will be given to premake
        :type actions: collections.Iterable[str] | None
        :param platform: name of the platform as known to premake
        :type platform: str | None
        """
        self.actions = actions
        self.platform = platform

class BuildInfo:
    def __init__(self, command=None, configurations=None, archs=None, toolsets=None, toolchain_env_script=None, toolchain_vers=None, on=None, require_configuration=False, require_arch=False, require_toolset=False, wwise_platform_to_triplet={}):
        """
        :param command: command to call in order to build the platform
        :type command: (str, str) -> int | None
        :param configurations: configurations supported by the platform
        :type configurations: collections.Iterable[str] | None
        :param archs: architectures supported by the platform
        :type archs: collections.Iterable[str] | None
        :param toolsets: toolsets supported by the platform
        :type toolsets: collections.Iterable[str] | None
        :param toolchain_env_script: Optional script to run to get appropriate toolchain environments for each toolchain ver. toolchain_env_script and toolchain_vers must both be defined together.
        :type toolchain_env_script: str | None
        :param toolchain_vers: Optional file to query for supported toolchain versions. toolchain_env_script and toolchain_vers must both be defined together.
        :type toolchain_vers: str | None
        :param on: systems on which the command is supported, uses the same names as those returned by platform.system()
        :type on: collections.Iterable[str] | None
        :param require_configuration: whether or not the build command requires the -c flag to be set, defaults to False
        :type require_configuration: bool
        :param require_arch: whether or not the build command requires the -x flag to be set, defaults to False
        :type require_arch: bool
        :param require_toolset: whether or not the build command requires the -t flag to be set, defaults to False
        :type require_toolset: bool
        """
        self.command = command
        self.configurations = configurations or []
        self.archs = archs or []
        self.toolsets = toolsets or []
        self.toolchain_env_script = toolchain_env_script
        self.toolchain_vers = toolchain_vers
        self.on = on or []
        self.require_configuration = require_configuration
        self.require_arch = require_arch
        self.require_toolset = require_toolset
        self.wwise_platform_to_triplet = wwise_platform_to_triplet

class TestInfo:
    def __init__(self, command=None, on=None, exec_path_provider=None):
        """
        :param command: command to call in order to run unit tests for the platform
        :type command: (str, str) -> int | None
        :param on: systems on which the command is supported, uses the same names as those returned by platform.system()
        :type on: collections.Iterable[str] | None
        :param test_app_names: list of test application names to run
        :type test_app_names: collections.Iterable[str] | None
        """
        self.command = command
        self.on = on or []
        self.exec_path_provider = exec_path_provider

class PackageInfo:
    def __init__(self, artifacts=None, is_licensed=False, license_platform_id=None):
        """
        :param artifacts:
        :type artifacts: collections.Iterable[str] | None
        :param is_licensed: whether or not the platform is licensed, defaults to False
        :type is_licensed: bool
        """
        self.artifacts = artifacts or []
        self.is_licensed = is_licensed
        self.license_platform_id = license_platform_id

class PlatformInfo:
    def __init__(self, name="", premake=None, build=None, test=None, package=None):
        """
        :param name: Complete platform name
        :type name: string | ""
        :type premake: PremakeInfo | None
        :type build: BuildInfo | None
        :type test: TestInfo | None
        :type package: PackageInfo | None
        """
        self.name = name
        self.premake = premake
        self.build = build
        self.test = test
        self.package = package

class PlatformTriplet:
    def __init__(self, name="", arch="", toolset=""):
        """
        :param name: Complete platform name for this triplet
        :type name: string | ""
        :param arch: Architecture for this triplet
        :type arch: string | ""
        :param toolset: Toolset for this triplet
        :type toolset: string | ""
        """
        self.name = name
        self.arch = arch
        self.toolset = toolset

def get_supported_platforms(feature, system=None):
    """
    :param feature: name of the feature that must be supported (premake, build, etc.)
    :type feature: str
    :param system: name of the system on which the feature must be supported, uses the same names as those returned by platform.system()
    :type system: str | None
    :rtype: list[str]
    """
    return [
        name for name, info in platform_registry.items()
        if getattr(info, feature) and (not system or system in getattr(info, feature).on)
    ]

def get_wwise_platform_to_triplet_mapping():
    """
    :param feature: name of the feature that must be supported (premake, build, etc.)
    :type feature: str
    :param system: name of the system on which the feature must be supported, uses the same names as those returned by platform.system()
    :type system: str | None
    :rtype: list[str]
    """
    triplets_mapping = {}
    for _, info in platform_registry.items():
        if info.build:
            triplets_mapping.update(info.build.wwise_platform_to_triplet)

    return triplets_mapping

def is_authoring_target(platform):
    """
    :param platform: name of the platform to check
    :type platform: str
    :rtype: True if authoring needs to be built, False otherwise
    """
    return "authoring" in platform.lower()

def is_common(platform):
    """
    :param platform: name of the platform to check
    :type platform: str
    :rtype: True if the platform is the common platform, False otherwise
    """
    return platform.lower() == "common"

def is_documentation(platform):
    """
    :param platform: name of the platform to check
    :type platform: str
    :rtype: True if the platform is the documentation platform, False otherwise
    """
    return platform.lower() == "documentation"
