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

#include "Metadata/WwiseMetadataExternalSource.h"
#include "Metadata/WwiseMetadataSwitchValue.h"

struct WwiseMetadataSwitchContainer : public WwiseMetadataLoadable
{
	WwiseMetadataSwitchValue SwitchValue;
	WwiseDBArray<WwiseMetadataMediaReference> MediaRefs;
	WwiseDBArray<WwiseMetadataExternalSourceReference> ExternalSourceRefs;
	WwiseMetadataPluginReferenceGroup* PluginRefs;
	WwiseDBArray<WwiseMetadataSwitchContainer> Children;

	WwiseMetadataSwitchContainer(WwiseMetadataLoader& Loader);
	void GetAllMedia(WwiseDBSet<const WwiseMetadataMediaReference*>& outMedias) const;
	WwiseDBSet<WwiseMetadataExternalSourceReference> GetAllExternalSources() const;
	WwiseDBSet<WwiseMetadataPluginReference> GetAllCustomPlugins() const;
	WwiseDBSet<WwiseMetadataPluginReference> GetAllPluginShareSets() const;
	WwiseDBSet<WwiseMetadataPluginReference> GetAllAudioDevices() const;
	unsigned int GetNumberOfMedias() const;
};
