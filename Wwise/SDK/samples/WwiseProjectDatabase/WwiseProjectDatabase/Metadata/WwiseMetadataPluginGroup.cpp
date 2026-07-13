/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2026 Audiokinetic Inc.
*******************************************************************************/

#include "Metadata/WwiseMetadataPluginGroup.h"
#include "Metadata/WwiseMetadataLoader.h"

WwiseMetadataPluginReferenceGroup::WwiseMetadataPluginReferenceGroup(WwiseMetadataLoader& Loader) :
	Custom(Loader.GetArray<WwiseMetadataPluginReference>(this, "Custom"_wwise_db)),
	ShareSets(Loader.GetArray<WwiseMetadataPluginReference>(this, "ShareSets"_wwise_db)),
	AudioDevices(Loader.GetArray<WwiseMetadataPluginReference>(this, "AudioDevices"_wwise_db))
{
	Loader.LogParsed("PluginReferenceGroup"_wwise_db);
}

WwiseMetadataPluginGroup::WwiseMetadataPluginGroup(WwiseMetadataLoader& Loader) :
	Custom(Loader.GetArray<WwiseMetadataPlugin>(this, "Custom"_wwise_db)),
	ShareSets(Loader.GetArray<WwiseMetadataPlugin>(this, "ShareSets"_wwise_db)),
	AudioDevices(Loader.GetArray<WwiseMetadataPlugin>(this, "AudioDevices"_wwise_db))
{
	Loader.LogParsed("PluginGroup"_wwise_db);
}
