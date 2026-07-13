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

#include "Metadata/WwiseMetadataPlatformInfo.h"
#include "Metadata/WwiseMetadataLoader.h"

WwiseMetadataPlatformInfo::WwiseMetadataPlatformInfo(WwiseMetadataLoader& Loader) :
	Platform(Loader.GetLoaderObject<WwiseMetadataPlatform>(this, "Platform"_wwise_db)),
	RootPaths(Loader.GetLoaderObject<WwiseMetadataRootPaths>(this, "RootPaths"_wwise_db)),
	DefaultAlign(Loader.GetWwiseShortId(this, "DefaultAlign"_wwise_db)),
	Settings(Loader.GetLoaderObject<WwiseMetadataSettings>(this, "Settings"_wwise_db)),
	FileHash(Loader.GetGuid(this, "FileHash"_wwise_db))
{
	Loader.LogParsed("PlatformInfo"_wwise_db);
}
