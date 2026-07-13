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

struct WwiseMetadataDialogueEventReference : public WwiseMetadataBasicReference
{
	WwiseMetadataDialogueEventReference(WwiseMetadataLoader& Loader);
};

struct WwiseMetadataDialogueArgument : public WwiseMetadataBasicReference
{
	WwiseMetadataDialogueArgument(WwiseMetadataLoader& Loader);
};

struct WwiseMetadataDialogueEvent : public WwiseMetadataDialogueEventReference
{
	WwiseDBArray<WwiseMetadataDialogueArgument> Arguments;

	WwiseMetadataDialogueEvent(WwiseMetadataLoader& Loader);
};

template <>
struct std::hash<WwiseMetadataDialogueEventReference>
{
	std::size_t operator()(const WwiseMetadataDialogueEventReference& Id) const noexcept
	{
		return GetTypeHash(Id);
	}
};

template <>
struct std::hash<WwiseMetadataDialogueArgument>
{
	std::size_t operator()(const WwiseMetadataDialogueArgument& Id) const noexcept
	{
		return GetTypeHash(Id);
	}
};