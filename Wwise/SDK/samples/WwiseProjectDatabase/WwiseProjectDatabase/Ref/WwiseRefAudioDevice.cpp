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

#include "Ref/WwiseRefAudioDevice.h"

#include "Metadata/WwiseMetadataPlugin.h"
#include "Metadata/WwiseMetadataPluginGroup.h"
#include "Metadata/WwiseMetadataSoundBank.h"
#include "Ref/WwiseRefMedia.h"

const WwiseDBString WwiseRefAudioDevice::NAME = "AudioDevice"_wwise_db;

const WwiseMetadataPlugin* WwiseRefAudioDevice::GetPlugin() const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank || !SoundBank->Plugins) [[unlikely]]
	{
		return nullptr;
	}

	const auto& Plugins = SoundBank->Plugins->AudioDevices;
	if (Plugins.IsValidIndex(AudioDeviceIndex))
	{
		return &Plugins.Array[AudioDeviceIndex];
	}
	else
	{
		WWISE_DB_LOG(Error, "Could not get Audio Device index #%zu", AudioDeviceIndex);
		return nullptr;
	}
}

WwiseMediaIdsMap WwiseRefAudioDevice::GetPluginMedia(const WwiseMediaGlobalIdsMap& GlobalMap) const
{
	const auto* AudioDevice = GetPlugin();
	const auto* SoundBank = GetSoundBank();
	if (!AudioDevice || !SoundBank) [[unlikely]]
	{
		return {};
	}
	const auto& Media = AudioDevice->MediaRefs;

	WwiseMediaIdsMap Result;
	Result.Empty(Media.Size());
	for (const auto& Elem : Media.Array)
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

WwiseDBShortId WwiseRefAudioDevice::AudioDeviceId() const
{
	const auto* AudioDevice = GetPlugin();
	if (!AudioDevice) [[unlikely]]
	{
		return 0;
	}
	return AudioDevice->Id;
}

WwiseDBGuid WwiseRefAudioDevice::AudioDeviceGuid() const
{
	const auto* AudioDevice = GetPlugin();
	if (!AudioDevice) [[unlikely]]
	{
		return {};
	}
	return AudioDevice->GUID;
}

const WwiseDBString* WwiseRefAudioDevice::AudioDeviceName() const
{
	const auto* AudioDevice = GetPlugin();
	if (!AudioDevice) [[unlikely]]
	{
		return &WwiseDBString::Empty;
	}
	return &AudioDevice->Name;
}

const WwiseDBString* WwiseRefAudioDevice::AudioDeviceObjectPath() const
{
	const auto* AudioDevice = GetPlugin();
	if (!AudioDevice) [[unlikely]]
	{
		return &WwiseDBString::Empty;
	}
	return &AudioDevice->ObjectPath;
}

WwiseDBShortId WwiseRefAudioDevice::Hash() const
{
	auto Result = WwiseRefSoundBank::Hash();
	Result = WwiseDBHashCombine(Result, GetTypeHash(AudioDeviceIndex));
	return Result;
}
