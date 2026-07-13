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
import argparse
import copy
from collections import OrderedDict
import  xml.etree.ElementTree as ET

class PropertyData():
    DECIBELS = 1
    PITCHCENTS = 2
    FREQUENCY = 3
    PERCENTAGE = 4

property_data_meanings = {
    PropertyData.DECIBELS : "Decibels",
    PropertyData.PITCHCENTS : "PitchCents",
    PropertyData.FREQUENCY : "Frequency",
}

# maps the DataMeaning attribute to what it should display
property_attribute_dict = {
    property_data_meanings[PropertyData.DECIBELS] : "dB",
    property_data_meanings[PropertyData.PITCHCENTS] : "Cents",
    property_data_meanings[PropertyData.FREQUENCY] : "Frequency",
}


class WObjectType():
    OBJECTBASE = 1
    WWISEOBJECT = 2

class PropValueType():
    MIN = 1
    MAX = 2
    DEFAULTVALUE = 3

class WObject(object):
    obj_type = None
    name = None
    display_group = None
    property_list = [] # Property list of only this WObject
    full_property_list = [] # Property list of this WObject and its subchildren  WObjects
    object_base_ref_list = []
    object_base_ref_names_list = []

    def __init__(self, obj_name):
        self.name = obj_name
        self.property_list = []
        self.full_property_list = []
        self.object_base_ref_list = []
        self.object_base_ref_names_list = []
        self.display_group = None

class Property(object): # Also includes references
    name = None
    type = None
    display_name = None
    display_group = None
    data_meaning = None # The units of the data (Ex: Decibel)
    data_range = {PropValueType.MIN: None, PropValueType.MAX: None}
    data_default_value = None
    is_visible = True
    enumeration = None # Property value restriction enumeration.
    is_reference = False
    not_null_restruction = False # Has the 'cannot be null' restriction.
    type_enumeration_restriction = None
    category_enumeration_restriction = None

    def __init__(self, prop_name, prop_type):
        self.name = prop_name
        self.type = prop_type
        self.display_name = None
        self.display_group = None
        self.data_meaning = None
        self.data_range = {PropValueType.MIN: None, PropValueType.MAX: None}
        self.data_default_value = None
        self.is_visible = True
        self.enumeration = None
        self.is_reference = False
        self.not_null_restruction = False
        self.type_enumeration_restriction = None
        self.category_enumeration_restriction = None

def parse_property(xml_prop, is_reference):
    prop_name = ""
    if 'DocId' in xml_prop.attrib:
        prop_name = xml_prop.attrib['DocId']
    else:
        prop_name = xml_prop.attrib['Name']

    prop_type = ""
    if 'Type' in xml_prop.attrib:
        prop_type = xml_prop.attrib['Type']

    prop = Property(prop_name, prop_type)

    if 'DisplayName' in xml_prop.attrib:
        prop.display_name = xml_prop.attrib['DisplayName']
    else: #Because sometimes the displayName is in UserInterface
        for child in xml_prop:
            if child.tag == "UserInterface":
                if 'DisplayName' in child.attrib:
                    prop.display_name = child.attrib['DisplayName']

    if 'DisplayGroup' in xml_prop.attrib:
        prop.display_group = xml_prop.attrib['DisplayGroup']

    if 'DataMeaning' in xml_prop.attrib:
        prop.data_meaning = xml_prop.attrib['DataMeaning']

    if 'IsVisible' in xml_prop.attrib and xml_prop.attrib['IsVisible'] == 'false':
        prop.is_visible = False

    if is_reference:
        prop.is_reference = True
        parse_reference_children(list(xml_prop), prop)
    else:
        prop.is_reference = False
        parse_property_children(list(xml_prop), prop)

    return prop

def parse_property_children(xml_children, prop):
    for child in xml_children:
        if child.tag == 'DefaultValue':
            prop.data_default_value = child.text
        if child.tag == 'Restrictions':
            for restriction_child in child:
                if restriction_child.tag == 'ValueRestriction':
                    for value_restriction_child in restriction_child:
                        if value_restriction_child.tag == "Range":
                            for range_child in value_restriction_child:
                                if range_child.tag == "Min":
                                    prop.data_range[PropValueType.MIN] = range_child.text
                                elif range_child.tag == "Max":
                                    prop.data_range[PropValueType.MAX] = range_child.text
                        if value_restriction_child.tag == "Enumeration":
                            prop.enumeration = {}
                            for enum_value in value_restriction_child:
                                if enum_value.tag == "Value" and "DisplayName" in enum_value.attrib:
                                    value_display_name = enum_value.attrib["DisplayName"]
                                    prop.enumeration[int(enum_value.text)] = value_display_name

