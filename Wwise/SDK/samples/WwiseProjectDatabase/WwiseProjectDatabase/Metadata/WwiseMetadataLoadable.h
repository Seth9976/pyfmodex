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

#include "Metadata/WwiseMetadataForwardDeclarations.h"
#include "AdapterTypes/WwiseDataTypesAdapter.h"
#include "AdapterTypes/WwiseWrapperTypes.h"

struct WwiseMetadataLoadable
{
protected:
	WwiseDBSet<WwiseDBString> RequestedValues;
	unsigned int LoadedSize;

	inline WwiseMetadataLoadable() :
		RequestedValues(),
		LoadedSize(0)
	{}

	inline ~WwiseMetadataLoadable()
	{
		UnloadLoadedSize();
	}

public:
	void AddRequestedValue(const WwiseDBString& Type, const WwiseDBString& Value);
	void CheckRequestedValues(WwiseDBJsonObject& JsonObject) const;
	void IncLoadedSize(unsigned int Size);
	void DecLoadedSize(unsigned int Size);
	void UnloadLoadedSize();
};
