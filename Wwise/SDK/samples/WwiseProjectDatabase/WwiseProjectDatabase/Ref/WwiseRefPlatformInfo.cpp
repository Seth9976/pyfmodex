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

#include "Ref/WwiseRefPlatformInfo.h"

#include "Metadata/WwiseMetadataRootFile.h"

const WwiseDBString WwiseRefPlatformInfo::NAME = "PlatformInfo"_wwise_db;

const WwiseMetadataPlatformInfo* WwiseRefPlatformInfo::GetPlatformInfo() const
{
	const auto* RootFile = GetRootFile();
	if (!RootFile) [[unlikely]]
	{
		return nullptr;
	}
	WWISE_DB_CLOG(!RootFile->PlatformInfo, Error, "Could not get PlatformInfo");
	return RootFile->PlatformInfo;
}
