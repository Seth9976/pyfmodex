# coding: utf-8

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
import os
import re
import logging

from shutil import copy2

from common.plugin_xml_parser import create_plugin_wobjects
from common.plugin_xml_parser import PropValueType
from common.plugin_xml_parser import property_attribute_dict
from common.constant import WWISE_ROOT, PROJECT_ROOT

MD_FOLDER = "Md"
HTML_FOLDER = "Html"
HTML_EXTENSION = ".html"

# Unsupported encodings by xml element tree parser
VERSION = " &version;"
REGISTERED_TRADEMARK = "&#174;"

TEMPLATE = """
{{content}}
"""

SDK_DOC = "SDKDoc="
HELP_DOC = "HelpDoc="

TRANSLATION_TIP = "Tip"
TRANSLATION_NOTE = "Note"
TRANSLATION_WARNING = "Warning"
TRANSLATION_CAUTION = "Caution"

TRANSLATION_TABLE = {
    'en': {
        TRANSLATION_TIP: TRANSLATION_TIP,
        TRANSLATION_NOTE: TRANSLATION_NOTE,
        TRANSLATION_WARNING: TRANSLATION_WARNING,
        TRANSLATION_CAUTION: TRANSLATION_CAUTION
    },
    'ja': {
        TRANSLATION_TIP: 'Tip',
        TRANSLATION_NOTE: u'注釈',
        TRANSLATION_WARNING: u'警告',
        TRANSLATION_CAUTION: u'注意'
    },
    'zh': {
        TRANSLATION_TIP: u'技巧',
        TRANSLATION_NOTE: u'备注',
        TRANSLATION_WARNING: u'警告',
        TRANSLATION_CAUTION: u'注意'
    },
    'ko': {
        TRANSLATION_TIP: u'작은 정보',
        TRANSLATION_NOTE: u'참고',
        TRANSLATION_WARNING: u'주의',
        TRANSLATION_CAUTION: u'경고'
    }
}

def format_md_content(md_content, property_list, file_name):
    md_content = clean_newlines_at_eof(md_content)

    anchors = re.findall(r"!?\[.*?\]\(.*?\)", md_content) #Find all anchors
    regex_anchor = re.compile(r"\[(.*?)\]\((.*?)\)")
    regex_http = re.compile(r"https?://.*")

    for anchor in anchors:
        match_obj = regex_anchor.match(anchor)
        if not match_obj or not match_obj.group(1) or not match_obj.group(2):
            logger.error("Anchor " + anchor + " doesn't have proper format in file " + file_name)
            sys.exit(1)

        # Only replace anchor if not a url
        if regex_http.match( match_obj.group(2) ):
            continue

        old_ref_id = get_anchor_ref_id(anchor)
        new_ref_id = HELP_DOC + old_ref_id

        new_anchor = anchor.replace(old_ref_id, new_ref_id)
        md_content = md_content.replace(anchor, new_anchor)

    prop_name = os.path.splitext( file_name )[0] #remove the .md
    prop = get_property(prop_name, property_list)
    if prop is not None:
        if prop.is_reference:
            md_content = insert_reference_values(md_content, prop)
        else:
            md_content = insert_property_values(md_content, prop)
    else:
        logger.warning(file_name + " does not have a property or reference in the xml file. It is most likely an InnerType (not supported by this script).")

    return md_content

# Returns the property having the name passed in parameter
def get_property(prop_name, prop_list):
    prop = None
    for x in prop_list:
        if x.name == prop_name:
            prop = x
    return prop

# Removes all newlines at the end of the file.
def clean_newlines_at_eof(md_content):
    md_content_lines = md_content.split("\n")

    index = md_content_lines.__len__() - 1
    while index > 0 and md_content_lines[index] == "":
        del md_content_lines[index]
        index = index - 1
    return "\n".join(md_content_lines)

