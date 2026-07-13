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

#include "Ref/WwiseRefPluginShareSet.h"

#include "Metadata/WwiseMetadataPlugin.h"
#include "Metadata/WwiseMetadataPluginGroup.h"
#include "Metadata/WwiseMetadataSoundBank.h"
#include "Ref/WwiseRefAudioDevice.h"
#include "Ref/WwiseRefCustomPlugin.h"
#include "Ref/WwiseRefMedia.h"

const WwiseDBString WwiseRefPluginShareSet::NAME = "PluginShareSet"_wwise_db;

const WwiseMetadataPlugin* WwiseRefPluginShareSet::GetPlugin() const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank || !SoundBank->Plugins) [[unlikely]]
	{
		return nullptr;
	}

	const auto& Plugins = SoundBank->Plugins->ShareSets;
	if (Plugins.IsValidIndex(PluginShareSetIndex))
	{
		return &Plugins[PluginShareSetIndex];
	}
	else
	{
		WWISE_DB_LOG(Error, "Could not get Plugin ShareSet index #%zu", PluginShareSetIndex);
		return nullptr;
	}
}

WwiseMediaIdsMap WwiseRefPluginShareSet::GetPluginMedia(const WwiseMediaGlobalIdsMap& GlobalMap) const
{
	const auto* PluginShareSet = GetPlugin();
	const auto* SoundBank = GetSoundBank();
	if (!PluginShareSet || !SoundBank) [[unlikely]]
	{
		return {};
	}
	const auto& Media = PluginShareSet->MediaRefs;

	WwiseMediaIdsMap Result;
	Result.Empty(Media.Size());
	for (const auto& Elem : Media)
	{
		WwiseDatabaseMediaIdKey Id(Elem.Id, SoundBank->Id);

		const WwiseRefMedia* MediaInGlobalMap = GlobalMap.Find(Id);
		if (MediaInGlobalMap)
		{
			Result.Add(Elem.Id, *MediaInGlobalMap);
		}
	}
	return Result;
}

WwiseCustomPluginIdsMap WwiseRefPluginShareSet::GetPluginCustomPlugins(const WwiseCustomPluginGlobalIdsMap& GlobalMap) const
{
	const auto* Plugin = GetPlugin();
	if (!Plugin || !Plugin->PluginRefs) [[unlikely]]
	{
		return {};
	}
	const auto& Plugins = Plugin->PluginRefs->Custom;
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

WwisePluginShareSetIdsMap WwiseRefPluginShareSet::GetPluginPluginShareSets(const WwisePluginShareSetGlobalIdsMap& GlobalMap) const
{
	const auto* Plugin = GetPlugin();
	if (!Plugin || !Plugin->PluginRefs) [[unlikely]]
	{
		return {};
	}
	const auto& Plugins = Plugin->PluginRefs->ShareSets;
	WwisePluginShareSetIdsMap Result;
	Result.Empty(Plugins.Size());
	for (const auto& Elem : Plugins)
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

WwiseAudioDeviceIdsMap WwiseRefPluginShareSet::GetPluginAudioDevices(const WwiseAudioDeviceGlobalIdsMap& GlobalMap) const
{
	const auto* Plugin = GetPlugin();
	if (!Plugin || !Plugin->PluginRefs) [[unlikely]]
	{
		return {};
	}
	const auto& Plugins = Plugin->PluginRefs->AudioDevices;
	WwiseAudioDeviceIdsMap Result;
	Result.Empty(Plugins.Size());
	for (const auto& Elem : Plugins)
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

WwiseDBShortId WwiseRefPluginShareSet::PluginShareSetId() const
{
	const auto* PluginShareSet = GetPlugin();
	if (!PluginShareSet) [[unlikely]]
	{
		return 0;
	}
	return PluginShareSet->Id;
}

WwiseDBGuid WwiseRefPluginShareSet::PluginShareSetGuid() const
{
	const auto* PluginShareSet = GetPlugin();
	if (!PluginShareSet) [[unlikely]]
	{
		return {};
	}
	return PluginShareSet->GUID;
}

const WwiseDBString* WwiseRefPluginShareSet::PluginShareSetName() const
{
	const auto* PluginShareSet = GetPlugin();
	if (!PluginShareSet) [[unlikely]]
	{
		return &WwiseDBString::Empty;
	}
	return &PluginShareSet->Name;
}

const WwiseDBString* WwiseRefPluginShareSet::PluginShareSetObjectPath() const
{
	const auto* PluginShareSet = GetPlugin();
	if (!PluginShareSet) [[unlikely]]
	{
		return &WwiseDBString::Empty;
	}
	return &PluginShareSet->ObjectPath;
}

WwiseDBShortId WwiseRefPluginShareSet::Hash() const
{
	auto Result = WwiseRefSoundBank::Hash();
	Result = WwiseDBHashCombine(Result, GetTypeHash(PluginShareSetIndex));
	return Result;
}
