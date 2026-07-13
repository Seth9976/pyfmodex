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
from glob import glob
import hashlib
import json
import os
import re
import subprocess
import sys
import tarfile
import codecs
from common.constant import PLUGIN_NAME, PROJECT_ROOT, XZ_UTILS
from common.version import VersionArgParser
from common.platform import *
from common.registry import platform_registry, is_common, is_documentation, is_authoring_target
from common.template import get_bundle_template
from common.util import exit_with_error

def run(argv):
    # parse the command line
    parser = argparse.ArgumentParser(description="Audiokinetic Inc. bundle generation tool for plugins")
    req_group = parser.add_argument_group("required arguments")
    req_group.add_argument("-v", "--version", action=VersionArgParser, help="version of the bundle (formatted as 'year.major.minor.build')", required=True)
    args = parser.parse_args(argv)

    # generate the bundle
    print("Generating bundle for {}".format(PLUGIN_NAME))

    package_glob = os.path.join(PROJECT_ROOT, PLUGIN_NAME + "_*.tar.xz")
    package_filename_re = re.compile(r"^" + PLUGIN_NAME + r"_v\d+\.\d+\.\d+_Build\d+_(?P<type>Authoring|SDK)\.(?P<platform>[A-z0-9_]+)(\.(?P<configuration>Debug|Release))?\.tar\.xz$")

    packages_found = glob(package_glob)
    if not packages_found:
        exit_with_error("Did not find any package!")

    bundle = get_bundle_template(PROJECT_ROOT, vars(args.version))
    bundle["files"] = []

    # generate the bundle info for each package
    for package in packages_found:
        print("Found {}".format(package))

        # compute sha1 checksum
        checksum = hashlib.sha1()
        checksum_bufsize = 2 ** 16 # read the file in chuncks of 64kb
        with open(package, "rb") as f:
            block = f.read(checksum_bufsize)
            while len(block) != 0:
                checksum.update(block)
                block = f.read(checksum_bufsize)

        checksum = checksum.hexdigest()
        print("checksum: {}".format(checksum))

        # compute uncompressed / compressed sizes
        compressedSize = os.path.getsize(package)
        uncompressedSize = 0
        try:
            with tarfile.open(package, "r:xz", format=tarfile.GNU_FORMAT) as tar:
                for tarinfo in tar:
                    uncompressedSize = uncompressedSize + tarinfo.size
        except tarfile.CompressionError:
            # xz compression isn't supported on older versions of tarfile
            output = subprocess.check_output([XZ_UTILS, "--robot", "-l", package])
            for line in output.split("\n"):
                if line.startswith("totals"):
                    uncompressedSize = int(line.split("\t")[4])
                    break

        print("compressed size: {} bytes".format(compressedSize))
        print("uncompressed size: {} bytes".format(uncompressedSize))

        # perform some regex gymnastics on the package's filename in order to
        # retrieve the necessary information to populate the bundle
        package_filename = os.path.basename(package)
        match = re.match(package_filename_re, package_filename)
        if not match:
            exit_with_error("Invalid filename {} at {}".format(package_filename, PROJECT_ROOT))

        package_group = match.group('type')
        package_platform = match.group('platform') # Includes the architecture when relevant
        package_configuration = match.group('configuration') or "" # Might be omitted
        package_licenses = []

        if is_authoring_target(package_group):
            if is_documentation(package_platform):
                package_group_value_id = "Authoring"
            else:
                package_group_value_id = f"Authoring_{package_platform}_{package_configuration}"
                package_licenses = {
                    "Debug": [
                        {"licenseId": "wwise_debug"},
                        {"licenseId": "maintenance"}
                    ],
                    "Release": []
                }.get(package_configuration, [])
        else:
            package_group_value_id = package_group

        package_name = f"{package_group_value_id}.{package_platform}.{PLUGIN_NAME}"
        package_info = {
            "groups": [{
                "groupId": "Packages",
                "groupValueId": package_group_value_id
            }],
            "id": package_filename,
            "licenses": package_licenses,
            "name": package_name,
            "sha1": checksum,
            "size": compressedSize,
            "sourceName": package_filename,
            "uncompressedSize": uncompressedSize
        }

        if not is_common(package_platform) and not is_documentation(package_platform):
            if not is_authoring_target(package_group):
                package_info.get("groups").append({
                    "groupId": "DeploymentPlatforms",
                    "groupValueId": package_platform
                })

            platform_info = platform_registry.get("Authoring" if is_authoring_target(package_group) else package_platform)
            if platform_info and platform_info.package and platform_info.package.is_licensed:
                package_info.get("licenses").append({
                    "licenseId": "maintenance",
                    "platformId": platform_info.package.license_platform_id or package_platform
                })

        bundle.get("files").append(package_info)

    with codecs.open(os.path.join(PROJECT_ROOT, "bundle.json"), "w", "utf-8") as f:
        json.dump(bundle, f, ensure_ascii=False)

    print("Wrote bundle.json")
    return 0

if __name__ == "__main__":
    sys.exit(run(sys.argv[1:]))
