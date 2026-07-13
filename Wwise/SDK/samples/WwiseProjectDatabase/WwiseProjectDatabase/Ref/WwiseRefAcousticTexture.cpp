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

#include "Ref/WwiseRefAcousticTexture.h"

#include "Metadata/WwiseMetadataAcousticTexture.h"
#include "Metadata/WwiseMetadataSoundBank.h"

const WwiseDBString WwiseRefAcousticTexture::NAME = "AcousticTexture"_wwise_db;

const WwiseMetadataAcousticTexture* WwiseRefAcousticTexture::GetAcousticTexture() const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return nullptr;
	}
	const auto& AcousticTextures = SoundBank->AcousticTextures;
	if (AcousticTextures.IsValidIndex(AcousticTextureIndex))
	{
		return &AcousticTextures.Array[AcousticTextureIndex];
	}
	else
	{
		WWISE_DB_LOG(Error, "Could not get Acoustic Texture index #%zu", AcousticTextureIndex);
		return nullptr;
	}
}

WwiseDBShortId WwiseRefAcousticTexture::AcousticTextureId() const
{
	const auto* AcousticTexture = GetAcousticTexture();
	if (!AcousticTexture) [[unlikely]]
	{
		return 0;
	}
	return AcousticTexture->Id;
}

const WwiseDBGuid* WwiseRefAcousticTexture::AcousticTextureGuid() const
{
	const auto* AcousticTexture = GetAcousticTexture();
	if (!AcousticTexture) [[unlikely]]
	{
		return {};
	}
	return &AcousticTexture->GUID;
}

const WwiseDBString* WwiseRefAcousticTexture::AcousticTextureName() const
{
	const auto* AcousticTexture = GetAcousticTexture();
	if (!AcousticTexture) [[unlikely]]
	{
		return &WwiseDBString::Empty;
	}
	return &AcousticTexture->Name;
}

const WwiseDBString* WwiseRefAcousticTexture::AcousticTextureObjectPath() const
{
	const auto* AcousticTexture = GetAcousticTexture();
	if (!AcousticTexture) [[unlikely]]
	{
		return &WwiseDBString::Empty;
	}
	return &AcousticTexture->ObjectPath;
}

WwiseDBShortId WwiseRefAcousticTexture::Hash() const
{
	auto Result = WwiseRefSoundBank::Hash();
	Result = WwiseDBHashCombine(Result, GetTypeHash(AcousticTextureIndex));
	return Result;
}
