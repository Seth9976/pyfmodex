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

#include "Ref/WwiseRefBus.h"

#include "Metadata/WwiseMetadataBus.h"
#include "Metadata/WwiseMetadataSoundBank.h"

const WwiseDBString WwiseRefBus::NAME = "Bus"_wwise_db;

const WwiseMetadataBus* WwiseRefBus::GetBus() const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return nullptr;
	}
	const auto& Busses = SoundBank->Busses;
	if (Busses.IsValidIndex(BusIndex))
	{
		return &Busses[BusIndex];
	}
	else
	{
		WWISE_DB_LOG(Error, "Could not get Bus index #%zu", BusIndex);
		return nullptr;
	}
}

WwiseDBShortId WwiseRefBus::BusId() const
{
	const auto* Bus = GetBus();
	if (!Bus) [[unlikely]]
	{
		return 0;
	}
	return Bus->Id;
}

const WwiseDBGuid* WwiseRefBus::BusGuid() const
{
	const auto* Bus = GetBus();
	if (!Bus) [[unlikely]]
	{
		return {};
	}
	return &Bus->GUID;
}

const WwiseDBString* WwiseRefBus::BusName() const
{
	const auto* Bus = GetBus();
	if (!Bus) [[unlikely]]
	{
		return &WwiseDBString::Empty;
	}
	return &Bus->Name;
}

const WwiseDBString* WwiseRefBus::BusObjectPath() const
{
	const auto* Bus = GetBus();
	if (!Bus) [[unlikely]]
	{
		return &WwiseDBString::Empty;
	}
	return &Bus->ObjectPath;
}

WwiseDBShortId WwiseRefBus::Hash() const
{
	auto Result = WwiseRefSoundBank::Hash();
	Result = WwiseDBHashCombine(Result, GetTypeHash(BusIndex));
	return Result;
}
