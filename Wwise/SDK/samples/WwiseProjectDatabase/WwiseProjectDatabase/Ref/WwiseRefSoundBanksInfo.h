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

#include "Ref/WwiseRefRootFile.h"

class WwiseRefSoundBanksInfo : public WwiseRefRootFile
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::SoundBanksInfo;

	WwiseRefType Type() const override { return TYPE; }

	WwiseRefSoundBanksInfo() {}
	WwiseRefSoundBanksInfo(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath) :
		WwiseRefRootFile(InRootMediaRef, InJsonFilePath)
	{}
	const WwiseMetadataSoundBanksInfo* GetSoundBanksInfo() const;
};
