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

class WwiseRefMedia : public WwiseRefSoundBank
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::Media;
	struct FGlobalIdsMap;

	WwiseRefIndexType MediaIndex;

	WwiseRefMedia() {}
	WwiseRefMedia(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath,
		WwiseRefIndexType InSoundBankIndex, WwiseDBShortId InLanguageId,
		WwiseRefIndexType InMediaIndex) :
		WwiseRefSoundBank(InRootMediaRef, InJsonFilePath, InSoundBankIndex, InLanguageId),
		MediaIndex(InMediaIndex)
	{}
	const WwiseMetadataMedia* GetMedia() const;

	WwiseDBShortId MediaId() const;
	const WwiseDBString* MediaShortName() const;
	const WwiseDBString* MediaPath() const;

	WwiseDBShortId Hash() const override;
	WwiseRefType Type() const override { return TYPE; }
	bool operator==(const WwiseRefMedia& Rhs) const
	{
		return WwiseRefSoundBank::operator==(Rhs)
			&& MediaIndex == Rhs.MediaIndex;
	}
	bool operator!=(const WwiseRefMedia& Rhs) const { return !operator==(Rhs); }
};

struct WwiseRefMedia::FGlobalIdsMap
{
	WwiseMediaGlobalIdsMap GlobalIdsMap;
};

struct WwiseOutsourcedRefMedia
{
	unsigned int ReferenceCount = 0;
	WwiseDBArray<WwiseDatabaseLocalizableIdKey> SoundBankLocalizableIdKeys = WwiseDBArray<WwiseDatabaseLocalizableIdKey>();
};