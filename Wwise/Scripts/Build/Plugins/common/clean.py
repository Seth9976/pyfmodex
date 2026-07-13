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

""" This script deletes all the html files created by md_to_html.py.py """

import glob
import os
from glob import glob
from shutil import rmtree
from common.constant import WWISE_ROOT, PROJECT_ROOT, PLUGIN_NAME

def delete_html_folders():
    delete_html_folder_in_path(os.path.join( PROJECT_ROOT, "WwisePlugin" ))
    delete_html_folder_in_path(os.path.join( WWISE_ROOT, "Authoring", "Data", "Plugins", PLUGIN_NAME ))

def delete_html_folder_in_path(path):
    # Recursively get paths for all Html folders in the Plugins folder
    html_folders_paths = [y for x in os.walk(path) for y in glob(os.path.join(x[0], 'Html'))]
    for html_folder_path in html_folders_paths:
        if os.path.isdir(html_folder_path):
            rmtree(html_folder_path, ignore_errors=True)

def remove_folders():
    delete_html_folders()
    print( "Property help folder cleanup done !" )
