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

#include "WwiseDirectoryVisitor.h"

#include "Metadata/WwiseMetadataRootFile.h"
#include "Metadata/WwiseMetadataProjectInfo.h"
#include "Metadata/WwiseMetadataPlatform.h"
#include "Metadata/WwiseMetadataLanguage.h"

#include "AdapterTypes/IWwiseDirectoryVisitor.h"
#include "AdapterTypes/WwiseWrapperTypes.h"


//
// FSoundBankVisitor
//
class WwiseDirectoryVisitor::FSoundBankVisitor : public WwisePlatformVisitor
{
public:
	FSoundBankVisitor()
	{
	}

	bool Visit(const std::filesystem::path& FilenameOrDirectory, bool bIsDirectory);
	WwiseGeneratedFiles::FPlatformFiles& Get() override;

	WwiseGeneratedFiles::FPlatformFiles Result;
};

//
// FMediaVisitor
//
class WwiseDirectoryVisitor::FMediaVisitor : public WwisePlatformVisitor
{
public:
	FMediaVisitor()
	{
	}

	bool Visit(const std::filesystem::path& FilenameOrDirectory, bool bIsDirectory);
	WwiseGeneratedFiles::FPlatformFiles& Get() override;

	WwiseGeneratedFiles::FPlatformFiles Result;
};

