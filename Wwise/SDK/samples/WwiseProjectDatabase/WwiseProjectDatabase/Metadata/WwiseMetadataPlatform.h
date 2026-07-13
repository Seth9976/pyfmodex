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

#include "Metadata/WwiseMetadataLoadable.h"

struct WwiseMetadataPlatformAttributes : public WwiseMetadataLoadable
{
	WwiseDBString Name;
	WwiseDBString BasePlatform;
	WwiseDBString Generator;

	WwiseMetadataPlatformAttributes();
	WwiseMetadataPlatformAttributes(WwiseMetadataLoader& Loader);
};

struct WwiseMetadataPlatformReference : public WwiseMetadataLoadable
{
	WwiseDBString Name;
	WwiseDBGuid GUID;
	WwiseDBString BasePlatform;
	WwiseDBGuid BasePlatformGUID;
	WwiseDBString Path;

	WwiseMetadataPlatformReference(WwiseMetadataLoader& Loader);
};

struct WwiseMetadataPlatform : public WwiseMetadataPlatformAttributes
{
	WwiseMetadataPlatform();
	WwiseMetadataPlatform(WwiseMetadataLoader& Loader);
};