# Writes property values parsed from xml files.
def insert_property_values(md_content, prop):
    if prop is None:
        return md_content

    idx_default = -1
    idx_range = -1
    idx_units = -1

    md_list = md_content.split('\n')

    regex_default = re.compile(r"(default value|default)\s*:")
    regex_range = re.compile(r"(range|slider range)\s*:")
    regex_units = re.compile(r"(unit|units)\s*:")

    # Look for property values already contained in .md
    for idx in range(len(md_list)):
        md_line = md_list[idx]
        md_line = md_line.strip()
        md_lower = md_line.lower()

        match_default = None
        match_range = None
        match_units = None

        if idx_default < 0:
            match_default = regex_default.match(md_lower)
            if match_default is not None: idxDefault = idx

        if idx_range < 0:
            match_range = regex_range.match(md_lower)
            if match_range is not None: idx_range = idx

        if idx_units < 0:
            match_units = regex_units.match(md_lower)
            if match_units is not None: idx_units = idx


        if match_default is not None or match_range is not None or match_units is not None:
            if md_lower[-5:] != "<br/>":
                md_line += "<br/>"

        md_list[idx] = md_line

    # Remember property value lines
    if idx_default >= 0: strDefault = md_list[idxDefault]
    if idx_range >= 0: strRange = md_list[idx_range]
    if idx_units >= 0: strUnits = md_list[idx_units]

    # Remove property value lines
    idx_to_remove = sorted([ idx_default, idx_range, idx_units ], reverse=True)
    for idx in idx_to_remove:
        if idx > 0:
            del md_list[idx]

    # Remove blank lines at end
    while len(md_list) > 0 :
        match_blank = re.match(r"[^\s\r\n]",md_list[-1])
        if match_blank is None:
            del md_list[-1]
        else:
            break

    # Add empty line between text and property values
    md_list.append('')

    if idx_default >= 0:
        md_list.append(strDefault)
    elif prop.data_default_value is not None:
        if prop.enumeration is None:
            default_value = str(prop.data_default_value)
            if prop.type.lower() == "bool":
                if default_value.lower() == "false" or default_value.lower() == "0":
                    default_value = "false"
                else:
                    default_value = "true"
            md_list.append("Default value: " + default_value + "<br/>")
        else:
            if str(prop.data_default_value) == "false":
                default_value = 0
            elif str(prop.data_default_value) == "true":
                default_value = 1
            else:
                default_value = int(prop.data_default_value)

            if default_value in prop.enumeration:
                md_list.append("Default value: " + str(prop.enumeration[default_value]) + "<br/>")
            else:
                logger.warning("Property " + prop.name + " has a default value that was not found in the enum")
                md_list.append("Default value: " + str(default_value) + "<br/>")

    if idx_range >= 0:
        md_list.append(strRange)
    elif prop.data_range[PropValueType.MIN] is not None and \
        prop.data_range[PropValueType.MAX] is not None \
        :
        minValue = str(prop.data_range[PropValueType.MIN])
        maxValue = str(prop.data_range[PropValueType.MAX])
        md_list.append("Range: " + minValue + " to " + maxValue + "<br/>")

    if idx_units >= 0:
        md_list.append(strUnits)
    elif  prop.data_meaning is not None:
        md_list.append("Units: " + str(property_attribute_dict[prop.data_meaning]) + "<br/>")

    md_list.append('')
    return "\n".join(md_list)

# Writes reference value restrictions parsed from xml file.
def insert_reference_values(md_content, prop):
    # No longer inserts values for reference types...
    return md_content

def get_anchor_ref_id(anchor):
    regex = re.compile(r"\[.*?\]\((.*?)\)")
    match_obj = regex.match(anchor)
    if match_obj:
        return match_obj.group(1)
    return None

# Converts a markdown file to html and places it in the Html folder
def md_file_to_html(md_files_path, file_name, property_list):
    rel_path = os.path.relpath( os.path.join(md_files_path,file_name), PROJECT_ROOT )
    logger.info( "Parsing " + rel_path )

    md_file = open( os.path.join(md_files_path,file_name), encoding='utf-8' )
    md = md_file.read()
    md_file.close()

    # If the file does not have a header tag, then we don't process it
    regex = re.compile(r"^\s*(?:#){1,6}[^#]*?$", flags=re.M)
    match = regex.search(md)
    if match is None:
        return

    # Look for custom .md include syntax ??filename.md
    regex = re.compile(r"^[ \t]*?\?\?([^\.\r\n]*\.md)[ \t]*$", flags=re.I|re.M)
    match = regex.search(md)
    while match is not None:
        insert_text = ""
        insert_name = match.group(1)
        if os.path.isfile( os.path.join(md_files_path,insert_name) ):
            insert_file = open( os.path.join(md_files_path,insert_name), encoding='utf-8' )
            insert_text = insert_file.read()
            insert_file.close()
        md = md[:match.start(0)] + insert_text + md[match.end(0):]
        match = regex.search(md)

    md = format_md_content(md, property_list, file_name)
    extensions = ['extra', 'smarty']
    import markdown
    html = markdown.markdown(md, extensions=extensions, output_format='html5')

    import jinja2
    doc = jinja2.Template(TEMPLATE).render(content=html)

    # Have to adjust html for <span> tags that cover more than one paragraph
    #   <p><span ...></p> and <p></span></p>
    # Just need to inverse the order of the tags
    regex = re.compile(r"<p>((?:<span\s+.*?>)|(?:<span>))(.*?)<\/span><\/p>", flags=re.I|re.S)
    doc = regex.sub(r"\1<p>\2</p></span>", doc)

    # Remove useless <p></p> tags within <li></li>
    regex = re.compile(r"<li>\s*<p>(.*?)</p>\s*</li>")
    doc = regex.sub(r"<li>\1</li>", doc)

    return doc

