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

class WwiseRefDialogueEvent : public WwiseRefSoundBank
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::DialogueEvent;
	struct FGlobalIdsMap;

	WwiseRefIndexType DialogueEventIndex;

	WwiseRefDialogueEvent() {}
	WwiseRefDialogueEvent(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath,
		WwiseRefIndexType InSoundBankIndex, WwiseDBShortId InLanguageId,
		WwiseRefIndexType InDialogueEventIndex) :
		WwiseRefSoundBank(InRootMediaRef, InJsonFilePath, InSoundBankIndex, InLanguageId),
		DialogueEventIndex(InDialogueEventIndex)
	{}
	const WwiseMetadataDialogueEvent* GetDialogueEvent() const;
	WwiseDialogueArgumentArray GetDialogueArguments(const WwiseDialogueArgumentGlobalIdsMap& GlobalMap) const;
	WwiseMediaIdsMap GetAllMedia(const WwiseMediaGlobalIdsMap& GlobalMap) const;

	WwiseDBShortId DialogueEventId() const;
	WwiseDBGuid DialogueEventGuid() const;
	const WwiseDBString* DialogueEventName() const;
	const WwiseDBString* DialogueEventObjectPath() const;

	WwiseDBShortId Hash() const override;
	WwiseRefType Type() const override { return TYPE; }
	bool operator==(const WwiseRefDialogueEvent& Rhs) const
	{
		return WwiseRefSoundBank::operator ==(Rhs)
			&& DialogueEventIndex == Rhs.DialogueEventIndex;
	}
	bool operator!=(const WwiseRefDialogueEvent& Rhs) const { return !operator==(Rhs); }
};

struct WwiseRefDialogueEvent::FGlobalIdsMap
{
	WwiseDialogueEventGlobalIdsMap GlobalIdsMap;
};
