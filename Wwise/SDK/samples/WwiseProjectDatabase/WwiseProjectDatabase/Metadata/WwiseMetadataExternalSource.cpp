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

#include "Metadata/WwiseMetadataExternalSource.h"
#include "Metadata/WwiseMetadataLoader.h"

WwiseMetadataExternalSourceReference::WwiseMetadataExternalSourceReference(WwiseMetadataLoader& Loader) :
	Cookie(Loader.GetWwiseShortId(this, "Cookie"_wwise_db))
{
	Loader.LogParsed("ExternalSourceReference"_wwise_db, Cookie);
}

WwiseMetadataExternalSource::WwiseMetadataExternalSource(WwiseMetadataLoader& Loader) :
	WwiseMetadataExternalSourceReference(Loader),
	Name(Loader.GetString(this, "Name"_wwise_db)),
	ObjectPath(Loader.GetString(this, "ObjectPath"_wwise_db)),
	GUID(Loader.GetGuid(this, "GUID"_wwise_db))
{
	Loader.LogParsed("ExternalSource"_wwise_db, Cookie, Name);
}
