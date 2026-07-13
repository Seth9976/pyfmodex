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

class WwiseRefAcousticTexture : public WwiseRefSoundBank
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::AcousticTexture;
	struct FGlobalIdsMap;

	WwiseRefIndexType AcousticTextureIndex;

	WwiseRefAcousticTexture() {}
	WwiseRefAcousticTexture(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath,
		WwiseRefIndexType InSoundBankIndex, WwiseDBShortId InLanguageId,
		WwiseRefIndexType InAcousticTextureIndex) :
		WwiseRefSoundBank(InRootMediaRef, InJsonFilePath, InSoundBankIndex, InLanguageId),
		AcousticTextureIndex(InAcousticTextureIndex)
	{}
	const WwiseMetadataAcousticTexture* GetAcousticTexture() const;
	
	WwiseDBShortId AcousticTextureId() const;
	const WwiseDBGuid* AcousticTextureGuid() const;
	const WwiseDBString* AcousticTextureName() const;
	const WwiseDBString* AcousticTextureObjectPath() const;

	WwiseDBShortId Hash() const override;
	WwiseRefType Type() const override { return TYPE; }
	bool operator==(const WwiseRefAcousticTexture& Rhs) const
	{
		return WwiseRefSoundBank::operator ==(Rhs)
			&& AcousticTextureIndex == Rhs.AcousticTextureIndex;
	}
	bool operator!=(const WwiseRefAcousticTexture& Rhs) const { return !operator==(Rhs); }
};

struct WwiseRefAcousticTexture::FGlobalIdsMap
{
	WwiseAcousticTextureGlobalIdsMap GlobalIdsMap;

	FGlobalIdsMap() {}
	FGlobalIdsMap(const FGlobalIdsMap& Rhs) :
		GlobalIdsMap(Rhs.GlobalIdsMap)
	{}
	FGlobalIdsMap(FGlobalIdsMap&& Rhs) :
		GlobalIdsMap(std::move(Rhs.GlobalIdsMap))
	{}
};
