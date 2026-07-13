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

#include "Ref/WwiseRefDialogueEvent.h"

#include "Ref/WwiseRefCollections.h"
#include "Ref/WwiseRefDialogueArgument.h"
#include "Ref/WwiseRefMedia.h"
#include "Metadata/WwiseMetadataDialogue.h"
#include "Metadata/WwiseMetadataSoundBank.h"

#include <inttypes.h>


const WwiseDBString WwiseRefDialogueEvent::NAME = "DialogueEvent"_wwise_db;

const WwiseMetadataDialogueEvent* WwiseRefDialogueEvent::GetDialogueEvent() const
{
	const auto* SoundBank = GetSoundBank();
	if (!SoundBank) [[unlikely]]
	{
		return nullptr;
	}
	const auto& DialogueEvents = SoundBank->DialogueEvents;
	if (DialogueEvents.IsValidIndex(DialogueEventIndex))
	{
		return &DialogueEvents[DialogueEventIndex];
	}
	else
	{
		WWISE_DB_LOG(Error, "Could not get Dialogue Event index #%zu", DialogueEventIndex);
		return nullptr;
	}
}

WwiseDialogueArgumentArray WwiseRefDialogueEvent::GetDialogueArguments(const WwiseDialogueArgumentGlobalIdsMap& GlobalMap) const
{
	const auto* DialogueEvent = GetDialogueEvent();
	if (!DialogueEvent) [[unlikely]]
	{
		return {};
	}
	const auto& Arguments = DialogueEvent->Arguments;
	WwiseDialogueArgumentArray Result;
	Result.Empty(Arguments.Size());
	for (const auto& Elem : Arguments)
	{
		const auto* GlobalRef = GlobalMap.Find(WwiseDatabaseLocalizableIdKey(Elem.Id, LanguageId));
		if (GlobalRef)
		{
			Result.Add(*GlobalRef);
		}
		else
		{
			WWISE_DB_LOG(Error, "Could not get Dialogue Argument ID %" PRIu32, Elem.Id);
		}
	}

	return Result;
}

WwiseMediaIdsMap WwiseRefDialogueEvent::GetAllMedia(const WwiseMediaGlobalIdsMap& GlobalMap) const
{
	const auto* SoundBank = GetSoundBank();
	const auto* Event = GetDialogueEvent();
	if (!Event || !SoundBank) [[unlikely]]
	{
		return {};
	}
	WwiseMediaIdsMap Result;

	const auto& SoundBankMedia = SoundBank->Media;
	for (const auto& Elem : SoundBankMedia)
	{
		WwiseDatabaseMediaIdKey Id(Elem.Id, SoundBank->Id);
		const WwiseRefMedia* GlobalRef = GlobalMap.Find(Id);
		if (GlobalRef)
		{
			Result.Add(Elem.Id, *GlobalRef);
		}
	}
	return Result;
}

WwiseDBShortId WwiseRefDialogueEvent::DialogueEventId() const
{
	const auto* DialogueEvent = GetDialogueEvent();
	if (!DialogueEvent) [[unlikely]]
	{
		return 0;
	}
	return DialogueEvent->Id;
}

WwiseDBGuid WwiseRefDialogueEvent::DialogueEventGuid() const
{
	const auto* DialogueEvent = GetDialogueEvent();
	if (!DialogueEvent) [[unlikely]]
	{
		return {};
	}
	return DialogueEvent->GUID;
}

const WwiseDBString* WwiseRefDialogueEvent::DialogueEventName() const
{
	const auto* DialogueEvent = GetDialogueEvent();
	if (!DialogueEvent) [[unlikely]]
	{
		return &WwiseDBString::Empty;
	}
	return &DialogueEvent->Name;
}

const WwiseDBString* WwiseRefDialogueEvent::DialogueEventObjectPath() const
{
	const auto* DialogueEvent = GetDialogueEvent();
	if (!DialogueEvent) [[unlikely]]
	{
		return &WwiseDBString::Empty;
	}
	return &DialogueEvent->ObjectPath;
}

WwiseDBShortId WwiseRefDialogueEvent::Hash() const
{
	auto Result = WwiseRefSoundBank::Hash();
	Result = WwiseDBHashCombine(Result, GetTypeHash(DialogueEventIndex));
	return Result;
}