def translate_special( content, ln ):
    if ln not in TRANSLATION_TABLE:
        return content

    regex = re.compile( r"<strong>(\w*)</strong>:" )

    lines = content.split('\n')
    for idx in range(len(lines)):
        line = lines[idx]
        match = regex.search( line )
        while match is not None:
            tag = match.group(1)
            if tag in TRANSLATION_TABLE[ln]:
                line = line[:match.start(1)] + TRANSLATION_TABLE[ln][tag] + line[match.end(1):]
            match = regex.search( line, match.start(1)+1 )
        lines[idx] = line

    return "\n".join( lines )

def write_html_file(html, html_file_path, logger):
    html_file = open( html_file_path, "w", encoding='utf-8' )
    html_file.write( html )
    html_file.close()

    relPath = os.path.relpath( html_file_path, PROJECT_ROOT )
    logger.info( "Created " + relPath )

# Returns a list of all properties in a list of WObjects
def get_prop_list(full_wobjects_list):
    property_list = []
    for wobject in full_wobjects_list:
        for prop in wobject.property_list:
            if prop not in property_list:
                property_list.append(prop)
    return property_list

def get_values(prop):
    values = ""

    if prop.data_default_value is not None:
        values += "Default value: "
        values += str(prop.data_default_value)
        values += "<br/>"

    if prop.data_range[PropValueType.MIN] is not None and \
                    prop.data_range[PropValueType.MAX] is not None:
        values += "Range: "
        values += str(prop.data_range[PropValueType.MIN])
        values += " to "
        values += str(prop.data_range[PropValueType.MAX])
        values += "<br/>"

    if prop.data_meaning is not None:
        values += "Units: "
        values += str(property_attribute_dict[prop.data_meaning])
        values += "<br/>"

    return values

def create_plugins_html_in_path(path):
    # Open report file
    report_path = get_missing_props_file_path()
    if not os.path.exists(os.path.dirname(report_path)):
        os.makedirs(os.path.dirname(report_path))
    with open( report_path, "w", encoding='utf-8' ) as report_file:
        # Recursively get paths for all res folders in the given path
        folder_paths = []

        for x in os.walk(path):
            res_path = os.path.join(x[0], 'res')
            if os.path.isdir( res_path ):
                folder_paths.append( res_path )
            xml_path = os.path.join(x[0], 'xml')
            if os.path.isdir( xml_path ):
                folder_paths.append( xml_path )

        for res_path in folder_paths:
            file_path = ""
            for file_name in os.listdir(os.path.join(res_path, "..")):
                if file_name.endswith(".xml"):
                    file_path = os.path.join(os.path.join(res_path, ".."), file_name)
                    break
            if res_path:
                wobject_list = create_plugin_wobjects(file_path)
                property_list = get_prop_list(wobject_list)
                plugin_name = os.path.splitext( os.path.basename( file_path ) )[0]

                if os.path.isdir(os.path.join(res_path,MD_FOLDER)):
                    languages = get_language_folders(res_path)
                    create_output_dirs(res_path, languages)
                    create_plugin_output_dirs(plugin_name, languages)
                    created = create_html_from_xml( file_path, languages, wobject_list, property_list, report_file)
                    copy_to_plugin_output_dirs(plugin_name, created)



def create_plugins_html(plugin_path):
    create_plugins_html_in_path(plugin_path)

