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

#include "Metadata/WwiseMetadataSwitchContainer.h"
#include "Metadata/WwiseMetadataPluginGroup.h"
#include "Metadata/WwiseMetadataLoader.h"

WwiseMetadataSwitchContainer::WwiseMetadataSwitchContainer(WwiseMetadataLoader& Loader) :
	SwitchValue(Loader.GetLoaderObject<WwiseMetadataSwitchValue>(this, "SwitchValue"_wwise_db)),
	MediaRefs(Loader.GetArray<WwiseMetadataMediaReference>(this, "MediaRefs"_wwise_db)),
	ExternalSourceRefs(Loader.GetArray<WwiseMetadataExternalSourceReference>(this, "ExternalSourceRefs"_wwise_db)),
	PluginRefs(Loader.GetObjectPtr<WwiseMetadataPluginReferenceGroup>(this, "PluginRefs"_wwise_db)),
	Children(Loader.GetArray<WwiseMetadataSwitchContainer>(this, "Children"_wwise_db))
{
	Loader.LogParsed("SwitchContainer"_wwise_db);
}

void WwiseMetadataSwitchContainer::GetAllMedia(WwiseDBSet<const WwiseMetadataMediaReference*>& outMedias) const
{
	for (const auto& MediaRef : MediaRefs)
	{
		outMedias.Add(&MediaRef);
	}
	for (const auto& Child : Children)
	{
		Child.GetAllMedia(outMedias);
	}
}

WwiseDBSet<WwiseMetadataExternalSourceReference> WwiseMetadataSwitchContainer::GetAllExternalSources() const
{
	WwiseDBSet<WwiseMetadataExternalSourceReference> Result(ExternalSourceRefs);
	for (const auto& Child : Children)
	{
		Result.Append(Child.GetAllExternalSources());
	}
	return Result;
}

unsigned int WwiseMetadataSwitchContainer::GetNumberOfMedias() const
{
	int NumberOfMedias = MediaRefs.Size();
	for (const auto& Child : Children)
	{
		NumberOfMedias += Child.GetNumberOfMedias();
	}
	return NumberOfMedias;
}

WwiseDBSet<WwiseMetadataPluginReference> WwiseMetadataSwitchContainer::GetAllCustomPlugins() const
{
	if (!PluginRefs)
	{
		return {};
	}
	WwiseDBSet<WwiseMetadataPluginReference> Result(PluginRefs->Custom);
	for (const auto& Child : Children)
	{
		Result.Append(Child.GetAllCustomPlugins());
	}
	return Result;
}

WwiseDBSet<WwiseMetadataPluginReference> WwiseMetadataSwitchContainer::GetAllPluginShareSets() const
{
	if (!PluginRefs)
	{
		return {};
	}
	WwiseDBSet<WwiseMetadataPluginReference> Result(PluginRefs->ShareSets);
	for (const auto& Child : Children)
	{
		Result.Append(Child.GetAllPluginShareSets());
	}
	return Result;
}

WwiseDBSet<WwiseMetadataPluginReference> WwiseMetadataSwitchContainer::GetAllAudioDevices() const
{
	if (!PluginRefs)
	{
		return {};
	}
	WwiseDBSet<WwiseMetadataPluginReference> Result(PluginRefs->AudioDevices);
	for (const auto& Child : Children)
	{
		Result.Append(Child.GetAllAudioDevices());
	}
	return Result;
}
