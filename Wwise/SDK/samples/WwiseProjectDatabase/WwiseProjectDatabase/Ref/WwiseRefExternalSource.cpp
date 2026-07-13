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


#include "Ref/WwiseRefExternalSource.h"
#include "Metadata/WwiseMetadataExternalSource.h"
#include "Metadata/WwiseMetadataSoundBank.h"

const WwiseDBString WwiseRefExternalSource::NAME = "ExternalSource"_wwise_db;

const WwiseMetadataExternalSource* WwiseRefExternalSource::GetExternalSource() const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return nullptr;
	}
	const auto& ExternalSources = SoundBank->ExternalSources;
	if (ExternalSources.IsValidIndex(ExternalSourceIndex))
	{
		return &ExternalSources[ExternalSourceIndex];
	}
	else
	{
		WWISE_DB_LOG(Error, "Could not get External Source index #%zu", ExternalSourceIndex);
		return nullptr;
	}
}

WwiseDBShortId WwiseRefExternalSource::ExternalSourceCookie() const
{
	const auto* ExternalSource = GetExternalSource();
	if (!ExternalSource) [[unlikely]]
	{
		return {};
	}
	return ExternalSource->Cookie;
}

WwiseDBGuid WwiseRefExternalSource::ExternalSourceGuid() const
{
	const auto* ExternalSource = GetExternalSource();
	if (!ExternalSource) [[unlikely]]
	{
		return {};
	}
	return ExternalSource->GUID;
}

const WwiseDBString* WwiseRefExternalSource::ExternalSourceName() const
{
	const auto* ExternalSource = GetExternalSource();
	if (!ExternalSource) [[unlikely]]
	{
		return &WwiseDBString::Empty;
	}
	return &ExternalSource->Name;
}

const WwiseDBString* WwiseRefExternalSource::ExternalSourceObjectPath() const
{
	const auto* ExternalSource = GetExternalSource();
	if (!ExternalSource) [[unlikely]]
	{
		return &WwiseDBString::Empty;
	}
	return &ExternalSource->ObjectPath;
}

WwiseDBShortId WwiseRefExternalSource::Hash() const
{
	auto Result = WwiseRefSoundBank::Hash();
	Result = WwiseDBHashCombine(Result, GetTypeHash(ExternalSourceIndex));
	return Result;
}
