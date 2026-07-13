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

#include "Metadata/WwiseMetadataAcousticTexture.h"
#include "Metadata/WwiseMetadataBus.h"
#include "Metadata/WwiseMetadataCollections.h"
#include "Metadata/WwiseMetadataDialogue.h"
#include "Metadata/WwiseMetadataEvent.h"
#include "Metadata/WwiseMetadataExternalSource.h"
#include "Metadata/WwiseMetadataGameParameter.h"
#include "Metadata/WwiseMetadataMedia.h"
#include "Metadata/WwiseMetadataPluginGroup.h"
#include "Metadata/WwiseMetadataStateGroup.h"
#include "Metadata/WwiseMetadataSwitchGroup.h"
#include "Metadata/WwiseMetadataTrigger.h"
#include "Ref/WwiseRefCollections.h"

struct WwiseMetadataSoundBankReference : public WwiseMetadataLoadable
{
	WwiseDBShortId Id;
	WwiseDBGuid GUID;
	WwiseDBString Language;

	WwiseMetadataSoundBankReference();
	WwiseMetadataSoundBankReference(WwiseMetadataLoader& Loader);
};

enum class EMetadataSoundBankType : WwiseDBShortId
{
	User = 0,
	Event = 30,
	Bus = 31,
	Unknown = (WwiseDBShortId)-1
};

struct WwiseMetadataSoundBankAttributes : public WwiseMetadataSoundBankReference
{
	WwiseDBShortId Align;
	bool bDeviceMemory;
	WwiseDBGuid Hash;
	EMetadataSoundBankType Type;

	WwiseMetadataSoundBankAttributes();
	WwiseMetadataSoundBankAttributes(WwiseMetadataLoader& Loader);

private:
	static EMetadataSoundBankType TypeFromString(const WwiseDBString& TypeString);
};

struct WwiseMetadataSoundBank : public WwiseMetadataSoundBankAttributes
{
	WwiseDBString ObjectPath;
	int Color;
	WwiseDBString ShortName;
	WwiseDBString Path;

	WwiseDBArray<WwiseMetadataMedia> Media;
	WwiseStandardSharedPtr<WwiseMetadataPluginGroup> Plugins;
	WwiseDBArray<WwiseMetadataEvent> Events;
	WwiseDBArray<WwiseMetadataDialogueEvent> DialogueEvents;
	WwiseDBArray<WwiseMetadataBus> Busses;
	WwiseDBArray<WwiseMetadataBus> AuxBusses;
	WwiseDBArray<WwiseMetadataGameParameter> GameParameters;
	WwiseDBArray<WwiseMetadataStateGroup> StateGroups;
	WwiseDBArray<WwiseMetadataSwitchGroup> SwitchGroups;
	WwiseDBArray<WwiseMetadataTrigger> Triggers;
	WwiseDBArray<WwiseMetadataExternalSource> ExternalSources;
	WwiseDBArray<WwiseMetadataAcousticTexture> AcousticTextures;

	WwiseMetadataSoundBank();
	WwiseMetadataSoundBank(WwiseMetadataLoader& Loader);
	WwiseDBSet<WwiseMetadataDialogueArgument> GetAllDialogueArguments() const;
	WwiseDBSet<WwiseMetadataStateWithGroup> GetAllStates() const;
	WwiseDBSet<WwiseMetadataSwitchWithGroup> GetAllSwitches() const;
	bool IsInitBank() const
	{
		return bIsInitBank;
	}
	bool ContainsMedia() const
	{
		return Media.ContainsByPredicate([](const WwiseMetadataMedia& MediaToTest)
		{
			return MediaToTest.Location == WwiseMetadataMediaLocation::Memory;
		});
	}

protected:
	bool bIsInitBank;

};
