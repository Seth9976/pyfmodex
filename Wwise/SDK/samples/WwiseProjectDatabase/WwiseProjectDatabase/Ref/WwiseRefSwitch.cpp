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

#include "Ref/WwiseRefSwitch.h"
#include "Metadata/WwiseMetadataSwitchGroup.h"

#include "Metadata/WwiseMetadataSwitch.h"

const WwiseDBString WwiseRefSwitch::NAME = "Switch"_wwise_db;

const WwiseMetadataSwitch* WwiseRefSwitch::GetSwitch() const
{
	const auto* SwitchGroup = GetSwitchGroup();
	if (!SwitchGroup) [[unlikely]]
	{
		return nullptr;
	}
	const auto& Switches = SwitchGroup->Switches;
	if (Switches.IsValidIndex(SwitchIndex))
	{
		return &Switches[SwitchIndex];
	}
	else
	{
		WWISE_DB_LOG(Error, "Could not get Switch index #%zu", SwitchIndex);
		return nullptr;
	}
}

WwiseDBShortId WwiseRefSwitch::SwitchId() const
{
	const auto* Switch = GetSwitch();
	if (!Switch) [[unlikely]]
	{
		return 0;
	}
	return Switch->Id;
}

const WwiseDBGuid* WwiseRefSwitch::SwitchGuid() const
{
	const auto* Switch = GetSwitch();
	if (!Switch) [[unlikely]]
	{
		return {};
	}
	return &Switch->GUID;
}

const WwiseDBString* WwiseRefSwitch::SwitchName() const
{
	const auto* Switch = GetSwitch();
	if (!Switch) [[unlikely]]
	{
		return {};
	}
	return &Switch->Name;
}

const WwiseDBString* WwiseRefSwitch::SwitchObjectPath() const
{
	const auto* Switch = GetSwitch();
	if (!Switch) [[unlikely]]
	{
		return {};
	}
	return &Switch->ObjectPath;
}

WwiseDBShortId WwiseRefSwitch::Hash() const
{
	auto Result = WwiseRefSwitchGroup::Hash();
	Result = WwiseDBHashCombine(Result, GetTypeHash(SwitchIndex));
	return Result;
}
