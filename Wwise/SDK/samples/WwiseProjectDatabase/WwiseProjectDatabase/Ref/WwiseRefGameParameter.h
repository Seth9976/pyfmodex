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

class WwiseRefGameParameter : public WwiseRefSoundBank
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::GameParameter;
	struct FGlobalIdsMap;

	WwiseRefIndexType GameParameterIndex;

	WwiseRefGameParameter() {}
	WwiseRefGameParameter(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath,
		WwiseRefIndexType InSoundBankIndex, WwiseDBShortId InLanguageId,
		WwiseRefIndexType InGameParameterIndex) :
		WwiseRefSoundBank(InRootMediaRef, InJsonFilePath, InSoundBankIndex, InLanguageId),
		GameParameterIndex(InGameParameterIndex)
	{}
	const WwiseMetadataGameParameter* GetGameParameter() const;

	WwiseDBShortId GameParameterId() const;
	const WwiseDBGuid* GameParameterGuid() const;
	const WwiseDBString* GameParameterName() const;
	const WwiseDBString* GameParameterObjectPath() const;

	WwiseDBShortId Hash() const override;
	WwiseRefType Type() const override { return TYPE; }
	bool operator==(const WwiseRefGameParameter& Rhs) const
	{
		return WwiseRefSoundBank::operator ==(Rhs)
			&& GameParameterIndex == Rhs.GameParameterIndex;
	}
	bool operator!=(const WwiseRefGameParameter& Rhs) const { return !operator==(Rhs); }
};

struct WwiseRefGameParameter::FGlobalIdsMap
{
	WwiseGameParameterGlobalIdsMap GlobalIdsMap;
};
