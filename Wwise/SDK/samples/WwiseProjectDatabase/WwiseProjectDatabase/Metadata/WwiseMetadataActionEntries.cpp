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

#include "Metadata/WwiseMetadataActionEntries.h"

#include "Metadata/WwiseMetadataLoader.h"

WwiseMetadataActionPostEventEntry::WwiseMetadataActionPostEventEntry(WwiseMetadataLoader& Loader) :
	WwiseMetadataBasicReference(Loader)
{
	Loader.LogParsed("ActionPostEventEntry"_wwise_db, Id, Name);
}

WwiseMetadataActionSetFXEntry::WwiseMetadataActionSetFXEntry(WwiseMetadataLoader& Loader) :
	WwiseMetadataBasicReference(Loader)
{
	Loader.LogParsed("ActionSetFXEntry"_wwise_db, Id, Name);
}

WwiseMetadataActionSetStateEntry::WwiseMetadataActionSetStateEntry(WwiseMetadataLoader& Loader) :
	WwiseMetadataGroupValueReference(Loader)
{
	Loader.LogParsed("ActionSetStateEntry"_wwise_db, Id, Name);
}

WwiseMetadataActionSetSwitchEntry::WwiseMetadataActionSetSwitchEntry(WwiseMetadataLoader& Loader) :
	WwiseMetadataGroupValueReference(Loader)
{
	Loader.LogParsed("ActionSetSwitchEntry"_wwise_db, Id, Name);
}

WwiseMetadataActionTriggerEntry::WwiseMetadataActionTriggerEntry(WwiseMetadataLoader& Loader) :
	WwiseMetadataBasicReference(Loader)
{
	Loader.LogParsed("ActionTriggerEntry"_wwise_db, Id, Name);
}
