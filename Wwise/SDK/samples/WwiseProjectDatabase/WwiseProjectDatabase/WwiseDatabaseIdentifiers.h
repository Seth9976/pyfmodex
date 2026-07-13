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

#include "AdapterTypes/WwiseWrapperTypes.h"

struct WwiseDatabaseMediaIdKey
{
	WwiseDBShortId MediaId = 0;
	WwiseDBShortId SoundBankId = 0;

	WwiseDatabaseMediaIdKey()
	{}
	WwiseDatabaseMediaIdKey(WwiseDBShortId InMediaId, WwiseDBShortId InSoundBankId) :
		MediaId(InMediaId),
		SoundBankId(InSoundBankId)
	{}
	bool operator==(const WwiseDatabaseMediaIdKey& Rhs) const
	{
		return MediaId == Rhs.MediaId
			&& SoundBankId == Rhs.SoundBankId;
	}
	bool operator<(const WwiseDatabaseMediaIdKey& Rhs) const
	{
		return (MediaId < Rhs.MediaId)
			|| (MediaId == Rhs.MediaId && SoundBankId < Rhs.SoundBankId);
	}
};

struct WwiseDatabaseLocalizableIdKey
{
	static constexpr WwiseDBShortId GENERIC_LANGUAGE = 0;

	WwiseDBShortId Id = 0;
	WwiseDBShortId LanguageId = 0;
	WwiseDBShortId SoundBankId = 0;

	WwiseDatabaseLocalizableIdKey()
	{}
	WwiseDatabaseLocalizableIdKey(WwiseDBShortId InId, WwiseDBShortId InLanguageId) :
		Id(InId),
		LanguageId(InLanguageId)
	{}
	WwiseDatabaseLocalizableIdKey(WwiseDBShortId InId, WwiseDBShortId InLanguageId, WwiseDBShortId InSoundBankId) :
		Id(InId),
		LanguageId(InLanguageId),
		SoundBankId(InSoundBankId)
	{}
	bool operator==(const WwiseDatabaseLocalizableIdKey& Rhs) const
	{
		return Id == Rhs.Id
			&& LanguageId == Rhs.LanguageId
			&& SoundBankId == Rhs.SoundBankId;
	}
	bool operator<(const WwiseDatabaseLocalizableIdKey& Rhs) const
	{
		return (Id < Rhs.Id)
			|| (Id == Rhs.Id && LanguageId < Rhs.LanguageId)
			|| (Id == Rhs.Id && LanguageId == Rhs.LanguageId && SoundBankId < Rhs.SoundBankId);
	}
};

struct WwiseDatabaseGroupValueKey
{

	WwiseDBShortId GroupId = 0;
	WwiseDBShortId Id = 0;

	WwiseDatabaseGroupValueKey()
	{}
	WwiseDatabaseGroupValueKey(WwiseDBShortId InGroupId, WwiseDBShortId InId) :
		GroupId(InGroupId),
		Id(InId)
	{}
	bool operator==(const WwiseDatabaseGroupValueKey& Rhs) const
	{
		return GroupId == Rhs.GroupId
			&& Id == Rhs.Id;
	}
	bool operator<(const WwiseDatabaseGroupValueKey& Rhs) const
	{
		return (GroupId < Rhs.GroupId)
			|| (GroupId == Rhs.GroupId && Id < Rhs.Id);
	}
};

struct WwiseDatabaseLocalizableGroupValueKey
{

	static constexpr WwiseDBShortId GENERIC_LANGUAGE = 0;

	WwiseDatabaseGroupValueKey GroupValue;
	WwiseDBShortId LanguageId = 0;

	WwiseDatabaseLocalizableGroupValueKey()
	{}
	WwiseDatabaseLocalizableGroupValueKey(WwiseDBShortId InGroup, WwiseDBShortId InId, WwiseDBShortId InLanguageId) :
		GroupValue(InGroup, InId),
		LanguageId(InLanguageId)
	{}
	WwiseDatabaseLocalizableGroupValueKey(WwiseDatabaseGroupValueKey InGroupValue, WwiseDBShortId InLanguageId) :
		GroupValue(InGroupValue),
		LanguageId(InLanguageId)
	{}
	bool operator==(const WwiseDatabaseLocalizableGroupValueKey& Rhs) const
	{
		return GroupValue == Rhs.GroupValue
			&& LanguageId == Rhs.LanguageId;
	}
	bool operator<(const WwiseDatabaseLocalizableGroupValueKey& Rhs) const
	{
		return (GroupValue < Rhs.GroupValue)
			|| (GroupValue == Rhs.GroupValue && LanguageId < Rhs.LanguageId);
	}
};

struct WwiseDatabaseLocalizableGuidKey
{

	static constexpr WwiseDBShortId GENERIC_LANGUAGE = WwiseDatabaseLocalizableIdKey::GENERIC_LANGUAGE;

	WwiseDBGuid Guid;
	WwiseDBShortId LanguageId = 0;		// 0 if no Language

