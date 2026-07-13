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
#include "AdapterTypes/WwiseSet.h"

#include <unordered_set>
#include <vector>

template <typename ObjectType, typename StandardSet = std::unordered_set<ObjectType>>
class WwiseStandardSet : public IWwiseSet<StandardSet>
{
public:
	WwiseStandardSet(){}

	WwiseStandardSet(const StandardSet& Set)
		: IWwiseSet<StandardSet>(Set)
	{
	}
	
	WwiseStandardSet(const WwiseStandardArray<ObjectType>& Array)
	{
		this->Set = std::unordered_set(Array.Array.begin(), Array.Array.end());
	}

	unsigned int Size() const override
	{
		return (unsigned int)this->Set.size();
	}
	
	void Reserve(int NumberOfElements) override
	{
		this->Set.reserve(NumberOfElements);
	}

	WwiseStandardSet Difference(const WwiseStandardSet& OtherSet)
	{
		std::unordered_set<ObjectType> diff = this->Set;
		for (const auto& Element : OtherSet.Set)
		{
			diff.erase(Element);
		}
		return diff;
	}

	void Empty(int ExpectedUsage = 0) override
	{
		this->Set.clear();
	}

	void Append(const WwiseStandardArray<ObjectType>& T)
	{
		this->Set.insert(T.Array.begin(), T.Array.end());
	}

	void Append(const WwiseStandardSet<ObjectType>& T)
	{
		this->Set.insert(T.Set.begin(), T.Set.end());
	}

	void Add(const ObjectType& T, bool* bIsAlreadyInSetPtr = nullptr)
	{
		auto result = this->Set.insert(T);
		if(bIsAlreadyInSetPtr && !result.second)
		{
			*bIsAlreadyInSetPtr = true;
		}
	}

	template<typename KeyType>
	const ObjectType* Find(KeyType T) const
	{
		auto Itr = this->Set.find(T);
		if(Itr == this->Set.end())
		{
			return nullptr;
		}
		return &*Itr;	
	}

	void Emplace(const ObjectType& T, bool* bIsAlreadyInSetPtr = nullptr)
	{
		this->Set.emplace(T);
	}

	WwiseStandardArray<ObjectType> AsArray() const
	{
		std::vector<ObjectType> OutArray;
		for(auto& Element : this->Set)
		{
			OutArray.push_back(Element);
		}
		return OutArray;
	}

	bool Contains(const ObjectType& Type) const
	{
		return this->Set.find(Type) != this->Set.end();
	}

	ObjectType GetFirst() const
	{
		return *this->Set.begin();
	}

	template<typename Array>
	void AppendTo(Array& ArrayToAppend)
	{
		std::vector<ObjectType> Vector;
		for(auto& Object : this->Set)
		{
			Vector.push_back(Object);
		}
		auto Data = Vector.data();
		ArrayToAppend.Append(Data, this->Set.size());
	}
};