//
// WwiseDirectoryVisitor
//
bool WwiseDirectoryVisitor::Visit(const std::filesystem::path& FilenameOrDirectory, bool bIsDirectory)
{
	// make sure all paths are "standardized" so the other end can match up with it's own standardized paths
	WwiseDBString RelativeFilename = WwiseDBString(FilenameOrDirectory);
	RelativeFilename.MakeStandardFilename();
	const auto Filename = FilenameOrDirectory.filename().filename().u8string();

	if (Filename.starts_with(u8"."))
	{
		WWISE_DB_LOG(Verbose, "[WwiseDirectoryVisitor] Skipping: %s", *RelativeFilename);
		return true;
	}

	WWISE_DB_LOG(VeryVerbose, "[WwiseDirectoryVisitor] Visiting %s", *RelativeFilename);

	if (bIsDirectory)
	{
		// Skip directories, they are to be processed by ProjectInfo.json's Path
		return true;
	}

	WwiseGeneratedFiles::FileTuple FileToAdd(*RelativeFilename, IWwiseDirectoryVisitor<WwiseDirectoryVisitor>::GetTimeStamp(FilenameOrDirectory));
	const auto Extension = RelativeFilename.GetExtension();

	if (Filename == u8"ProjectInfo.json")
	{
		WWISE_DB_LOG(Verbose, "Found ProjectInfo: %s", *RelativeFilename);

		// We need to retrieve the Path from ProjectInfo. Parse this file immediately
		// (will be parsed twice. Now once, and officially later - since the file is small, it's not a big worry)
		auto Root = WwiseMetadataRootFile::LoadFile(WwiseDBString(*RelativeFilename));
		if (!Root || !Root->ProjectInfo)
		{
			WWISE_DB_LOG(Error, "Could not read ProjectInfo to retrieve paths.", *RelativeFilename);
			return true;
		}

		GeneratedDirectory.ProjectInfo = Root;

		auto Path = WwiseDBString(FilenameOrDirectory.parent_path());

		auto& Platforms = Root->ProjectInfo->Platforms;
		bool bFoundPlatform = false;
		if (!PlatformName)
		{
			WWISE_DB_LOG(Log, "Skipping loading all platforms");
		}
		else
		{
			for (auto& Platform : Platforms)
			{
				if (PlatformName->Equals(Platform.Name, EStringCompareType::IgnoreCase))
				{
					bFoundPlatform = true;
					break;
				}
			}

			WWISE_DB_CLOG(!bFoundPlatform, Log, "Requested platform not found: %s", **PlatformName);
		}

		if (bFoundPlatform)
		{
			for (auto& Platform : Platforms)
			{
				WwiseDBString PlatformPath = Platform.Path.IsRelative() ? Path / Platform.Path : Platform.Path;
				PlatformPath.CollapseRelativeDirectories();

				WwiseDBString RequestedPlatformPath = ""_wwise_db; 

				if (PlatformPath.IsRelative())
				{
					RequestedPlatformPath = Path / Platform.Path;
				}
				else
				{
					RequestedPlatformPath = PlatformPath;
				}

				if (!PlatformName->Equals(Platform.Name, EStringCompareType::IgnoreCase))
				{
					WWISE_DB_LOG(Verbose, "Skipping platform %s", *Platform.Name);
					continue;
				}

				WWISE_DB_LOG(Verbose, "Visiting platform %s at: %s", *Platform.Name, *Platform.Path);
				
				Futures.Add(WwiseAsync([this, Platform, PlatformPath] {
					WwiseDBSharedPlatformId PlatformRef;
					PlatformRef.Platform = WwiseDBSharedPtr<WwiseDBPlatformId>(new WwiseDBPlatformId(Platform.GUID, *Platform.Name, *Platform.Path));
					auto RootVisitor = IWwiseDirectoryVisitor<FPlatformRootDirectoryVisitor>(new FPlatformRootDirectoryVisitor(PlatformRef));
					std::filesystem::path path (reinterpret_cast<const char8_t*>(*PlatformPath));
					if (!IWwiseDirectoryVisitor<FPlatformRootDirectoryVisitor>::IterateDirectory(RootVisitor.DirectoryVisitor, path) ||
						!RootVisitor.DirectoryVisitor->StartJobIfValid())
					{
						delete RootVisitor.DirectoryVisitor;
						RootVisitor.DirectoryVisitor = nullptr;
						WWISE_DB_LOG(Warning, "Could not find generated platform %s at: %s", *PlatformRef.GetPlatformName(), *PlatformPath);
					}

					return RootVisitor;
				}));
			}
		}

		GeneratedDirectory.GeneratedRootFiles.ProjectInfoFile = std::move(FileToAdd.Tuple);
	}
	else if (Filename == u8"Wwise_IDs.h")
	{
		WWISE_DB_LOG(Verbose, "Found Wwise IDs: %s", *RelativeFilename);
		GeneratedDirectory.GeneratedRootFiles.WwiseIDsFile = std::move(FileToAdd.Tuple);
	}
	else if (Filename == u8"SoundBanksGeneration.log"
		|| Extension.Equals("xml"_wwise_db, EStringCompareType::IgnoreCase))
	{
		// Nothing to do
	}

	return true;
}

WwiseGeneratedFiles& WwiseDirectoryVisitor::Get()
{
	for (auto& Future : Futures)
	{
		IWwiseDirectoryVisitor<FPlatformRootDirectoryVisitor> Result = Future.Get();
		if (Result.DirectoryVisitor)
		{
			auto& PlatformFiles = Result.DirectoryVisitor->Get();
			if (PlatformFiles.IsValid())
			{
				GeneratedDirectory.Platforms.Add(Result.DirectoryVisitor->Platform, PlatformFiles);
			}
		}
	}
	Futures.Empty();
	return GeneratedDirectory;
}



