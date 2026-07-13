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

#include "Metadata/WwiseMetadataBasicReference.h"

struct WwiseMetadataAcousticTexture : public WwiseMetadataBasicReference
{
	float AbsorptionLow { 0 };
	float AbsorptionMidLow { 0 };
	float AbsorptionMidHigh { 0 };
	float AbsorptionHigh { 0 };
	float Scattering { 0 };

	WwiseMetadataAcousticTexture(WwiseMetadataLoader& Loader);

private:
	static const WwiseDBMap<WwiseDBString, unsigned int> FloatProperties;
	static const WwiseDBMap<WwiseDBString, unsigned int> FillFloatProperties();
};

