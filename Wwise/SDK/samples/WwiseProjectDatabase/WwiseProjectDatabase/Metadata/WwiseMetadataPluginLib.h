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

enum class WwiseMetadataPluginLibType : WwiseDBShortId
{
	Source,
	Effect,
	AudioDevice,
	Metadata,
	Unknown = (WwiseDBShortId)-1
};

struct WwiseMetadataPluginLibAttributes : public WwiseMetadataLoadable
{
	WwiseDBString LibName;
	WwiseDBShortId LibId;
	WwiseMetadataPluginLibType Type;
	WwiseDBString DLL;
	WwiseDBString StaticLib;

	WwiseMetadataPluginLibAttributes(WwiseMetadataLoader& Loader);

private:
	static WwiseMetadataPluginLibType TypeFromString(const WwiseDBString& TypeString);
};

struct WwiseMetadataPluginLib : public WwiseMetadataPluginLibAttributes
{
	WwiseMetadataPluginLib(WwiseMetadataLoader& Loader);
};