def create_html_from_xml(xml_path, languages, wobjects_list, property_list, report_file):
    relPath = os.path.relpath( xml_path, PROJECT_ROOT )

    logger.info("")
    logger.info( ">>>>> Starting " + relPath )
    logger.info("")

    dir_path = os.path.split( xml_path )[0]
    created = []

    for ln in languages:
        html_files_path = os.path.join(dir_path,"res",HTML_FOLDER,ln)
        md_files_path = os.path.join(dir_path,"res",MD_FOLDER,ln)
        file_names = os.listdir(md_files_path)
        lower_processed = []

        for md_file in file_names:
            content = md_file_to_html( md_files_path, md_file, property_list )
            if content:
                file_name_no_ext = os.path.splitext( md_file )[0]
                lower_processed.append( file_name_no_ext.lower() )
                html_file = os.path.join( html_files_path, file_name_no_ext + HTML_EXTENSION )
                created.append( html_file )
                content = translate_special( content, ln )
                write_html_file( content, html_file, logger )

        # Report missing files
        if ln == "en":
            reported_properties = []
            reported_count = 0

            report_file.write( "\nChecking for missing Markdown files for " + relPath + ":\n" )
            logger.info("")
            logger.info( "Checking for missing Markdown files for " + relPath )

            for _property in property_list:
                property_lower = _property.name.lower()
                if property_lower not in lower_processed and property_lower not in reported_properties and _property.is_visible:
                    report_file.write( _property.name + "\n" )
                    logger.info( _property.name )
                    reported_properties.append( property_lower )
                    reported_count += 1
            logger.info( "Missing markdown files for " + relPath + ": " + str(reported_count) )
            report_file.write( "Missing markdown files for " + relPath + ": " + str(reported_count) + "\n" )

    logger.info("")
    logger.info( ">>>>> Finished " + relPath )
    logger.info("")

    return created

# Get path for missing properties file
def get_missing_props_file_path():
    file_name = "missing_properties.txt"
    return os.path.join( PROJECT_ROOT, "Output", file_name )

# Creates output folder if they don't exist
def create_output_dirs(root_path, languages):
    create_output_dir( os.path.join( root_path, HTML_FOLDER ), languages )

def create_plugin_output_dirs(plugin_name, languages):
    path = os.path.join( WWISE_ROOT, "Authoring","Data", "Plugins" )
    path = os.path.join( path, plugin_name, HTML_FOLDER )
    create_output_dir( path, languages )

def copy_to_plugin_output_dirs( plugin_name, html_files ):
    path = os.path.join( WWISE_ROOT, "Authoring","Data", "Plugins", plugin_name )
    if os.path.isdir( path ):
        for html_file in html_files:
            language = os.path.split( os.path.split( html_file )[0] )[1]
            copy2( html_file, os.path.join( path, HTML_FOLDER, language ) )

# Creates all required html if they don't exist.
def create_output_dir(path, languages):
    if not os.path.isdir(path):
        os.makedirs(path)

    for lan in languages:
        if not os.path.isdir( os.path.join( path,  lan ) ):
            os.makedirs( os.path.join( path, lan ) )

# Creates list of language folders from md folder
def get_language_folders(root_path):
    if not os.path.isdir(root_path):
        logger.error("Invalid path. Cannot create html folder")
        sys.exit(1)

    languages = []

    md_folder_path = os.path.join(root_path,MD_FOLDER)
    md_folder_contents = os.listdir(md_folder_path)
    for it in md_folder_contents:
        if os.path.isdir(os.path.join(md_folder_path,it)):
            languages.append(it)

    return languages

def create_help():
    global logger
    # create logger
    logger = logging.getLogger('build_property_help_logger')
    logger.setLevel(logging.DEBUG)

    # create console handler and set level to debug
    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)

    # create formatter
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')

    # add formatter to ch
    ch.setFormatter(formatter)

    # add ch to logger
    logger.addHandler(ch)

    # 'application' code
    logger.debug('DEBUG')
    logger.info('INFO')
    logger.warn('WARNING')
    logger.error('ERROR')
    logger.critical('CRITICAL')

    LOG_FILE_PATH = os.path.join( PROJECT_ROOT, "Output" )
    LOG_FILE_NAME = os.path.join( LOG_FILE_PATH, "build_property_help.log" )

    if not os.path.exists(LOG_FILE_PATH):
        os.makedirs(LOG_FILE_PATH)

    if os.path.isfile(LOG_FILE_NAME):
        os.remove(LOG_FILE_NAME)

    logging.basicConfig(filename=LOG_FILE_NAME)

    create_plugins_html(os.path.join(PROJECT_ROOT, "WwisePlugin"))
