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

#include "Ref/WwiseRefGameParameter.h"
#include "Metadata/WwiseMetadataGameParameter.h"
#include "Metadata/WwiseMetadataSoundBank.h"

const WwiseDBString WwiseRefGameParameter::NAME = "GameParameter"_wwise_db;

const WwiseMetadataGameParameter* WwiseRefGameParameter::GetGameParameter() const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return nullptr;
	}
	const auto& GameParameters = SoundBank->GameParameters;
	if (GameParameters.IsValidIndex(GameParameterIndex))
	{
		return &GameParameters[GameParameterIndex];
	}
	else
	{
		WWISE_DB_LOG(Error, "Could not get GameParameter index #%zu", GameParameterIndex);
		return nullptr;
	}
}

WwiseDBShortId WwiseRefGameParameter::GameParameterId() const
{
	const auto* GameParameter = GetGameParameter();
	if (!GameParameter) [[unlikely]]
	{
		return 0;
	}
	return GameParameter->Id;
}

const WwiseDBGuid* WwiseRefGameParameter::GameParameterGuid() const
{
	const auto* GameParameter = GetGameParameter();
	if (!GameParameter) [[unlikely]]
	{
		return {};
	}
	return &GameParameter->GUID;
}

const WwiseDBString* WwiseRefGameParameter::GameParameterName() const
{
	const auto* GameParameter = GetGameParameter();
	if (!GameParameter) [[unlikely]]
	{
		return &WwiseDBString::Empty;
	}
	return &GameParameter->Name;
}

const WwiseDBString* WwiseRefGameParameter::GameParameterObjectPath() const
{
	const auto* GameParameter = GetGameParameter();
	if (!GameParameter) [[unlikely]]
	{
		return &WwiseDBString::Empty;
	}
	return &GameParameter->ObjectPath;
}

WwiseDBShortId WwiseRefGameParameter::Hash() const
{
	auto Result = WwiseRefSoundBank::Hash();
	Result = WwiseDBHashCombine(Result, GetTypeHash(GameParameterIndex));
	return Result;
}
