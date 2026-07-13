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

#include "Metadata/WwiseMetadataLoadable.h"
#include "AdapterTypes/WwiseDataTypesAdapter.h"
#include "WwiseDatabaseIdentifiers.h"

struct WwiseMetadataMediaReference : public WwiseMetadataLoadable
{
	WwiseDBShortId Id;

	WwiseMetadataMediaReference(WwiseMetadataLoader& Loader);
};

inline WwiseDBShortId GetTypeHash(const WwiseMetadataMediaReference& Media)
{
	return GetTypeHash(Media.Id);
}
inline bool operator ==(const WwiseMetadataMediaReference& Lhs, const WwiseMetadataMediaReference& Rhs)
{
	return Lhs.Id == Rhs.Id;
}
inline bool operator <(const WwiseMetadataMediaReference& Lhs, const WwiseMetadataMediaReference& Rhs)
{
	return Lhs.Id < Rhs.Id;
}

enum class WwiseMetadataMediaLocation : WwiseDBShortId
{
	Memory,
	Loose,
	OtherBank,

	Unknown = (WwiseDBShortId)-1
};

struct WwiseMetadataMediaAttributes : public WwiseMetadataMediaReference
{
	WwiseDBString Language;
	bool bStreaming;
	WwiseMetadataMediaLocation Location;
	bool bUsingReferenceLanguage;
	WwiseDBShortId Align;
	bool bDeviceMemory;

	WwiseMetadataMediaAttributes(WwiseMetadataLoader& Loader);

private:
	static WwiseMetadataMediaLocation LocationFromString(const WwiseDBString& LocationString);
};

struct WwiseMetadataMedia : public WwiseMetadataMediaAttributes
{
	WwiseDBString ShortName;
	WwiseDBString Path;
	WwiseDBString CachePath;
	WwiseDBShortId PrefetchSize;
	bool bContainsPrefetch;

	WwiseMetadataMedia(WwiseMetadataLoader& Loader);
};

template <>
struct std::hash<WwiseMetadataMediaReference>
{
	std::size_t operator()(const WwiseMetadataMediaReference& Id) const noexcept
	{
		return GetTypeHash(Id);
	}
};