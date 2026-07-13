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

#include "Ref/WwiseRefTrigger.h"

#include "Metadata/WwiseMetadataSoundBank.h"
#include "Metadata/WwiseMetadataTrigger.h"

const WwiseDBString WwiseRefTrigger::NAME = "Trigger"_wwise_db;

const WwiseMetadataTrigger* WwiseRefTrigger::GetTrigger() const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return nullptr;
	}
	const auto& Triggers = SoundBank->Triggers;
	if (Triggers.IsValidIndex(TriggerIndex))
	{
		return &Triggers[TriggerIndex];
	}
	else
	{
		WWISE_DB_LOG(Error, "Could not get Trigger index #%zu", TriggerIndex);
		return nullptr;
	}
}

WwiseDBShortId WwiseRefTrigger::TriggerId() const
{
	const auto* Trigger = GetTrigger();
	if (!Trigger) [[unlikely]]
	{
		return 0;
	}
	return Trigger->Id;
}

const WwiseDBGuid* WwiseRefTrigger::TriggerGuid() const
{
	const auto* Trigger = GetTrigger();
	if (!Trigger) [[unlikely]]
	{
		return {};
	}
	return &Trigger->GUID;
}

const WwiseDBString* WwiseRefTrigger::TriggerName() const
{
	const auto* Trigger = GetTrigger();
	if (!Trigger) [[unlikely]]
	{
		return &WwiseDBString::Empty;
	}
	return &Trigger->Name;
}

const WwiseDBString* WwiseRefTrigger::TriggerObjectPath() const
{
	const auto* Trigger = GetTrigger();
	if (!Trigger) [[unlikely]]
	{
		return &WwiseDBString::Empty;
	}
	return &Trigger->ObjectPath;
}

WwiseDBShortId WwiseRefTrigger::Hash() const
{
	auto Result = WwiseRefSoundBank::Hash();
	Result = WwiseDBHashCombine(Result, GetTypeHash(TriggerIndex));
	return Result;
}