def parse_reference_children(xml_children, prop):
    for child in xml_children:
        if child.tag == 'Restrictions':
            for restriction_child in child._children:
                if restriction_child.tag == 'NotNullRestriction':
                    prop.not_null_restruction = True
                elif restriction_child.tag == 'TypeEnumerationRestriction':
                    prop.type_enumeration_restriction = []
                    for enum_restriction_value in restriction_child._children:
                        if 'Name' in enum_restriction_value.attrib and enum_restriction_value.tag == 'Type':
                            prop.type_enumeration_restriction.append(enum_restriction_value.attrib['Name'])
                elif restriction_child.tag == 'CategoryEnumerationRestriction':
                    prop.category_enumeration_restriction = []
                    for category_restriction_value in restriction_child._children:
                        if 'Name' in category_restriction_value.attrib and category_restriction_value.tag == 'Category':
                            prop.category_enumeration_restriction.append(category_restriction_value.attrib['Name'])

def parse_wobject(xml_wobject):
    wobject = WObject(xml_wobject.attrib['Name'])
    if xml_wobject.tag == "ObjectBase":
        wobject.objType = WObjectType.OBJECTBASE
    elif xml_wobject.tag == "WwiseObject":
        wobject.objType = WObjectType.WWISEOBJECT

    if 'DisplayGroup' in xml_wobject.attrib:
        wobject.display_group = xml_wobject.attrib['DisplayGroup']

    for child in xml_wobject._children:
        if child.tag == "Properties":
            parse_properties(wobject, child._children)
        elif child.tag == "InnerTypes":
            parse_inner_types(wobject, child._children)
        elif child.tag == "ObjectBaseRef":
            wobject.object_base_ref_names_list.append(child.attrib['Name']) 

    return wobject

def parse_inner_types(wobject, children):
    for child in filter(lambda x: x.tag == "InnerType", children):
        for grandkid in filter(lambda x: x.tag == "Properties", child._children):
            parse_properties(wobject, grandkid._children)
	
def parse_properties(wobject, children):
    for xmlProp in filter(lambda x: x.tag == "Property" or x.tag == "Reference", children):
        prop = parse_property(xmlProp, xmlProp.tag == "Reference")
        if prop.display_group is None:
            prop.display_group = wobject.display_group
        if prop.is_visible:
            wobject.property_list.append(prop)

def fill_wobject_base_ref_lists(wobj_list):
    for wobj in wobj_list:
        for base_ref_name in wobj.object_base_ref_names_list:
            for base_ref in wobj_list:
                if base_ref_name == base_ref.name and base_ref.objType == WObjectType.OBJECTBASE:
                    base_ref_copy = copy.copy(base_ref)
                    wobj.object_base_ref_list.append(base_ref_copy)
    return wobj_list

# For every WObject in the list, fill its fullPropertyList
# attribute with the properties of the WObject
# and of all of its subchildren.
def fill_full_property_lists(wobjects_list):
    for wobj in wobjects_list:
        full_prop_list = []
        append_all_children_properties(wobj, full_prop_list)
        full_prop_list = list(OrderedDict.fromkeys(full_prop_list)) # To get rid of duplicates
        full_prop_list.sort(key=lambda x: x.display_group, reverse=False)
        wobj.full_property_list = full_prop_list
    return wobjects_list

# Fills the fullPropertyList of a WObject
def append_all_children_properties(wobject, full_prop_list):
    for prop in wobject.property_list:
        full_prop_list.append(prop)
    for childNode in wobject.object_base_ref_list:
        append_all_children_properties(childNode, full_prop_list)

# Parses plugin xml file
def create_plugin_wobjects(file_path):
    plugin_list = []
    plugin_xml_tree = ET.parse(file_path)

    xml_plugins = list(plugin_xml_tree.getroot())
    for xml_plugin in xml_plugins:
        if 'Name' in xml_plugin.attrib:
            plugin = parse_plugin_xml_element(xml_plugin, plugin_list)
            plugin_list.append(plugin)
    return plugin_list

def parse_plugin_xml_element(xml_plugin, plugin_list):
    plugin = WObject(xml_plugin.attrib['Name'])
    plugin.objType = WObjectType.WWISEOBJECT # A plugin is treated as a wwise object
    if 'DisplayGroup' in xml_plugin.attrib:
        plugin.displayGroup = xml_plugin.attrib['displayGroup']
    for child in xml_plugin:
        if child.tag == "Properties":
            parse_properties(plugin, list(child))
        elif child.tag == "InnerTypes":
            parse_inner_types(plugin, list(child))
    return plugin