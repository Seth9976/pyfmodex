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

#include "Ref/WwiseRefDialogueEvent.h"

class WwiseRefDialogueArgument : public WwiseRefDialogueEvent
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::DialogueArgument;
	struct FGlobalIdsMap;

	WwiseRefIndexType DialogueArgumentIndex;

	WwiseRefDialogueArgument() {}
	WwiseRefDialogueArgument(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath,
		WwiseRefIndexType InSoundBankIndex, WwiseDBShortId InLanguageId,
		WwiseRefIndexType InDialogueEventIndex,
		WwiseRefIndexType InDialogueArgumentIndex) :
		WwiseRefDialogueEvent(InRootMediaRef, InJsonFilePath, InSoundBankIndex, InLanguageId, InDialogueEventIndex),
		DialogueArgumentIndex(InDialogueArgumentIndex)
	{}
	const WwiseMetadataDialogueArgument* GetDialogueArgument() const;

	WwiseDBShortId DialogueArgumentId() const;
	WwiseDBGuid DialogueArgumentGuid() const;
	WwiseDBString DialogueArgumentName() const;
	WwiseDBString DialogueArgumentObjectPath() const;

	WwiseDBShortId Hash() const override;
	WwiseRefType Type() const override { return TYPE; }
	bool operator==(const WwiseRefDialogueArgument& Rhs) const
	{
		return WwiseRefDialogueEvent::operator ==(Rhs)
			&& DialogueArgumentIndex == Rhs.DialogueArgumentIndex;
	}
	bool operator!=(const WwiseRefDialogueArgument& Rhs) const { return !operator==(Rhs); }
};

struct WwiseRefDialogueArgument::FGlobalIdsMap
{
	WwiseDialogueArgumentGlobalIdsMap GlobalIdsMap;
};
