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

#include "AdapterTypes/WwiseWrapperTypes.h"
#include "Metadata/WwiseMetadataCollections.h"
#include "WwiseSharedDBPlatformId.h"

struct WwiseGeneratedFiles
{
	using FileTuple = WwiseDBTuple<WwiseDBString, WwiseDateTime>;
	using FileDateTimeMap = WwiseDBMap<WwiseDBString, WwiseDateTime>;

	struct FGeneratedRootFiles
	{
		FileTuple ProjectInfoFile;
		FileTuple WwiseIDsFile;
	};

	struct FPlatformFiles
	{
		FileTuple PlatformInfoFile;
		FileTuple PluginInfoFile;
		FileTuple SoundbanksInfoFile;

		FileDateTimeMap SoundBankFiles;
		FileDateTimeMap MediaFiles;
		FileDateTimeMap MetadataFiles;
		FileDateTimeMap ExtraFiles;

		WwiseDBArray<WwiseDBString> DirectoriesToWatch;
		WwiseDBArray<WwiseDBString> LanguageDirectories;
		WwiseDBArray<WwiseDBString> AutoSoundBankDirectories;
		WwiseDBString MediaDirectory;

		bool IsValid() const;

		void Append(FPlatformFiles&& Rhs);
	};

	FGeneratedRootFiles GeneratedRootFiles;
	WwiseDBMap<WwiseDBSharedPlatformId, FPlatformFiles> Platforms;
	WwiseMetadataSharedRootFileConstPtr ProjectInfo;

	bool IsValid() const;
};
