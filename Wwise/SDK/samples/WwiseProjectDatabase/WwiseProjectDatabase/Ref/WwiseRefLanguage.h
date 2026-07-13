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

#include "Ref/WwiseRefProjectInfo.h"

class WwiseRefLanguage : public WwiseRefProjectInfo
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::Language;

	WwiseRefIndexType LanguageIndex;

	WwiseRefLanguage() :
		LanguageIndex(-1)
	{}
	WwiseRefLanguage(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath,
		WwiseRefIndexType InLanguageIndex) :
		WwiseRefProjectInfo(InRootMediaRef, InJsonFilePath),
		LanguageIndex(InLanguageIndex)
	{}
	const WwiseMetadataLanguage* GetLanguage() const;

	WwiseDBShortId LanguageId() const;
	WwiseDBGuid LanguageGuid() const;
	WwiseDBString LanguageName() const;

	WwiseDBShortId Hash() const override;
	WwiseRefType Type() const override { return TYPE; }
	bool operator==(const WwiseRefLanguage& Rhs) const
	{
		return WwiseRefProjectInfo::operator==(Rhs)
			&& LanguageIndex == Rhs.LanguageIndex;
	}
	bool operator!=(const WwiseRefLanguage& Rhs) const { return !operator==(Rhs); }
};
