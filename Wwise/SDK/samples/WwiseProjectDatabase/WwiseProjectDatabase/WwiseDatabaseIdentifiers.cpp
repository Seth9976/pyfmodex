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

#include "WwiseDatabaseIdentifiers.h"

WwiseDBShortId GetTypeHash(const WwiseDatabaseMediaIdKey& MediaId)
{
	return WwiseDBHashCombine(
		GetTypeHash(MediaId.MediaId),
		GetTypeHash(MediaId.SoundBankId));
}

WwiseDBShortId GetTypeHash(const WwiseDatabaseLocalizableIdKey& LocalizableId)
{
	return WwiseDBHashCombine(WwiseDBHashCombine(
		GetTypeHash(LocalizableId.Id),
		GetTypeHash(LocalizableId.SoundBankId)),
		GetTypeHash(LocalizableId.LanguageId));
}

WwiseDBShortId GetTypeHash(const WwiseDatabaseGroupValueKey& GroupId)
{
	return WwiseDBHashCombine(
		GetTypeHash(GroupId.GroupId),
		GetTypeHash(GroupId.Id));
}

WwiseDBShortId GetTypeHash(const WwiseDatabaseLocalizableGroupValueKey& LocalizableGroupValue)
{
	return WwiseDBHashCombine(
		GetTypeHash(LocalizableGroupValue.GroupValue),
		GetTypeHash(LocalizableGroupValue.LanguageId));
}

WwiseDBShortId GetTypeHash(const WwiseDatabaseLocalizableGuidKey& LocalizableGuid)
{
	return WwiseDBHashCombine(
		GetTypeHash(LocalizableGuid.Guid),
		GetTypeHash(LocalizableGuid.LanguageId));
}

WwiseDBShortId GetTypeHash(const WwiseDatabaseLocalizableNameKey& LocalizableName)
{
	return WwiseDBHashCombine(
		GetTypeHash(LocalizableName.Name),
		GetTypeHash(LocalizableName.LanguageId));
}