//
// FPlatformRootDirectoryVisitor
//
bool WwiseDirectoryVisitor::FPlatformRootDirectoryVisitor::Visit(const std::filesystem::path& FilenameOrDirectory, bool bIsDirectory)
{
	// make sure all paths are "standardized" so the other end can match up with its own standardized paths
	WwiseDBString RelativeFilename = WwiseDBString(FilenameOrDirectory);
	RelativeFilename.MakeStandardFilename();
	const auto Filename = RelativeFilename.GetFileName();

	if (Filename.StartsWith("."_wwise_db))
	{
		WWISE_DB_LOG(Verbose, "[RootFilesVisitor] Skipping: %s", *RelativeFilename);
		return true;
	}

	WWISE_DB_LOG(VeryVerbose, "[RootFilesVisitor] Visiting %s", *RelativeFilename);

	if (bIsDirectory)
	{
		PlatformFiles.DirectoriesToWatch.Add(RelativeFilename);

		if (Filename.Equals("Media"_wwise_db, EStringCompareType::IgnoreCase))
		{
			WWISE_DB_LOG(Verbose, "Found media directory: %s", *RelativeFilename);
			PlatformFiles.MediaDirectory = RelativeFilename;
		}
		else if (Filename.Equals("Bus"_wwise_db, EStringCompareType::IgnoreCase)
			|| Filename.Equals("Event"_wwise_db, EStringCompareType::IgnoreCase))
		{
			WWISE_DB_LOG(Verbose, "Found auto SoundBank directory: %s", *RelativeFilename);
			PlatformFiles.AutoSoundBankDirectories.Add(RelativeFilename);
		}
		else {
			WWISE_DB_LOG(Verbose, "Found language directory: %s", *RelativeFilename);
			PlatformFiles.LanguageDirectories.Add(RelativeFilename);
		}

		return true;
	}

	WwiseGeneratedFiles::FileTuple FileToAdd(RelativeFilename, IWwiseDirectoryVisitor<WwiseDirectoryVisitor>::GetTimeStamp(FilenameOrDirectory));
	const auto Extension = RelativeFilename.GetExtension();

	if (!Extension.Equals("json"_wwise_db, EStringCompareType::IgnoreCase) 
		&& !Extension.Equals("txt"_wwise_db, EStringCompareType::IgnoreCase)
		&& !Extension.Equals("bnk"_wwise_db, EStringCompareType::IgnoreCase)
		&& !Extension.Equals("xml"_wwise_db, EStringCompareType::IgnoreCase))
	{
		WWISE_DB_LOG(VeryVerbose, "Adding extra file: %s", *RelativeFilename);
		PlatformFiles.ExtraFiles.Add(std::move(FileToAdd));
		return true;
	}

	if (Extension.Equals("bnk"_wwise_db, EStringCompareType::IgnoreCase))
	{
		WWISE_DB_LOG(VeryVerbose, "Adding SoundBank file: %s", *RelativeFilename);
		PlatformFiles.SoundBankFiles.Add(std::move(FileToAdd));
		return true;
	}
	else if (Extension.Equals("xml"_wwise_db, EStringCompareType::IgnoreCase)
		|| Extension.Equals("txt"_wwise_db, EStringCompareType::IgnoreCase))
	{
		WWISE_DB_LOG(VeryVerbose, "Skipping file: %s", *RelativeFilename);
		return true;
	}
	else if (Filename.Equals("SoundbanksInfo.json"_wwise_db, EStringCompareType::IgnoreCase))
	{
		WWISE_DB_LOG(Verbose, "Found monolithic SoundBank info: %s", *RelativeFilename);
		PlatformFiles.SoundbanksInfoFile = std::move(FileToAdd);
	}
	else if (Filename.Equals("PlatformInfo.json"_wwise_db, EStringCompareType::IgnoreCase))
	{
		WWISE_DB_LOG(Verbose, "Found platform info: %s", *RelativeFilename);
		PlatformFiles.PlatformInfoFile = std::move(FileToAdd);
	}
	else if (Filename.Equals("PluginInfo.json"_wwise_db, EStringCompareType::IgnoreCase))
	{
		WWISE_DB_LOG(Verbose, "Found plugin info: %s", *RelativeFilename);
		PlatformFiles.PluginInfoFile = std::move(FileToAdd);
	}
	else
	{
		WWISE_DB_LOG(VeryVerbose, "Adding metadata file: %s", *RelativeFilename);
		PlatformFiles.MetadataFiles.Add(std::move(FileToAdd));
	}

	return true;
}

