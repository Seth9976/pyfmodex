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

#include "Ref/WwiseRefState.h"
#include "Metadata/WwiseMetadataStateGroup.h"

#include "Metadata/WwiseMetadataState.h"

const WwiseDBString WwiseRefState::NAME = "State"_wwise_db;

const WwiseMetadataState* WwiseRefState::GetState() const
{
	const auto* StateGroup = GetStateGroup();
	if (!StateGroup) [[unlikely]]
	{
		return nullptr;
	}
	const auto& States = StateGroup->States;
	if (States.IsValidIndex(StateIndex))
	{
		return &States[StateIndex];
	}
	else
	{
		WWISE_DB_LOG(Error, "Could not get State index #%zu", StateIndex);
		return nullptr;
	}
}

WwiseDBShortId WwiseRefState::StateId() const
{
	const auto* State = GetState();
	if (!State) [[unlikely]]
	{
		return 0;
	}
	return State->Id;
}

const WwiseDBGuid* WwiseRefState::StateGuid() const
{
	const auto* State = GetState();
	if (!State) [[unlikely]]
	{
		return {};
	}
	return &State->GUID;
}

const WwiseDBString* WwiseRefState::StateName() const
{
	const auto* State = GetState();
	if (!State) [[unlikely]]
	{
		return {};
	}
	return &State->Name;
}

const WwiseDBString* WwiseRefState::StateObjectPath() const
{
	const auto* State = GetState();
	if (!State) [[unlikely]]
	{
		return {};
	}
	return &State->ObjectPath;
}

WwiseDBShortId WwiseRefState::Hash() const
{
	auto Result = WwiseRefStateGroup::Hash();
	Result = WwiseDBHashCombine(Result, GetTypeHash(StateIndex));
	return Result;
}
