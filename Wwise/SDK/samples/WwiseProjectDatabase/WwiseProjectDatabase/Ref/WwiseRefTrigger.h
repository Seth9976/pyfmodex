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

class WwiseRefTrigger : public WwiseRefSoundBank
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::Trigger;
	struct FGlobalIdsMap;

	WwiseRefIndexType TriggerIndex;

	WwiseRefTrigger() {}
	WwiseRefTrigger(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath,
		WwiseRefIndexType InSoundBankIndex, WwiseDBShortId InLanguageId,
		WwiseRefIndexType InTriggerIndex) :
		WwiseRefSoundBank(InRootMediaRef, InJsonFilePath, InSoundBankIndex, InLanguageId),
		TriggerIndex(InTriggerIndex)
	{}
	const WwiseMetadataTrigger* GetTrigger() const;

	WwiseDBShortId TriggerId() const;
	const WwiseDBGuid* TriggerGuid() const;
	const WwiseDBString* TriggerName() const;
	const WwiseDBString* TriggerObjectPath() const;

	WwiseDBShortId Hash() const override;
	WwiseRefType Type() const override { return TYPE; }
	bool operator==(const WwiseRefTrigger& Rhs) const
	{
		return WwiseRefSoundBank::operator ==(Rhs)
			&& TriggerIndex == Rhs.TriggerIndex;
	}
	bool operator!=(const WwiseRefTrigger& Rhs) const { return !operator==(Rhs); }
};

struct WwiseRefTrigger::FGlobalIdsMap
{
	WwiseTriggerGlobalIdsMap GlobalIdsMap;
};
