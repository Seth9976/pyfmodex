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

#include "WwiseDatabaseIdentifiers.h"
#include "AdapterTypes/WwiseDataTypesAdapter.h"
#include "AdapterTypes/WwiseWrapperTypes.h"

struct WwiseDBPlatformId
{

	WwiseDBPlatformId() :
	PlatformGuid(),
	PlatformName()
	{}

	WwiseDBPlatformId(const WwiseDBGuid& InPlatformGuid, const WwiseDBString& InPlatformName) :
		PlatformGuid(InPlatformGuid),
		PlatformName(InPlatformName)
	{}

	WwiseDBPlatformId(const WwiseDBGuid& InPlatformGuid, const WwiseDBString& InPlatformName, const WwiseDBString& InGeneratedSoundBanksPath) :
	PlatformGuid(InPlatformGuid),
	PlatformName(InPlatformName),
	PathRelativeToGeneratedSoundBanks(InGeneratedSoundBanksPath)
	{}
	
	WwiseDBGuid PlatformGuid;

	WwiseDBString PlatformName;

	WwiseDBString PathRelativeToGeneratedSoundBanks;

	WwiseDBString ExternalSourceRootPath;
};

struct WwiseDBSharedPlatformId
{

	WwiseDBSharedPtr<WwiseDBPlatformId> Platform;

	WwiseDBSharedPlatformId() :
		Platform(new WwiseDBPlatformId)
	{}

	WwiseDBSharedPlatformId(const WwiseDBGuid& InPlatformGuid, const WwiseDBString& InPlatformName) :
		Platform(new WwiseDBPlatformId(InPlatformGuid, InPlatformName))
	{}

	WwiseDBSharedPlatformId(const WwiseDBGuid& InPlatformGuid, const WwiseDBString& InPlatformName, const WwiseDBString& InRelativePath) :
	Platform(new WwiseDBPlatformId(InPlatformGuid, InPlatformName, InRelativePath))
	{}

	const WwiseDBGuid& GetPlatformGuid() const
	{
		return Platform->PlatformGuid;
	}

	const WwiseDBString& GetPlatformName() const
	{
		return Platform->PlatformName;
	}

	bool operator<(const WwiseDBSharedPlatformId& Rhs) const
	{
		return Platform->PlatformGuid < Rhs.Platform->PlatformGuid;
	}
	
	bool operator==(const WwiseDBSharedPlatformId& Rhs) const
	{
		return Platform->PlatformGuid == Rhs.Platform->PlatformGuid;
	}

	bool operator!=(const WwiseDBSharedPlatformId& Rhs) const
	{
		return Platform->PlatformGuid != Rhs.Platform->PlatformGuid;
	}
};

template <>
struct std::hash<WwiseDBSharedPlatformId>
{
	std::size_t operator()(const WwiseDBSharedPlatformId& Id) const noexcept
	{
		return GetTypeHash(Id.Platform->PlatformGuid);
	}
};
