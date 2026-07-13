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

#include "Metadata/WwiseMetadataAcousticTexture.h"
#include "Metadata/WwiseMetadataLoader.h"

WwiseMetadataAcousticTexture::WwiseMetadataAcousticTexture(WwiseMetadataLoader& Loader) :
	WwiseMetadataBasicReference(Loader)
{
	Loader.GetPropertyArray(this, FloatProperties);
	Loader.LogParsed("AcousticTexture"_wwise_db, Id, Name);
}

const WwiseDBMap<WwiseDBString, unsigned int> WwiseMetadataAcousticTexture::FloatProperties = WwiseMetadataAcousticTexture::FillFloatProperties();
const WwiseDBMap<WwiseDBString, unsigned int> WwiseMetadataAcousticTexture::FillFloatProperties()
{
	WwiseDBMap<WwiseDBString, unsigned int> Result;
	Result.Add("AbsorptionLow"_wwise_db, offsetof(WwiseMetadataAcousticTexture, AbsorptionLow));
	Result.Add("AbsorptionMidLow"_wwise_db, offsetof(WwiseMetadataAcousticTexture, AbsorptionMidLow));
	Result.Add("AbsorptionMidHigh"_wwise_db, offsetof(WwiseMetadataAcousticTexture, AbsorptionMidHigh));
	Result.Add("AbsorptionHigh"_wwise_db, offsetof(WwiseMetadataAcousticTexture, AbsorptionHigh));
	Result.Add("Scattering"_wwise_db, offsetof(WwiseMetadataAcousticTexture, Scattering));
	return Result;
}
