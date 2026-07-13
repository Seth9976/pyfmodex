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

#include "AdapterTypes/WwiseDataTypesAdapter.h"
#include "Metadata/WwiseMetadataLoadable.h"
#include "WwiseDatabaseIdentifiers.h"

struct WwiseMetadataExternalSourceReference : public WwiseMetadataLoadable
{
	WwiseDBShortId Cookie;

	WwiseMetadataExternalSourceReference(WwiseMetadataLoader& Loader);
};

inline WwiseDBShortId GetTypeHash(const WwiseMetadataExternalSourceReference& Ref)
{
	return GetTypeHash(Ref.Cookie);
}
inline bool operator==(const WwiseMetadataExternalSourceReference& Lhs, const WwiseMetadataExternalSourceReference& Rhs)
{
	return Lhs.Cookie == Rhs.Cookie;
}
inline bool operator<(const WwiseMetadataExternalSourceReference& Lhs, const WwiseMetadataExternalSourceReference& Rhs)
{
	return Lhs.Cookie < Rhs.Cookie;
}

struct WwiseMetadataExternalSource : public WwiseMetadataExternalSourceReference
{
	WwiseDBString Name;
	WwiseDBString ObjectPath;
	WwiseDBGuid GUID;

	WwiseMetadataExternalSource(WwiseMetadataLoader& Loader);
};

template <>
struct std::hash<WwiseMetadataExternalSourceReference>
{
	std::size_t operator()(const WwiseMetadataExternalSourceReference& Id) const noexcept
	{
		return GetTypeHash(Id);
	}
};
