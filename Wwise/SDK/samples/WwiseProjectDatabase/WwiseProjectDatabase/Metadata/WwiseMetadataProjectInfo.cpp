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

#include "Metadata/WwiseMetadataProjectInfo.h"

#include "Metadata/WwiseMetadataLanguage.h"
#include "Metadata/WwiseMetadataLoader.h"
#include "Metadata/WwiseMetadataPlatform.h"

WwiseMetadataProjectInfo::WwiseMetadataProjectInfo(WwiseMetadataLoader& Loader) :
	Project(Loader.GetLoaderObject<WwiseMetadataProject>(this, "Project"_wwise_db)),
	CacheRoot(Loader.GetString(this, "CacheRoot"_wwise_db)),
	Platforms(Loader.GetArray<WwiseMetadataPlatformReference>(this, "Platforms"_wwise_db)),
	Languages(Loader.GetArray<WwiseMetadataLanguage>(this, "Languages"_wwise_db)),
	FileHash(Loader.GetGuid(this, "FileHash"_wwise_db))
{
	Loader.LogParsed("ProjectInfo"_wwise_db);
}
