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

#include "Ref/WwiseRefStateGroup.h"

class WwiseRefState : public WwiseRefStateGroup
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::State;
	struct FGlobalIdsMap;

	WwiseRefIndexType StateIndex;

	WwiseRefState() {}
	WwiseRefState(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath,
		WwiseRefIndexType InSoundBankIndex, WwiseDBShortId InLanguageId,
		WwiseRefIndexType InStateGroupIndex,
		WwiseRefIndexType InStateIndex) :
		WwiseRefStateGroup(InRootMediaRef, InJsonFilePath, InSoundBankIndex, InLanguageId, InStateGroupIndex),
		StateIndex(InStateIndex)
	{}
	const WwiseMetadataState* GetState() const;

	WwiseDBShortId StateId() const;
	const WwiseDBGuid* StateGuid() const;
	const WwiseDBString* StateName() const;
	const WwiseDBString* StateObjectPath() const;

	WwiseDBShortId Hash() const override;
	WwiseRefType Type() const override { return TYPE; }
	bool operator==(const WwiseRefState& Rhs) const
	{
		return WwiseRefStateGroup::operator ==(Rhs)
			&& StateIndex == Rhs.StateIndex;
	}
	bool operator!=(const WwiseRefState& Rhs) const { return !operator==(Rhs); }
};

struct WwiseRefState::FGlobalIdsMap
{
	WwiseStateGlobalIdsMap GlobalIdsMap;
};
