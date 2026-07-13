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
#include "Metadata/WwiseMetadataMedia.h"

struct WwiseMetadataPluginReference : public WwiseMetadataLoadable
{
	WwiseDBShortId Id;

	WwiseMetadataPluginReference(WwiseMetadataLoader& Loader);
};

inline WwiseDBShortId GetTypeHash(const WwiseMetadataPluginReference& Plugin)
{
	return GetTypeHash(Plugin.Id);
}
inline bool operator ==(const WwiseMetadataPluginReference& Lhs, const WwiseMetadataPluginReference& Rhs)
{
	return Lhs.Id == Rhs.Id;
}
inline bool operator <(const WwiseMetadataPluginReference& Lhs, const WwiseMetadataPluginReference& Rhs)
{
	return Lhs.Id < Rhs.Id;
}

struct WwiseMetadataPluginAttributes : public WwiseMetadataBasicReference
{
	WwiseDBString LibName;
	WwiseDBShortId LibId;

	WwiseMetadataPluginAttributes(WwiseMetadataLoader& Loader);
};

struct WwiseMetadataPlugin : public WwiseMetadataPluginAttributes
{
	WwiseDBArray<WwiseMetadataMediaReference> MediaRefs;
	WwiseMetadataPluginReferenceGroup* PluginRefs;

	WwiseMetadataPlugin(WwiseMetadataLoader& Loader);
};

template <>
struct std::hash<WwiseMetadataPluginReference>
{
	std::size_t operator()(const WwiseMetadataPluginReference& Id) const noexcept
	{
		return GetTypeHash(Id);
	}
};
