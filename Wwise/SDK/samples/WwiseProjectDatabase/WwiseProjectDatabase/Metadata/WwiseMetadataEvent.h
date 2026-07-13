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

#include "Metadata/WwiseMetadataActionEntries.h"
#include "Metadata/WwiseMetadataBus.h"
#include "Metadata/WwiseMetadataSwitchContainer.h"

enum class WwiseMetadataEventDurationType : WwiseDBShortId
{
	OneShot = 0,
	Unknown = 1,
	Infinite = 2,
	Mixed = 3
};

struct WwiseMetadataEventReference : public WwiseMetadataBasicReference
{
	float MaxAttenuation;
	WwiseMetadataEventDurationType DurationType;
	float DurationMin;
	float DurationMax;

	WwiseMetadataEventReference(WwiseMetadataLoader& Loader);

private:
	static WwiseMetadataEventDurationType DurationTypeFromString(const WwiseDBString& TypeString);
};

struct WwiseMetadataEvent : public WwiseMetadataEventReference
{
	WwiseDBArray<WwiseMetadataMediaReference> MediaRefs;
	WwiseDBArray<WwiseMetadataExternalSourceReference> ExternalSourceRefs;
	WwiseStandardSharedPtr<WwiseMetadataPluginReferenceGroup> PluginRefs;
	WwiseDBArray<WwiseMetadataActionSetFXEntry> ActionSetFX; 
	WwiseMetadataEvent(WwiseMetadataLoader& Loader);
	WwiseDBArray<WwiseMetadataBusReference> AuxBusRefs;
	WwiseDBArray<WwiseMetadataSwitchContainer> SwitchContainers;
	WwiseDBArray<WwiseMetadataActionPostEventEntry> ActionPostEvent;
	WwiseDBArray<WwiseMetadataActionSetStateEntry> ActionSetState;
	WwiseDBArray<WwiseMetadataActionSetSwitchEntry> ActionSetSwitch;
	WwiseDBArray<WwiseMetadataActionTriggerEntry> ActionTrigger;


	bool IsMandatory() const;
};
