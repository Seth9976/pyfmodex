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

#pragma once

#include "Ref/WwiseRefSoundBank.h"

class WwiseRefAudioDevice : public WwiseRefSoundBank
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::AudioDevice;
	struct FGlobalIdsMap;

	WwiseRefIndexType AudioDeviceIndex;

	WwiseRefAudioDevice() {}
	WwiseRefAudioDevice(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath,
		WwiseRefIndexType InSoundBankIndex, WwiseDBShortId InLanguageId,
		WwiseRefIndexType InAudioDeviceIndex) :
		WwiseRefSoundBank(InRootMediaRef, InJsonFilePath, InSoundBankIndex, InLanguageId),
		AudioDeviceIndex(InAudioDeviceIndex)
	{}
	const WwiseMetadataPlugin* GetPlugin() const;
	WwiseMediaIdsMap GetPluginMedia(const WwiseMediaGlobalIdsMap& GlobalMap) const;

	WwiseDBShortId AudioDeviceId() const;
	WwiseDBGuid AudioDeviceGuid() const;
	const WwiseDBString* AudioDeviceName() const;
	const WwiseDBString* AudioDeviceObjectPath() const;

	WwiseDBShortId Hash() const override;
	WwiseRefType Type() const override { return TYPE; }
	bool operator==(const WwiseRefAudioDevice& Rhs) const
	{
		return WwiseRefSoundBank::operator==(Rhs)
			&& AudioDeviceIndex == Rhs.AudioDeviceIndex;
	}
	bool operator!=(const WwiseRefAudioDevice& Rhs) const { return !operator==(Rhs); }
};

struct WwiseRefAudioDevice::FGlobalIdsMap
{
	WwiseAudioDeviceGlobalIdsMap GlobalIdsMap;
};
