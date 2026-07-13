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

class WwiseRefSwitchGroup : public WwiseRefSoundBank
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::SwitchGroup;
	struct FGlobalIdsMap;

	WwiseRefIndexType SwitchGroupIndex;

	WwiseRefSwitchGroup() {}
	WwiseRefSwitchGroup(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath,
		WwiseRefIndexType InSoundBankIndex, WwiseDBShortId InLanguageId,
		WwiseRefIndexType InSwitchGroupIndex) :
		WwiseRefSoundBank(InRootMediaRef, InJsonFilePath, InSoundBankIndex, InLanguageId),
		SwitchGroupIndex(InSwitchGroupIndex)
	{}
	const WwiseMetadataSwitchGroup* GetSwitchGroup() const;
	bool IsControlledByGameParameter() const;

	WwiseDBShortId SwitchGroupId() const;
	const WwiseDBGuid* SwitchGroupGuid() const;
	const WwiseDBString* SwitchGroupName() const;
	const WwiseDBString* SwitchGroupObjectPath() const;

	WwiseDBShortId Hash() const override;
	WwiseRefType Type() const override { return TYPE; }
	bool operator==(const WwiseRefSwitchGroup& Rhs) const
	{
		return WwiseRefSoundBank::operator ==(Rhs)
			&& SwitchGroupIndex == Rhs.SwitchGroupIndex;
	}
	bool operator!=(const WwiseRefSwitchGroup& Rhs) const { return !operator==(Rhs); }
};

struct WwiseRefSwitchGroup::FGlobalIdsMap
{
	WwiseSwitchGroupGlobalIdsMap GlobalIdsMap;
};
