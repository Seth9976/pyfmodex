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

#include "Ref/WwiseRefPluginInfo.h"

class WwiseRefPluginLib : public WwiseRefPluginInfo
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::PluginLib;
	struct FGlobalIdsMap;

	WwiseRefIndexType PluginLibIndex;

	WwiseRefPluginLib() :
		PluginLibIndex(-1)
	{}
	WwiseRefPluginLib(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath,
		WwiseRefIndexType InPluginIndex) :
		WwiseRefPluginInfo(InRootMediaRef, InJsonFilePath),
		PluginLibIndex(InPluginIndex)
	{}
	const WwiseMetadataPluginLib* GetPluginLib() const;

	WwiseDBShortId PluginLibId() const;
	const WwiseDBString* PluginLibName() const;
	const WwiseDBString* PluginLibDLL() const;
	const WwiseDBString* PluginLibStaticLib() const;

	WwiseDBShortId Hash() const override;
	WwiseRefType Type() const override { return TYPE; }
	bool operator==(const WwiseRefPluginLib& Rhs) const
	{
		return WwiseRefPluginInfo::operator==(Rhs)
			&& PluginLibIndex == Rhs.PluginLibIndex;
	}
	bool operator!=(const WwiseRefPluginLib& Rhs) const { return !operator==(Rhs); }
};

struct WwiseRefPluginLib::FGlobalIdsMap
{
	WwisePluginLibGlobalIdsMap GlobalIdsMap;
};
