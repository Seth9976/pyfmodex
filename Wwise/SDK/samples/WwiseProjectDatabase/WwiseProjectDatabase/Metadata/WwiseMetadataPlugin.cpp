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

#include "Metadata/WwiseMetadataPlugin.h"
#include "Metadata/WwiseMetadataPluginGroup.h"
#include "Metadata/WwiseMetadataLoader.h"

WwiseMetadataPluginReference::WwiseMetadataPluginReference(WwiseMetadataLoader& Loader) :
	Id(Loader.GetWwiseShortId(this, "Id"_wwise_db))
{
	Loader.LogParsed("PluginReference"_wwise_db, Id);
}

WwiseMetadataPluginAttributes::WwiseMetadataPluginAttributes(WwiseMetadataLoader& Loader) :
	WwiseMetadataBasicReference(Loader),
	LibName(Loader.GetString(this, "LibName"_wwise_db)),
	LibId(Loader.GetWwiseShortId(this, "LibId"_wwise_db))
{
	Loader.LogParsed("PluginAttributes"_wwise_db, Id, Name);
}

WwiseMetadataPlugin::WwiseMetadataPlugin(WwiseMetadataLoader& Loader) :
	WwiseMetadataPluginAttributes(Loader),
	MediaRefs(Loader.GetArray<WwiseMetadataMediaReference>(this, "MediaRefs"_wwise_db)),
	PluginRefs(Loader.GetObjectPtr<WwiseMetadataPluginReferenceGroup>(this, "PluginRefs"_wwise_db))
{
	Loader.LogParsed("Plugin"_wwise_db, Id, Name);
}
