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

#include "Ref/WwiseRefDialogueArgument.h"
#include "Metadata/WwiseMetadataDialogue.h"

const WwiseDBString WwiseRefDialogueArgument::NAME = "DialogueArgument"_wwise_db;

const WwiseMetadataDialogueArgument* WwiseRefDialogueArgument::GetDialogueArgument() const
{
	const auto* DialogueEvent = GetDialogueEvent();
	if (!DialogueEvent) [[unlikely]]
	{
		return nullptr;
	}
	const auto& Arguments = DialogueEvent->Arguments;
	if (Arguments.IsValidIndex(DialogueArgumentIndex))
	{
		return &Arguments[DialogueArgumentIndex];
	}
	else
	{
		WWISE_DB_LOG(Error,"Could not get Dialogue Argument index #%zu", DialogueArgumentIndex);
		return nullptr;
	}
}

WwiseDBShortId WwiseRefDialogueArgument::DialogueArgumentId() const
{
	const auto* DialogueArgument = GetDialogueArgument();
	if (!DialogueArgument) [[unlikely]]
	{
		return 0;
	}
	return DialogueArgument->Id;
}

WwiseDBGuid WwiseRefDialogueArgument::DialogueArgumentGuid() const
{
	const auto* DialogueArgument = GetDialogueArgument();
	if (!DialogueArgument) [[unlikely]]
	{
		return {};
	}
	return DialogueArgument->GUID;
}

WwiseDBString WwiseRefDialogueArgument::DialogueArgumentName() const
{
	const auto* DialogueArgument = GetDialogueArgument();
	if (!DialogueArgument) [[unlikely]]
	{
		return {};
	}
	return DialogueArgument->Name;
}

WwiseDBString WwiseRefDialogueArgument::DialogueArgumentObjectPath() const
{
	const auto* DialogueArgument = GetDialogueArgument();
	if (!DialogueArgument) [[unlikely]]
	{
		return {};
	}
	return DialogueArgument->ObjectPath;
}

WwiseDBShortId WwiseRefDialogueArgument::Hash() const
{
	auto Result = WwiseRefDialogueEvent::Hash();
	Result = WwiseDBHashCombine(Result, GetTypeHash(DialogueArgumentIndex));
	return Result;
}
