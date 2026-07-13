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

#include "Metadata/WwiseMetadataSwitchGroup.h"
#include "Metadata/WwiseMetadataLoader.h"

WwiseMetadataSwitchGroup::WwiseMetadataSwitchGroup(WwiseMetadataLoader& Loader) :
	WwiseMetadataBasicReference(Loader),
	GameParameterRef(Loader.GetObjectPtr<WwiseMetadataGameParameterReference>(this, "GameParameterRef"_wwise_db)),
	Switches(Loader.GetArray<WwiseMetadataSwitch>(this, "Switches"_wwise_db))
{
	Loader.LogParsed("SwitchGroup"_wwise_db, Id, Name);
}
