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

#include "Metadata/WwiseMetadataBasicReference.h"
#include "Metadata/WwiseMetadataLoader.h"

WwiseMetadataBasicReference::WwiseMetadataBasicReference()
{
	WWISE_DB_LOG(Error, "Using default WwiseMetadataBasicReference");
}

WwiseMetadataBasicReference::WwiseMetadataBasicReference(WwiseMetadataLoader& Loader) :
	Id(Loader.GetWwiseShortId(this, "Id"_wwise_db)),
	Name(Loader.GetString(this, "Name"_wwise_db)),
	ObjectPath(Loader.GetString(this, "ObjectPath"_wwise_db)),
	Color(Loader.GetInt(this, "Color"_wwise_db, WwiseRequiredMetadata::Optional)),
	GUID(Loader.GetGuid(this, "GUID"_wwise_db))
{
	Loader.LogParsed("BasicReference"_wwise_db, Id, Name);
}
