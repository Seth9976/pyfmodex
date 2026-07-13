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

#include "WwiseGeneratedFiles.h"

#include "AdapterTypes/IWwiseDirectoryVisitor.h"

class WwisePlatformVisitor
{
public:
	virtual ~WwisePlatformVisitor() = default;
	virtual WwiseGeneratedFiles::FPlatformFiles& Get() = 0;
};

class WwiseDirectoryVisitor
{
	friend IWwiseDirectoryVisitor<WwiseDirectoryVisitor>;
public:
	WwiseDirectoryVisitor(const WwiseDBString* InPlatformName = nullptr) :
		PlatformName(InPlatformName)
	{}

	WwiseGeneratedFiles& Get();
	bool Visit(const std::filesystem::path& FilenameOrDirectory, bool bIsDirectory);

private:
	WwiseGeneratedFiles GeneratedDirectory;
	

	//
	// FPlatformRootDirectoryVisitor
	//
	class FPlatformRootDirectoryVisitor : public WwisePlatformVisitor
	{
	public:
		FPlatformRootDirectoryVisitor(
			const WwiseDBSharedPlatformId& InPlatform) :
			Platform(InPlatform)
		{
		}
		~FPlatformRootDirectoryVisitor() = default;
		bool Visit(const std::filesystem::path& FilenameOrDirectory, bool bIsDirectory);
		bool StartJobIfValid();
		WwiseGeneratedFiles::FPlatformFiles& Get();

		const WwiseDBSharedPlatformId Platform;

		WwiseGeneratedFiles::FPlatformFiles PlatformFiles;
		WwiseDBArray<WwiseDBFuture<IWwiseDirectoryVisitor<WwisePlatformVisitor>>> Futures;
	};

	WwiseDBArray<WwiseDBFuture<IWwiseDirectoryVisitor<FPlatformRootDirectoryVisitor>>> Futures;

	const WwiseDBString* PlatformName;

	class FSoundBankVisitor;
	class FMediaVisitor;
	WwiseDirectoryVisitor& operator=(const WwiseDirectoryVisitor& Rhs) = delete;
	WwiseDirectoryVisitor(const WwiseDirectoryVisitor& Rhs) = delete;
};
