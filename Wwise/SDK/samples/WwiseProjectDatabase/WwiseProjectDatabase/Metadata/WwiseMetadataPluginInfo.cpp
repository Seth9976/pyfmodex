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

#include "Metadata/WwiseMetadataPluginInfo.h"
#include "Metadata/WwiseMetadataLoader.h"

WwiseMetadataPluginInfoAttributes::WwiseMetadataPluginInfoAttributes(WwiseMetadataLoader& Loader) :
	Platform(Loader.GetString(this, "Platform"_wwise_db)),
	BasePlatform(Loader.GetString(this, "BasePlatform"_wwise_db))
{
	Loader.LogParsed("PluginInfoAttributes"_wwise_db);
}

WwiseMetadataPluginInfo::WwiseMetadataPluginInfo(WwiseMetadataLoader& Loader) :
	WwiseMetadataPluginInfoAttributes(Loader),
	PluginLibs(Loader.GetArray<WwiseMetadataPluginLib>(this, "PluginLibs"_wwise_db)),
	FileHash(Loader.GetGuid(this, "FileHash"_wwise_db))
{
	Loader.LogParsed("PluginInfo"_wwise_db);
}
