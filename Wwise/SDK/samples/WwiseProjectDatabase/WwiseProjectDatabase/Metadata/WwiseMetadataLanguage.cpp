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

#include "Metadata/WwiseMetadataLanguage.h"
#include "Metadata/WwiseMetadataLoader.h"

WwiseMetadataLanguageAttributes::WwiseMetadataLanguageAttributes(WwiseMetadataLoader& Loader) :
	Name(Loader.GetString(this, "Name"_wwise_db)),
	Id(Loader.GetWwiseShortId(this, "Id"_wwise_db)),
	GUID(Loader.GetGuid(this, "GUID"_wwise_db)),
	bDefault(Loader.GetBool(this, "Default"_wwise_db, WwiseRequiredMetadata::Optional)),
	bUseAsStandIn(Loader.GetBool(this, "UseAsStandIn"_wwise_db, WwiseRequiredMetadata::Optional))
{
	Loader.LogParsed("LanguageAttributes"_wwise_db, Id, Name);
}

WwiseMetadataLanguage::WwiseMetadataLanguage(WwiseMetadataLoader& Loader) :
	WwiseMetadataLanguageAttributes(Loader)
{
	Loader.LogParsed("Language"_wwise_db, Id, Name);
}
