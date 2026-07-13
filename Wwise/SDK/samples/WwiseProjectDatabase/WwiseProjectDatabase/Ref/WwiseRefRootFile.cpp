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

#include "Ref/WwiseRefRootFile.h"

const WwiseDBString WwiseRefRootFile::NAME = "RootFile"_wwise_db;

WwiseDBShortId WwiseRefRootFile::Hash() const
{
	auto Result = WwiseDBHashCombine(GetTypeHash(JsonFilePath), GetTypeHash(static_cast<std::underlying_type<WwiseRefType>::type>(Type())));
	return Result;
}

bool WwiseRefRootFile::IsValid() const
{
	return static_cast<bool>(RootFileRef);
}

const WwiseMetadataRootFile* WwiseRefRootFile::GetRootFile() const
{
	const auto* Result = RootFileRef.Get();
	WWISE_DB_CLOG(!Result, Error, "Could not get Root File Ref");
	return Result;
}
