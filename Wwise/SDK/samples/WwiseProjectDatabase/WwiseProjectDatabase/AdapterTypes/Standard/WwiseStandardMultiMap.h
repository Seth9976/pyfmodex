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

#include "AdapterTypes/Standard/WwiseStandardArray.h"
#include "AdapterTypes/WwiseMultiMap.h"

#include <unordered_map>

template <typename KeyType, typename ObjectType, typename StandardMultiMap = std::unordered_multimap<KeyType, ObjectType>>
class WwiseStandardMultiMap : public IWwiseMultiMap<StandardMultiMap>
{
public:
	WwiseStandardMultiMap(){}
	
	WwiseStandardMultiMap(const StandardMultiMap& MultiMap)
		: IWwiseMultiMap<StandardMultiMap>(MultiMap)
	{
	}

	unsigned int Size() const override
	{
		return (unsigned int)this->MultiMap.size();
	}
	
	void Reserve(int NumberOfElements) override
	{
		this->MultiMap.reserve(NumberOfElements);
	}

	void Empty(int ExpectedUsage = 0) override
	{
		this->MultiMap.clear();
	}

	void Append(const WwiseStandardMultiMap<KeyType, ObjectType>& T)
	{
		for(auto Elem : T.MultiMap)
		{
			this->MultiMap.emplace(Elem.first, Elem.second);	
		}
	}

	void Add(const KeyType& Key, const ObjectType& Object)
	{
		this->MultiMap.insert(std::pair(Key, Object));	
	}

	void MultiFind(const KeyType& Key, WwiseStandardArray<ObjectType>& OutValues, bool bMaintainOrder = true) const
	{
		auto range = this->MultiMap.equal_range(Key);
		if(range.first == this->MultiMap.end())
		{
			return;
		}
		for (auto itr = range.first; itr != range.second; ++itr)
		{
			OutValues.Add(itr->second);
		}
	}

	void MultiFindPointer(KeyType Key, WwiseStandardArray<const ObjectType*>& OutValues, bool bMaintainOrder = true) const
	{
		auto range = this->MultiMap.equal_range(Key);
		if(range.first == this->MultiMap.end())
		{
			return;
		}
		
		for (auto itr = range.first; itr != range.second; ++itr)
		{
			OutValues.Add(&itr->second);
		}
	}

	const ObjectType* At(int Index) const
	{
		int Count = 0;
		for(auto& Element : this->MultiMap)
		{
			if(Count == Index)
			{
				return &Element.second;
			}
			Count++;
		}
		return nullptr;
	}

	bool Contains(const KeyType& Key) const
	{
		return this->MultiMap.contains(Key);
	}
};