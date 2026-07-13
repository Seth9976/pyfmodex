import sys
import re
import os

def GetToolchainEnv():
    if (len(sys.argv) != 2):
        print ("Error: Incorrect arguments provided. Only the \"Toolchain Version\" should be provided as an argument.", file=sys.stderr)
        exit(1)
        
    # provided argument is expected to be a value from ToolchainVers.txt, e.g. "Xcode1500".
    # extract the Xcode version numbers from the argument
    match = re.match(r"Xcode(\d\d)(\d)(\d)", sys.argv[1])
    if not match:
        print("Error: Expected Toolchain Version to have the format \"XcodeXXYZ\".")
        exit(1)

    # Internal version is always XXYZ. Public version is XX.Y.Z, or simply XX.Y if Z is 0, or simply XX if both Y and Z are 0.
    xcode_internal_version = "{}{}{}".format(match.group(1), match.group(2), match.group(3))
    xcode_public_version = match.group(1)
    if match.group(2) != "0":
        xcode_public_version = "{}.{}".format(match.group(1), match.group(2))
        if match.group(3) != "0":
            xcode_public_version = "{}.{}.{}".format(match.group(1), match.group(2), match.group(3))

    # By convention, the path to the Xcode installation is /Applications/Xcode15.2.app
    xcode_location = "/Applications/Xcode{}.app/Contents/Developer".format(xcode_public_version)

    # Users can override the location of the Xcode installation path by specifying the AK_XCODE_DEVELOPER_DIR_XXYZ environment variable
    env_name = "AK_XCODE_DEVELOPER_DIR_{}".format(xcode_internal_version)
    overriden_path = os.environ.get(env_name)
    if overriden_path:
        xcode_location = overriden_path

    # DEVELOPER_DIR is the official, documented environment variable used by xcodebuild and xcrun
    print ("DEVELOPER_DIR={}".format(xcode_location), file=sys.stdout, end='')
