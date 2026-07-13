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

#include "Ref/WwiseRefAuxBus.h"

#include "Metadata/WwiseMetadataBus.h"
#include "Metadata/WwiseMetadataSoundBank.h"
#include "Ref/WwiseRefAudioDevice.h"
#include "Ref/WwiseRefCustomPlugin.h"
#include "Ref/WwiseRefPluginShareSet.h"
#include <inttypes.h>

const WwiseDBString WwiseRefAuxBus::NAME = "AuxBus"_wwise_db;

const WwiseMetadataBus* WwiseRefAuxBus::GetAuxBus() const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return nullptr;
	}
	const auto& AuxBusses = SoundBank->AuxBusses;
	if (AuxBusses.IsValidIndex(AuxBusIndex))
	{
		return &AuxBusses.Array[AuxBusIndex];
	}
	else
	{
		WWISE_DB_LOG(Error, "Could not get AuxBus index #%zu", AuxBusIndex);
		return nullptr;
	}
}

void WwiseRefAuxBus::GetAllAuxBusRefs(WwiseDBSet<const WwiseRefAuxBus*>& OutAuxBusRefs, const WwiseAuxBusGlobalIdsMap& InGlobalMap) const
{
	bool bIsAlreadyInSet = false;
	OutAuxBusRefs.Add(this, &bIsAlreadyInSet);
	if (bIsAlreadyInSet) [[unlikely]]	// Unlikely but can still be done (circular references are possible in Aux Busses)
	{
		return;
	}

	const auto* AuxBus = GetAuxBus();
	if (!AuxBus) [[unlikely]]
	{
		return;
	}
	for (const auto& SubAuxBus : AuxBus->AuxBusRefs)
	{
		const WwiseRefAuxBus* SubAuxBusRef = InGlobalMap.Find(WwiseDatabaseLocalizableIdKey(SubAuxBus.Id, LanguageId));
		if (!SubAuxBusRef) [[unlikely]]
		{
			SubAuxBusRef = InGlobalMap.Find(WwiseDatabaseLocalizableIdKey(SubAuxBus.Id, 0));
		}
		if (!SubAuxBusRef) [[unlikely]]
		{
			WWISE_DB_LOG(Error, "Could not get Aux Bus Id %" PRIu32, SubAuxBus.Id);
			continue;
		}
		SubAuxBusRef->GetAllAuxBusRefs(OutAuxBusRefs, InGlobalMap);
	}
}

WwiseCustomPluginIdsMap WwiseRefAuxBus::GetAuxBusCustomPlugins(const WwiseCustomPluginGlobalIdsMap& GlobalMap) const
{
	const auto* AuxBus = GetAuxBus();
	if (!AuxBus || !AuxBus->PluginRefs)
	{
		return {};
	}
	const auto& Plugins = AuxBus->PluginRefs->Custom;
	WwiseCustomPluginIdsMap Result;
	Result.Empty(Plugins.Size());
	for (const auto& Elem : Plugins)
	{
		WwiseDatabaseLocalizableIdKey Id(Elem.Id, LanguageId);
		const WwiseRefCustomPlugin* GlobalRef = GlobalMap.Find(Id);
		if (GlobalRef)
		{
			Result.Add(Elem.Id, *GlobalRef);
		}
	}
	return Result;
}

WwisePluginShareSetIdsMap WwiseRefAuxBus::GetAuxBusPluginShareSets(const WwisePluginShareSetGlobalIdsMap& GlobalMap) const
{
	const auto* AuxBus = GetAuxBus();
	if (!AuxBus || !AuxBus->PluginRefs)
	{
		return {};
	}
	const auto& Plugins = AuxBus->PluginRefs->ShareSets;
	WwisePluginShareSetIdsMap Result;
	Result.Empty(Plugins.Size());
	for (const auto& Elem : Plugins.Array)
	{
		WwiseDatabaseLocalizableIdKey Id(Elem.Id, LanguageId);
		const WwiseRefPluginShareSet* GlobalRef = GlobalMap.Find(Id);
		if (GlobalRef)
		{
			Result.Add(Elem.Id, *GlobalRef);
		}
	}
	return Result;
}

WwiseAudioDeviceIdsMap WwiseRefAuxBus::GetAuxBusAudioDevices(const WwiseAudioDeviceGlobalIdsMap& GlobalMap) const
{
	const auto* AuxBus = GetAuxBus();
	if (!AuxBus || !AuxBus->PluginRefs)
	{
		return {};
	}
	const auto& Plugins = AuxBus->PluginRefs->AudioDevices;
	WwiseAudioDeviceIdsMap Result;
	Result.Empty(Plugins.Size());
	for (const auto& Elem : Plugins.Array)
	{
		WwiseDatabaseLocalizableIdKey Id(Elem.Id, LanguageId);
		const WwiseRefAudioDevice* GlobalRef = GlobalMap.Find(Id);
		if (GlobalRef)
		{
			Result.Add(Elem.Id, *GlobalRef);
		}
	}
	return Result;
}

WwiseDBShortId WwiseRefAuxBus::AuxBusId() const
{
	const auto* AuxBus = GetAuxBus();
	if (!AuxBus) [[unlikely]]
	{
		return 0;
	}
	return AuxBus->Id;
}

const WwiseDBGuid* WwiseRefAuxBus::AuxBusGuid() const
{
	const auto* AuxBus = GetAuxBus();
	if (!AuxBus) [[unlikely]]
	{
		return {};
	}
	return &AuxBus->GUID;
}

const WwiseDBString* WwiseRefAuxBus::AuxBusName() const
{
	const auto* AuxBus = GetAuxBus();
	if (!AuxBus) [[unlikely]]
	{
		return &WwiseDBString::Empty;
	}
	return &AuxBus->Name;
}

const WwiseDBString* WwiseRefAuxBus::AuxBusObjectPath() const
{
	const auto* AuxBus = GetAuxBus();
	if (!AuxBus) [[unlikely]]
	{
		return &WwiseDBString::Empty;
	}
	return &AuxBus->ObjectPath;
}

WwiseDBShortId WwiseRefAuxBus::Hash() const
{
	auto Result = WwiseRefSoundBank::Hash();
	Result = WwiseDBHashCombine(Result, GetTypeHash(AuxBusIndex));
	return Result;
}
