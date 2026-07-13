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

#include "Ref/WwiseRefPluginLib.h"
#include "Metadata/WwiseMetadataPluginInfo.h"
#include "Metadata/WwiseMetadataPluginLib.h"

const WwiseDBString WwiseRefPluginLib::NAME = "PluginLib"_wwise_db;

const WwiseMetadataPluginLib* WwiseRefPluginLib::GetPluginLib() const
{
	const auto* PluginInfo = GetPluginInfo();
	if (!PluginInfo) [[unlikely]]
	{
		return nullptr;
	}
	const auto& PluginLibs = PluginInfo->PluginLibs;
	if (PluginLibs.IsValidIndex(PluginLibIndex))
	{
		return &PluginLibs[PluginLibIndex];
	}
	else
	{
		WWISE_DB_LOG(Error, "Could not get PluginLib index #%zu", PluginLibIndex);
		return nullptr;
	}
}

WwiseDBShortId WwiseRefPluginLib::PluginLibId() const
{
	const auto* PluginLib = GetPluginLib();
	if (!PluginLib) [[unlikely]]
	{
		return 0;
	}
	return PluginLib->LibId;
}

const WwiseDBString* WwiseRefPluginLib::PluginLibName() const
{
	const auto* PluginLib = GetPluginLib();
	if (!PluginLib) [[unlikely]]
	{
		return {};
	}
	return &PluginLib->LibName;
}

const WwiseDBString* WwiseRefPluginLib::PluginLibDLL() const
{
	const auto* PluginLib = GetPluginLib();
	if (!PluginLib) [[unlikely]]
	{
		return {};
	}
	return &PluginLib->DLL;
}

const WwiseDBString* WwiseRefPluginLib::PluginLibStaticLib() const
{
	const auto* PluginLib = GetPluginLib();
	if (!PluginLib) [[unlikely]]
	{
		return {};
	}
	return &PluginLib->StaticLib;
}

WwiseDBShortId WwiseRefPluginLib::Hash() const
{
	auto Result = WwiseRefPluginInfo::Hash();
	Result = WwiseDBHashCombine(Result, GetTypeHash(PluginLibIndex));
	return Result;
}
