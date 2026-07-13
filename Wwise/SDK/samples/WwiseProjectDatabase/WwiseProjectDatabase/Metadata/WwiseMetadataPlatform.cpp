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

#include "Metadata/WwiseMetadataPlatform.h"
#include "Metadata/WwiseMetadataLoader.h"

WwiseMetadataPlatformAttributes::WwiseMetadataPlatformAttributes()
{
	WWISE_DB_LOG(Error, "Using default WwiseMetadataPlatformAttributes");
}

WwiseMetadataPlatformAttributes::WwiseMetadataPlatformAttributes(WwiseMetadataLoader& Loader) :
	Name(Loader.GetString(this, "Name"_wwise_db)),
	BasePlatform(Loader.GetString(this, "BasePlatform"_wwise_db)),
	Generator(Loader.GetString(this, "Generator"_wwise_db))
{
	Loader.LogParsed("PlatformAttributes"_wwise_db, 0, Name);
}

WwiseMetadataPlatformReference::WwiseMetadataPlatformReference(WwiseMetadataLoader& Loader) :
	Name(Loader.GetString(this, "Name"_wwise_db)),
	GUID(Loader.GetGuid(this, "GUID"_wwise_db)),
	BasePlatform(Loader.GetString(this, "BasePlatform"_wwise_db)),
	BasePlatformGUID(Loader.GetGuid(this, "BasePlatformGUID"_wwise_db)),
	Path(Loader.GetString(this, "Path"_wwise_db))
{
	Loader.LogParsed("PlatformReference"_wwise_db, 0, Name);
}

WwiseMetadataPlatform::WwiseMetadataPlatform()
{
	WWISE_DB_LOG(Error, "Using default WwiseMetadataPlatform");
}

WwiseMetadataPlatform::WwiseMetadataPlatform(WwiseMetadataLoader& Loader) :
	WwiseMetadataPlatformAttributes(Loader)
{
	Loader.LogParsed("Platform"_wwise_db, 0, Name);
}
