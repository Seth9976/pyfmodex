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

#include "Metadata/WwiseMetadataCollections.h"
#include "Ref/WwiseRefCollections.h"
#include "AdapterTypes/WwiseDataTypesAdapter.h"
#include "AdapterTypes/WwiseWrapperTypes.h"
#include "Ref/WwiseRefType.h"

class WwiseRefRootFile
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::RootFile;

	WwiseMetadataSharedRootFileConstPtr RootFileRef;
	WwiseDBString JsonFilePath;

	WwiseRefRootFile() {}
	WwiseRefRootFile(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath) :
		RootFileRef(InRootMediaRef),
		JsonFilePath(InJsonFilePath)
	{}
	virtual ~WwiseRefRootFile() {}
	virtual WwiseDBShortId Hash() const;
	virtual WwiseRefType Type() const { return TYPE; }

	WwiseRefRootFile(WwiseRefRootFile&& other) noexcept
	: RootFileRef(std::move(other.RootFileRef)),
	JsonFilePath(std::move(other.JsonFilePath))
	{}

	WwiseRefRootFile(const WwiseRefRootFile& other) = default;
	WwiseRefRootFile& operator=(const WwiseRefRootFile& other) = default;
	WwiseRefRootFile& operator=(WwiseRefRootFile&& other) = default;
	bool operator==(const WwiseRefRootFile& Rhs) const
	{
		return JsonFilePath == Rhs.JsonFilePath;
	}
	bool operator!=(const WwiseRefRootFile& Rhs) const { return !operator==(Rhs); }

	bool operator<(const WwiseRefRootFile& Rhs) const
	{
		return JsonFilePath < Rhs.JsonFilePath;
	}
	
	bool IsValid() const;
	const WwiseMetadataRootFile* GetRootFile() const;
};

inline WwiseDBShortId GetTypeHash(const WwiseRefRootFile& Type)
{
	return Type.Hash();
}
