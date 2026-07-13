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


struct WwiseMetadataGroupValueReference : public WwiseMetadataBasicReference
{
	WwiseDBShortId GroupId;

	WwiseMetadataGroupValueReference();
	WwiseMetadataGroupValueReference(WwiseMetadataLoader& Loader);
};

inline WwiseDBShortId GetTypeHash(const WwiseMetadataGroupValueReference& Ref)
{
	return WwiseDBHashCombine(GetTypeHash((const WwiseMetadataBasicReference&)Ref), GetTypeHash(Ref.Id));
}
inline bool operator==(const WwiseMetadataGroupValueReference& Lhs, const WwiseMetadataGroupValueReference& Rhs)
{
	return (const WwiseMetadataBasicReference&)Lhs == Rhs && Lhs.GroupId == Rhs.GroupId;
}
inline bool operator<(const WwiseMetadataGroupValueReference& Lhs, const WwiseMetadataGroupValueReference& Rhs)
{
	return (const WwiseMetadataBasicReference&)Lhs < Rhs && Lhs.GroupId < Rhs.GroupId;
}
