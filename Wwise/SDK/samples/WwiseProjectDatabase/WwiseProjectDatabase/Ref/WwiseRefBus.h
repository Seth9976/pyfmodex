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

#include "Ref/WwiseRefSoundBank.h"

class WwiseRefBus : public WwiseRefSoundBank
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::Bus;
	struct FGlobalIdsMap;

	WwiseRefIndexType BusIndex;

	WwiseRefBus() {}
	WwiseRefBus(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath,
		WwiseRefIndexType InSoundBankIndex, WwiseDBShortId InLanguageId,
		WwiseRefIndexType InBusIndex) :
		WwiseRefSoundBank(InRootMediaRef, InJsonFilePath, InSoundBankIndex, InLanguageId),
		BusIndex(InBusIndex)
	{}
	const WwiseMetadataBus* GetBus() const;

	WwiseDBShortId BusId() const;
	const WwiseDBGuid* BusGuid() const;
	const WwiseDBString* BusName() const;
	const WwiseDBString* BusObjectPath() const;

	WwiseDBShortId Hash() const override;
	WwiseRefType Type() const override { return TYPE; }
	bool operator==(const WwiseRefBus& Rhs) const
	{
		return WwiseRefSoundBank::operator ==(Rhs)
			&& BusIndex == Rhs.BusIndex;
	}
	bool operator!=(const WwiseRefBus& Rhs) const { return !operator==(Rhs); }
};

struct WwiseRefBus::FGlobalIdsMap
{
	WwiseBusGlobalIdsMap GlobalIdsMap;
};
