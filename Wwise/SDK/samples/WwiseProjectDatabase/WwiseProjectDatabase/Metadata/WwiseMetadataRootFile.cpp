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

#include "Metadata/WwiseMetadataRootFile.h"

#include "AdapterTypes/WwiseSharedPtr.h"
#include "Metadata/WwiseMetadataPlatformInfo.h"
#include "Metadata/WwiseMetadataPluginInfo.h"
#include "Metadata/WwiseMetadataProjectInfo.h"
#include "Metadata/WwiseMetadataSoundBanksInfo.h"
#include "Metadata/WwiseMetadataLoader.h"

WwiseMetadataRootFile::WwiseMetadataRootFile(WwiseMetadataLoader& Loader) :
	PlatformInfo(Loader.GetObjectPtr<WwiseMetadataPlatformInfo>(this, "PlatformInfo"_wwise_db)),
	PluginInfo(Loader.GetObjectPtr<WwiseMetadataPluginInfo>(this, "PluginInfo"_wwise_db)),
	ProjectInfo(Loader.GetObjectPtr<WwiseMetadataProjectInfo>(this, "ProjectInfo"_wwise_db)),
	SoundBanksInfo(Loader.GetObjectPtr<WwiseMetadataSoundBanksInfo>(this, "SoundBanksInfo"_wwise_db))
{
	if (Loader.bResult && !PlatformInfo && !PluginInfo && !ProjectInfo && !SoundBanksInfo)
	{
		Loader.Fail("Metadata/WwiseMetadataRootFile"_wwise_db);
	}
	IncLoadedSize(sizeof(WwiseMetadataRootFile));
}

WwiseMetadataRootFile::~WwiseMetadataRootFile()
{
	if (PlatformInfo)
	{
		delete PlatformInfo;
		PlatformInfo = nullptr;
	}
	if (PluginInfo)
	{
		delete PluginInfo;
		PluginInfo = nullptr;
	}
	if (ProjectInfo)
	{
		delete ProjectInfo;
		ProjectInfo = nullptr;
	}
	if (SoundBanksInfo)
	{
		delete SoundBanksInfo;
		SoundBanksInfo = nullptr;
	}
}

WwiseMetadataSharedRootFilePtr WwiseMetadataRootFile::LoadFile(const WwiseDBString& FilePath)
{
	WWISE_DB_LOG(Verbose, "Parsing file in: %s", *FilePath);

	WwiseDBJsonObject RootJsonObject(FilePath);
	WwiseMetadataLoader Loader(RootJsonObject);
	auto Result = WwiseDBSharedPtr<WwiseMetadataRootFile>();
	Result.Make_Shared(Loader);

	if (!Loader.bResult)
	{
		Loader.LogParsed("LoadFile"_wwise_db, 0, WwiseDBString(FilePath));
		return {};
	}

	return Result;
}

WwiseMetadataFileMap WwiseMetadataRootFile::LoadFiles(const WwiseDBArray<WwiseDBString>& FilePaths)
{
	WwiseMetadataFileMap Result;

	for(const auto& FilePath : FilePaths)
	{
		Result.Add(FilePath, nullptr);
	}

	WwiseParallelFor(FilePaths,
	[&Result, &FilePaths](const auto& n)
	{
		Result.AddOrReplace(FilePaths(n), WwiseMetadataRootFile::LoadFile(FilePaths(n)));
	});

	return Result;
}
