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

#include "AdapterTypes/WwiseString.h"

#include <algorithm>
#include <filesystem>
#include <string>

#include "AdapterTypes/WwiseDataTypesAdapter.h"

#ifdef WWISE_DB_TEXT
#error There is already an implementation of StandardString in use
#endif

inline bool CaseInsensitiveCompare(const char& Lhs, const char& Rhs)
{
	return tolower(Lhs) == tolower(Rhs);
}

#ifdef _WIN32
inline constexpr char DirectorySeparator = '\\';
#else
inline constexpr char DirectorySeparator = '/';
#endif

// An implementation of IWwiseString which holds a UTF-8 string for use within the Wwise Project Database
class WwiseStandardString : public IWwiseString<std::string>
{
public:
	WwiseStandardString(){}

#ifdef _WIN32
	constexpr WwiseStandardString(const char* InChars)
	{
		String = InChars;
	}
#else
	WwiseStandardString(const char* InChars)
	{
		String = InChars;
	}
#endif

	WwiseStandardString(const std::string& InString)
	{
		String = InString;
	}

	WwiseStandardString(const std::filesystem::path& InString)
	{
		String = reinterpret_cast<const char*>(InString.u8string().c_str());
		MakeStandardFilename();
	}

	static WwiseStandardString Empty;

	bool IsEmpty() const override
	{
		return String.empty();
	}

	unsigned int Size() const override
	{
		return (unsigned int)String.size();
	}

	unsigned int AllocatedSize() const override
	{
		return (unsigned int)String.capacity();
	}

	void MakeStandardFilename()
	{
		std::replace(String.begin(), String.end(), '\\', '/');
	}

	WwiseStandardString GetFileName() const
	{
		auto index = String.find_last_of("/\\");
		if(index == std::string::npos)
		{
			return String;
		}
		return String.substr(index + 1);
	}

	WwiseStandardString GetPath() const
	{
		auto index = String.find_last_of("/\\");
		if(index == std::string::npos)
		{
			return WwiseStandardString();
		}
		return String.substr(0, index);
	}

	WwiseStandardString GetExtension() const
	{
		auto index = String.find_last_of('.');
		if(index == std::string::npos)
		{
			return WwiseStandardString();
		}
		return String.substr(index + 1);
	}

	void CollapseRelativeDirectories()
	{
		auto index = String.rfind("/..");
		std::string SubString = String;
		while(index != std::string::npos)
		{
			SubString = String.substr(0, index);
			auto previousIndex = SubString.rfind('/');
			if(previousIndex == std::string::npos)
			{
				previousIndex = 0;
			}
			
			int removeCount = 1;
			std::string Folder;
			while(previousIndex != std::string::npos)
			{
				previousIndex = SubString.rfind("/..");
				if(previousIndex < SubString.size() - 3 || previousIndex == std::string::npos)
				{
					break;
				}
				SubString = SubString.substr(0, previousIndex);
				removeCount++;
			}
			bool end = false;
			for(int i = 0; i < removeCount; i++)
			{
				if(SubString.empty())
				{
					removeCount = i;
					end = true;
					break;
				}
				index = SubString.rfind('/');
				if(index == std::string::npos)
				{
					index = 0;
				}
				std::string NewSubStr = SubString.substr(index);
				NewSubStr.append(Folder);
				Folder = NewSubStr;
				SubString = SubString.substr(0, index);
			}
			std::string StringToRemove;
			for(int i = 0; i < removeCount; i++)
			{
				StringToRemove.append("/..");
			}
			Folder.append(StringToRemove);
			String.replace(String.find(Folder), Folder.length(), "");
			index = String.rfind("/..");
			if(end)
			{
				return;
			}
		}
	}

	bool StartsWith(const WwiseStandardString& c) const
	{
		if(c.Size() > String.size())
		{
			return false;
		}
		for(unsigned int i = 0; i < c.Size(); i++)
		{
			if(c.String[i] != String[i])
			{
				return false;
			}
		}
		return true;
	}

	bool IsRelative() const
	{
		if(String.length() < 3)
		{
			return false;
		}
#ifdef _WIN32
		bool bIsUNCPath = String[0] == DirectorySeparator && String[1] == DirectorySeparator;
		bool bIsDOSPath = isalpha(String[0]) && String[1] == ':' && (String[2] == DirectorySeparator || String[2] == '/');
		return !bIsUNCPath && !bIsDOSPath;
#else
		return !(String[0] == DirectorySeparator || String[0] == '~');
#endif
	}

	bool Equals(const WwiseStandardString& Rhs, EStringCompareType Compare) const
	{
		if(Size() != Rhs.Size())
		{
			return false;
		}
		
		if(Compare == CaseSensitive)
		{
			return String == Rhs.String;	
		}
		
		return std::equal(String.begin(), String.end(), Rhs.String.begin(), Rhs.String.end(), CaseInsensitiveCompare);
	}

	bool operator==(const WwiseStandardString& Rhs) const
	{
		return this->String == Rhs.String;
	}
	
	bool operator!=(const WwiseStandardString& Rhs) const
	{
		return !(*this == Rhs);
	}

	bool operator==(const char* Rhs) const
	{
		return this->String == Rhs;
	}

	const char* operator*() const
	{
		return String.c_str();
	}

	WwiseStandardString operator/(const WwiseStandardString& Rhs) const
	{
		return WwiseStandardString(String + "/" + Rhs.String);
	}
};

inline WwiseStandardString WwiseStandardString::Empty("");

inline WwiseDBShortId GetTypeHash(const WwiseStandardString& InString)
{
	return (unsigned int)std::hash<std::string>{}(InString.String);
}

template <>
struct std::hash<WwiseStandardString>
{
	std::size_t operator()(const WwiseStandardString& InString) const noexcept
	{
		return GetTypeHash(InString);
	}
};

#ifdef _WIN32
constexpr WwiseStandardString operator ""_wwise_db(const char* c, size_t size)
{
	return WwiseStandardString{c};
}
#else
inline WwiseStandardString operator ""_wwise_db(const char* c, size_t size)
{
	return WwiseStandardString{c};
}
#endif