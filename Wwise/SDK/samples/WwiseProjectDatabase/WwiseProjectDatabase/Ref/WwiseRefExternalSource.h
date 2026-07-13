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

class WwiseRefExternalSource : public WwiseRefSoundBank
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::ExternalSource;
	struct FGlobalIdsMap;

	WwiseRefIndexType ExternalSourceIndex;

	WwiseRefExternalSource() {}
	WwiseRefExternalSource(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath,
		WwiseRefIndexType InSoundBankIndex, WwiseDBShortId InLanguageId,
		WwiseRefIndexType InExternalSourceIndex) :
		WwiseRefSoundBank(InRootMediaRef, InJsonFilePath, InSoundBankIndex, InLanguageId),
		ExternalSourceIndex(InExternalSourceIndex)
	{}
	const WwiseMetadataExternalSource* GetExternalSource() const;

	WwiseDBShortId ExternalSourceCookie() const;
	WwiseDBGuid ExternalSourceGuid() const;
	const WwiseDBString* ExternalSourceName() const;
	const WwiseDBString* ExternalSourceObjectPath() const;

	WwiseDBShortId Hash() const override;
	WwiseRefType Type() const override { return TYPE; }
	bool operator==(const WwiseRefExternalSource& Rhs) const
	{
		return WwiseRefSoundBank::operator ==(Rhs)
			&& ExternalSourceIndex == Rhs.ExternalSourceIndex;
	}
	bool operator!=(const WwiseRefExternalSource& Rhs) const { return !operator==(Rhs); }
};

struct WwiseRefExternalSource::FGlobalIdsMap
{
	WwiseExternalSourceGlobalIdsMap GlobalIdsMap;
};
