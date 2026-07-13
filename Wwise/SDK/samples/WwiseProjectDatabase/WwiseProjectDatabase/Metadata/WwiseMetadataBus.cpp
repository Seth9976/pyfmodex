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

#include "Metadata/WwiseMetadataBus.h"
#include "Metadata/WwiseMetadataLoader.h"
#include "Metadata/WwiseMetadataPluginGroup.h"

WwiseMetadataBusReference::WwiseMetadataBusReference(WwiseMetadataLoader& Loader) :
	WwiseMetadataBasicReference(Loader)
{
	Loader.LogParsed("BusReference"_wwise_db, Id, Name);
}

WwiseMetadataBus::WwiseMetadataBus(WwiseMetadataLoader& Loader) :
	WwiseMetadataBusReference(Loader),
	PluginRefs(Loader.GetObjectPtr<WwiseMetadataPluginReferenceGroup>(this, "PluginRefs"_wwise_db)),
	AuxBusRefs(Loader.GetArray<WwiseMetadataBusReference>(this, "AuxBusRefs"_wwise_db)),
	MaxAttenuation(Loader.GetFloat(this, "MaxAttenuation"_wwise_db, WwiseRequiredMetadata::Optional))
{
	Loader.LogParsed("Bus"_wwise_db, Id, Name);
}