bool WwiseDirectoryVisitor::FPlatformRootDirectoryVisitor::StartJobIfValid()
{
	if (!PlatformFiles.IsValid())
	{
		return false;
	}

	if (!PlatformFiles.MediaDirectory.IsEmpty())
	{
		const auto& Elem = PlatformFiles.MediaDirectory;
		Futures.Add(WwiseAsync([this, Elem] {
			WWISE_DB_LOG(Verbose, "Visiting media directory: %s", *Elem);
			auto MediaVisitor = IWwiseDirectoryVisitor<FMediaVisitor>(new FMediaVisitor());
			IWwiseDirectoryVisitor<FMediaVisitor>::IterateDirectory(MediaVisitor.DirectoryVisitor, std::filesystem::path(Elem.String));
			auto BaseVisitor = IWwiseDirectoryVisitor<WwisePlatformVisitor>(
				(WwisePlatformVisitor*)std::exchange(MediaVisitor.DirectoryVisitor, nullptr)
			);
			return BaseVisitor;
		}));
	}
	for (const auto& Elem : PlatformFiles.AutoSoundBankDirectories)
	{
		Futures.Add(WwiseAsync([this, Elem] {
			WWISE_DB_LOG(Verbose, "Visiting auto SoundBank directory: %s", *Elem);
			auto SoundBankVisitor = IWwiseDirectoryVisitor<FSoundBankVisitor>(new FSoundBankVisitor());
			IWwiseDirectoryVisitor<FSoundBankVisitor>::IterateDirectory(SoundBankVisitor.DirectoryVisitor, std::filesystem::path(Elem.String));
			auto BaseVisitor = IWwiseDirectoryVisitor<WwisePlatformVisitor>(
				(WwisePlatformVisitor*)std::exchange(SoundBankVisitor.DirectoryVisitor, nullptr)
			);
			return BaseVisitor;
		}));
	}

	for (const auto& Elem : PlatformFiles.LanguageDirectories)
	{
		Futures.Add(WwiseAsync([this, Elem] {
			WWISE_DB_LOG(Verbose, "Visiting language directory: %s", *Elem);
			auto SoundBankVisitor = IWwiseDirectoryVisitor<FSoundBankVisitor>(new FSoundBankVisitor());
			IWwiseDirectoryVisitor<FSoundBankVisitor>::IterateDirectory(SoundBankVisitor.DirectoryVisitor, std::filesystem::path(Elem.String));
			auto BaseVisitor = IWwiseDirectoryVisitor<WwisePlatformVisitor>(
				(WwisePlatformVisitor*)std::exchange(SoundBankVisitor.DirectoryVisitor, nullptr)
			);
			return BaseVisitor;
		}));
	}

	return true;
}

WwiseGeneratedFiles::FPlatformFiles& WwiseDirectoryVisitor::FPlatformRootDirectoryVisitor::Get()
{
	for (auto& Future : Futures)
	{
		IWwiseDirectoryVisitor<WwisePlatformVisitor> Result = Future.Get();
		if (Result.DirectoryVisitor)
		{
			PlatformFiles.Append(std::move(Result.DirectoryVisitor->Get()));
		}
	}
	Futures.Empty();
	return PlatformFiles;
}

