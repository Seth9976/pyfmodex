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

class WwiseRefStateGroup : public WwiseRefSoundBank
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::StateGroup;
	struct FGlobalIdsMap;

	WwiseRefIndexType StateGroupIndex;

	WwiseRefStateGroup() {}
	WwiseRefStateGroup(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath,
		WwiseRefIndexType InSoundBankIndex, WwiseDBShortId InLanguageId,
		WwiseRefIndexType InStateGroupIndex) :
		WwiseRefSoundBank(InRootMediaRef, InJsonFilePath, InSoundBankIndex, InLanguageId),
		StateGroupIndex(InStateGroupIndex)
	{}
	const WwiseMetadataStateGroup* GetStateGroup() const;

	WwiseDBShortId StateGroupId() const;
	const WwiseDBGuid* StateGroupGuid() const;
	const WwiseDBString* StateGroupName() const;
	const WwiseDBString* StateGroupObjectPath() const;

	WwiseDBShortId Hash() const override;
	WwiseRefType Type() const override { return TYPE; }
	bool operator==(const WwiseRefStateGroup& Rhs) const
	{
		return WwiseRefSoundBank::operator ==(Rhs)
			&& StateGroupIndex == Rhs.StateGroupIndex;
	}
	bool operator!=(const WwiseRefStateGroup& Rhs) const { return !operator==(Rhs); }
};

struct WwiseRefStateGroup::FGlobalIdsMap
{
	WwiseStateGroupGlobalIdsMap GlobalIdsMap;
};