	WwiseDatabaseLocalizableGuidKey()
	{}
	WwiseDatabaseLocalizableGuidKey(WwiseDBGuid InGuid, WwiseDBShortId InLanguageId) :
	Guid(InGuid),
	LanguageId(InLanguageId)
	{}
	bool operator==(const WwiseDatabaseLocalizableGuidKey& Rhs) const
	{
		return Guid == Rhs.Guid
			&& LanguageId == Rhs.LanguageId;
	}
	bool operator<(const WwiseDatabaseLocalizableGuidKey& Rhs) const
	{
		return (Guid < Rhs.Guid)
			|| (Guid == Rhs.Guid && LanguageId < Rhs.LanguageId);
	}
};

struct WwiseDatabaseLocalizableNameKey
{

	static constexpr WwiseDBShortId GENERIC_LANGUAGE = WwiseDatabaseLocalizableIdKey::GENERIC_LANGUAGE;

	WwiseDBString Name;
	WwiseDBShortId LanguageId = 0;		// 0 if no Language

	WwiseDatabaseLocalizableNameKey()
	{}
	WwiseDatabaseLocalizableNameKey(WwiseDBString InName, WwiseDBShortId InLanguageId) :
		Name(*InName),
		LanguageId(InLanguageId)
	{}

	bool operator==(const WwiseDatabaseLocalizableNameKey& Rhs) const
	{
		return Name == Rhs.Name
			&& LanguageId == Rhs.LanguageId;
	}
	bool operator<(const WwiseDatabaseLocalizableNameKey& Rhs) const
	{
		return (Name < Rhs.Name)
			|| (Name == Rhs.Name && LanguageId < Rhs.LanguageId);
	}
};

WwiseDBShortId GetTypeHash(const WwiseDatabaseMediaIdKey& FileId);
WwiseDBShortId GetTypeHash(const WwiseDatabaseLocalizableIdKey& LocalizableId);
WwiseDBShortId GetTypeHash(const WwiseDatabaseGroupValueKey& LocalizableGroupValue);
WwiseDBShortId GetTypeHash(const WwiseDatabaseLocalizableGroupValueKey& LocalizableGroupValue);
WwiseDBShortId GetTypeHash(const WwiseDatabaseLocalizableIdKey& EventId);
WwiseDBShortId GetTypeHash(const WwiseDatabaseLocalizableGuidKey& LocalizableGuid);
WwiseDBShortId GetTypeHash(const WwiseDatabaseLocalizableNameKey& LocalizableName);

template <>
struct std::hash<WwiseDatabaseMediaIdKey>
{
	std::size_t operator()(const WwiseDatabaseMediaIdKey& Key) const noexcept
	{
		return GetTypeHash(Key);
	}
};

template <>
struct std::hash<WwiseDatabaseGroupValueKey>
{
	std::size_t operator()(const WwiseDatabaseGroupValueKey& Key) const noexcept
	{
		return GetTypeHash(Key);
	}
};

template <>
struct std::hash<WwiseDatabaseLocalizableGroupValueKey>
{
	std::size_t operator()(const WwiseDatabaseLocalizableGroupValueKey& Key) const noexcept
	{
		return GetTypeHash(Key);
	}
};

template <>
struct std::hash<WwiseDatabaseLocalizableIdKey>
{
	std::size_t operator()(const WwiseDatabaseLocalizableIdKey& Key) const noexcept
	{
		return GetTypeHash(Key);
	}
};

template <>
struct std::hash<WwiseDatabaseLocalizableGuidKey>
{
	std::size_t operator()(const WwiseDatabaseLocalizableGuidKey& Key) const noexcept
	{
		return GetTypeHash(Key);
	}
};

template <>
struct std::hash<WwiseDatabaseLocalizableNameKey>
{
	std::size_t operator()(const WwiseDatabaseLocalizableNameKey& Key) const noexcept
	{
		return GetTypeHash(Key);
	}
};

inline WwiseDBShortId GetTypeHash(const WwiseDBShortId& ShortId)
{
	return ShortId;
}

inline WwiseDBShortId WwiseDBHashCombine(WwiseDBShortId A, WwiseDBShortId C)
{
	WwiseDBShortId B = 0x9e3779b9;
	A += B;

	A -= B; A -= C; A ^= (C>>13);
	B -= C; B -= A; B ^= (A<<8);
	C -= A; C -= B; C ^= (B>>13);
	A -= B; A -= C; A ^= (C>>12);
	B -= C; B -= A; B ^= (A<<16);
	C -= A; C -= B; C ^= (B>>5);
	A -= B; A -= C; A ^= (C>>3);
	B -= C; B -= A; B ^= (A<<10);
	C -= A; C -= B; C ^= (B>>15);

	return C;
}

typedef WwiseDatabaseLocalizableGuidKey LocalizableGuidKey;
typedef WwiseDatabaseLocalizableIdKey LocalizableIdKey;
typedef WwiseDatabaseLocalizableNameKey LocalizableNameKey;
typedef WwiseDatabaseMediaIdKey MediaIdKey;
typedef WwiseDatabaseLocalizableGroupValueKey LocalizableGroupValueKey;
typedef WwiseDatabaseGroupValueKey GroupValueKey;
