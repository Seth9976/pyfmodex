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
#include "AdapterTypes/WwisePair.h"

template <typename FirstType, typename SecondType, typename StandardPair = std::pair<FirstType, SecondType>>
class WwiseStandardPair : public IWwisePair<StandardPair>
{
public:
	WwiseStandardPair(): IWwisePair<StandardPair>()
	{
		this->Pair = std::pair<FirstType, SecondType>();
	}

	WwiseStandardPair(const StandardPair& InPair) : IWwisePair<StandardPair>(InPair)
	{
	}

	WwiseStandardPair(StandardPair& InPair) : IWwisePair<StandardPair>(InPair)
	{
	}

	WwiseStandardPair(StandardPair&& InPair) : IWwisePair<StandardPair>(std::move(InPair))
	{
	}
	
	WwiseStandardPair(const FirstType& Key, const SecondType& Value): IWwisePair<StandardPair>()
	{
		this->Pair.first = Key;
		this->Pair.second = Value;
	}

	const FirstType& GetFirst() const
	{
		return this->Pair.first;
	}

	FirstType& GetFirst()
	{
		return this->Pair.first;
	}

	SecondType GetSecond() const
	{
		return this->Pair.second;
	}

	SecondType& GetSecond()
	{
		return this->Pair.second;
	}

	bool operator<(const WwiseStandardPair<FirstType, SecondType>& Rhs) const
	{
		if(this->Pair.first == Rhs.GetFirst())
		{
			return this->Pair.second < Rhs.GetSecond();
		}
		return this->Pair.first < Rhs.GetFirst();
	}
	
	bool operator==(const WwiseStandardPair<FirstType, SecondType>& Rhs) const
	{
		return this->Pair == Rhs.Pair;
	}
};

template <typename FirstType, typename SecondType>
struct std::hash<WwiseStandardPair<FirstType, SecondType>>
{
	std::size_t operator()(const WwiseStandardPair<FirstType, SecondType>& Pair) const noexcept
	{
		return std::hash<FirstType>()(Pair.GetFirst()) ^ std::hash<SecondType>()(Pair.GetSecond());
	}
};
