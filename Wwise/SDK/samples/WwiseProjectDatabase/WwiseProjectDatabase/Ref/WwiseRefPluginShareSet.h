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

class WwiseRefPluginShareSet : public WwiseRefSoundBank
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::PluginShareSet;
	struct FGlobalIdsMap;

	WwiseRefIndexType PluginShareSetIndex;

	WwiseRefPluginShareSet() {}
	WwiseRefPluginShareSet(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath,
		WwiseRefIndexType InSoundBankIndex, WwiseDBShortId InLanguageId,
		WwiseRefIndexType InPluginShareSetIndex) :
		WwiseRefSoundBank(InRootMediaRef, InJsonFilePath, InSoundBankIndex, InLanguageId),
		PluginShareSetIndex(InPluginShareSetIndex)
	{}
	const WwiseMetadataPlugin* GetPlugin() const;
	WwiseMediaIdsMap GetPluginMedia(const WwiseMediaGlobalIdsMap& GlobalMap) const;
	WwiseCustomPluginIdsMap GetPluginCustomPlugins(const WwiseCustomPluginGlobalIdsMap& GlobalMap) const;
	WwisePluginShareSetIdsMap GetPluginPluginShareSets(const WwisePluginShareSetGlobalIdsMap& GlobalMap) const;
	WwiseAudioDeviceIdsMap GetPluginAudioDevices(const WwiseAudioDeviceGlobalIdsMap& GlobalMap) const;

	WwiseDBShortId PluginShareSetId() const;
	WwiseDBGuid PluginShareSetGuid() const;
	const WwiseDBString* PluginShareSetName() const;
	const WwiseDBString* PluginShareSetObjectPath() const;

	WwiseDBShortId Hash() const override;
	WwiseRefType Type() const override { return TYPE; }
	bool operator==(const WwiseRefPluginShareSet& Rhs) const
	{
		return WwiseRefSoundBank::operator==(Rhs)
			&& PluginShareSetIndex == Rhs.PluginShareSetIndex;
	}
	bool operator!=(const WwiseRefPluginShareSet& Rhs) const { return !operator==(Rhs); }
};

struct WwiseRefPluginShareSet::FGlobalIdsMap
{
	WwisePluginShareSetGlobalIdsMap GlobalIdsMap;
};
