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

#include "Ref/WwiseRefMedia.h"
#include "Metadata/WwiseMetadataMedia.h"
#include "Metadata/WwiseMetadataSoundBank.h"

const WwiseDBString WwiseRefMedia::NAME = "Media"_wwise_db;

const WwiseMetadataMedia* WwiseRefMedia::GetMedia() const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return nullptr;
	}

	const auto& Media = SoundBank->Media;
	if (Media.IsValidIndex(MediaIndex))
	{
		return &Media[MediaIndex];
	}
	else
	{
		WWISE_DB_LOG(Error, "Could not get Media index #%zu", MediaIndex);
		return nullptr;
	}
}

WwiseDBShortId WwiseRefMedia::MediaId() const
{
	const auto* Media = GetMedia();
	if (!Media) [[unlikely]]
	{
		return 0;
	}
	return Media->Id;
}

const WwiseDBString* WwiseRefMedia::MediaShortName() const
{
	const auto* Media = GetMedia();
	if (!Media) [[unlikely]]
	{
		return {};
	}
	return &Media->ShortName;
}

const WwiseDBString* WwiseRefMedia::MediaPath() const
{
	const auto* Media = GetMedia();
	if (!Media) [[unlikely]]
	{
		return {};
	}
	return &Media->Path;
}

WwiseDBShortId WwiseRefMedia::Hash() const
{
	auto Result = WwiseRefSoundBank::Hash();
	Result = WwiseDBHashCombine(Result, GetTypeHash(MediaIndex));
	return Result;
}