//
// FSoundBankVisitor
//
bool WwiseDirectoryVisitor::FSoundBankVisitor::Visit(const std::filesystem::path& FilenameOrDirectory, bool bIsDirectory)
{
	// make sure all paths are "standardized" so the other end can match up with its own standardized paths
	WwiseDBString RelativeFilename = WwiseDBString(FilenameOrDirectory);
	RelativeFilename.MakeStandardFilename();

	const auto Filename = RelativeFilename.GetFileName();
	const auto Extension = RelativeFilename.GetExtension();

	if (Filename.StartsWith("."_wwise_db))
	{
		WWISE_DB_LOG(Verbose, "[SoundBankVisitor] Skipping: %s", *RelativeFilename);
		return true;
	}

	WWISE_DB_LOG(VeryVerbose, "[SoundBankVisitor] Visiting %s", *RelativeFilename);

	if (bIsDirectory)
	{
		WWISE_DB_LOG(VeryVerbose, "Iterating folder: %s", *RelativeFilename);
		Result.DirectoriesToWatch.Add(RelativeFilename);
		IWwiseDirectoryVisitor<FSoundBankVisitor>::IterateDirectory(this, FilenameOrDirectory);
		return true;
	}

	WwiseGeneratedFiles::FileTuple FileToAdd(RelativeFilename, IWwiseDirectoryVisitor<WwiseDirectoryVisitor>::GetTimeStamp(FilenameOrDirectory));

	if (Extension.Equals("json"_wwise_db, EStringCompareType::IgnoreCase))
	{
		WWISE_DB_LOG(VeryVerbose, "Adding metadata file: %s", *RelativeFilename);
		Result.MetadataFiles.Add(std::move(FileToAdd));
		return true;
	}
	else if (Extension.Equals("bnk"_wwise_db, EStringCompareType::IgnoreCase))
	{
		WWISE_DB_LOG(VeryVerbose, "Adding SoundBank file: %s", *RelativeFilename);
		Result.SoundBankFiles.Add(std::move(FileToAdd));
		return true;
	}
	else if (Extension.Equals("xml"_wwise_db, EStringCompareType::IgnoreCase)
		|| Extension.Equals("txt"_wwise_db, EStringCompareType::IgnoreCase))
	{
		WWISE_DB_LOG(VeryVerbose, "Skipping file: %s", *RelativeFilename);
		return true;
	}
	else
	{
		WWISE_DB_LOG(VeryVerbose, "Adding extra file: %s", *RelativeFilename);
		Result.ExtraFiles.Add(std::move(FileToAdd));
		return true;
	}
}

WwiseGeneratedFiles::FPlatformFiles& WwiseDirectoryVisitor::FSoundBankVisitor::Get()
{
	return Result;
}

//
// FMediaVisitor
//
bool WwiseDirectoryVisitor::FMediaVisitor::Visit(const std::filesystem::path& FilenameOrDirectory, bool bIsDirectory)
{
	// make sure all paths are "standardized" so the other end can match up with it's own standardized paths
	WwiseDBString RelativeFilename = WwiseDBString(FilenameOrDirectory);
	RelativeFilename.MakeStandardFilename();

	WWISE_DB_LOG(VeryVerbose, "[MediaVisitor] Visiting %s", *RelativeFilename);

	if (bIsDirectory)
	{
		Result.DirectoriesToWatch.Add(RelativeFilename);
		IWwiseDirectoryVisitor<FMediaVisitor>::IterateDirectory(this, FilenameOrDirectory);
		return true;
	}
	WwiseGeneratedFiles::FileTuple FileToAdd(RelativeFilename, IWwiseDirectoryVisitor<WwiseDirectoryVisitor>::GetTimeStamp(FilenameOrDirectory));
	const auto Extension = RelativeFilename.GetExtension();

	if (Extension.Equals("wem"_wwise_db, EStringCompareType::IgnoreCase))
	{
		WWISE_DB_LOG(VeryVerbose, "Adding media file: %s", *RelativeFilename);
		Result.MediaFiles.Add(std::move(FileToAdd));
	}
	else
	{
		WWISE_DB_LOG(Log, "Adding unexpected extra file: %s", *RelativeFilename);
		Result.ExtraFiles.Add(std::move(FileToAdd));
	}
	return true;
}

WwiseGeneratedFiles::FPlatformFiles& WwiseDirectoryVisitor::FMediaVisitor::Get()
{
	return Result;
}
