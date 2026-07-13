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
#include <filesystem>

#include "WwiseGeneratedFiles.h"

template<typename Type>
class IWwiseDirectoryVisitor
{
public:
	Type* DirectoryVisitor;

	IWwiseDirectoryVisitor() : DirectoryVisitor(nullptr) {}
	IWwiseDirectoryVisitor(Type* DirectoryVisitor) : DirectoryVisitor(DirectoryVisitor){}
	~IWwiseDirectoryVisitor()
	{
		delete DirectoryVisitor;
		DirectoryVisitor = nullptr;
	}
	
	IWwiseDirectoryVisitor(IWwiseDirectoryVisitor&& Other) noexcept
	: DirectoryVisitor(std::exchange(Other.DirectoryVisitor, nullptr)){}

	IWwiseDirectoryVisitor& operator=(IWwiseDirectoryVisitor&& Other) noexcept
	{
		if (this != &Other)
		{
			delete DirectoryVisitor;
			DirectoryVisitor = std::exchange(Other.DirectoryVisitor, nullptr);
		}
		return *this;
	}
	
	IWwiseDirectoryVisitor(const IWwiseDirectoryVisitor& other) = delete;
	IWwiseDirectoryVisitor& operator=(const IWwiseDirectoryVisitor& other) = delete;

	static bool IterateDirectory(Type* Visitor, const std::filesystem::path& InPath, bool bIsRecursive = false)
	{
		if(!std::filesystem::exists(InPath))
		{
			return false;
		}
		bool bResult = true;
		if(bIsRecursive)
		{
			for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(InPath))
			{
				if(!Visitor->Visit(dirEntry.path(), dirEntry.is_directory()))
				{
					bResult = false;
				}
			}
		}
		else
		{
			for (const auto& dirEntry : std::filesystem::directory_iterator(InPath))
			{
				if(!Visitor->Visit(dirEntry.path(), dirEntry.is_directory()))
				{
					bResult = false;
				}
			}
		}

		return bResult;
	}

	auto Get()
	{
		return DirectoryVisitor->Get();
	}

	static std::filesystem::file_time_type GetTimeStamp(const std::filesystem::path& InPath)
	{
		if(!std::filesystem::exists(InPath))
		{
			return {};
		}
		return std::filesystem::last_write_time(InPath);
	}
};
