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

#include "Metadata/WwiseMetadataDialogue.h"
#include "Metadata/WwiseMetadataLoader.h"

WwiseMetadataDialogueEventReference::WwiseMetadataDialogueEventReference(WwiseMetadataLoader& Loader) :
	WwiseMetadataBasicReference(Loader)
{
	Loader.LogParsed("DialogueEventReference"_wwise_db, Id, Name);
}

WwiseMetadataDialogueArgument::WwiseMetadataDialogueArgument(WwiseMetadataLoader& Loader) :
	WwiseMetadataBasicReference(Loader)
{
	Loader.LogParsed("DialogueArgument"_wwise_db, Id, Name);
}

WwiseMetadataDialogueEvent::WwiseMetadataDialogueEvent(WwiseMetadataLoader& Loader) :
	WwiseMetadataDialogueEventReference(Loader),
	Arguments(Loader.GetArray<WwiseMetadataDialogueArgument>(this, "Arguments"_wwise_db))
{
	Loader.LogParsed("DialogueEvent"_wwise_db, Id, Name);
}
