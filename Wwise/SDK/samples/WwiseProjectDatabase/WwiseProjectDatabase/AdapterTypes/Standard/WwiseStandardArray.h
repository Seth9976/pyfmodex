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

#include "AdapterTypes/WwiseArray.h"

#include <algorithm>
#include <vector>

template <typename ObjectType, typename StandardArray = std::vector<ObjectType>>
class WwiseStandardArray : public IWwiseArray<StandardArray>
{
public:
	WwiseStandardArray(){}
	
	WwiseStandardArray(const StandardArray& Array)
		: IWwiseArray<StandardArray>(Array)
	{
	}

	unsigned int Size() const override
	{
		return (unsigned int)this->Array.size();
	}
	
	void Reserve(unsigned int NumberOfElements) override
	{
		this->Array.reserve(NumberOfElements);		
	}

	void Empty(unsigned int ExpectedUsage = 0) override
	{
		this->Array.clear();
		Reserve(ExpectedUsage);
	}

	void Pop() override
	{
		this->Array.pop_back();
	}

	unsigned int GetTypeSize() const override
	{
		return sizeof(ObjectType);
	}

	template <typename Predicate>
	bool ContainsByPredicate(Predicate InPredicate) const
	{
		return std::all_of(this->Array.begin(), this->Array.end(), InPredicate);
	}

	template <typename Predicate>
	ObjectType* FindByPredicate(Predicate InPredicate)
	{
		auto it = std::find_if(this->Array.begin(), this->Array.end(), InPredicate);
		if (it != this->Array.end())
		{
			return &(*it);
		}
		return nullptr;
	}
	
	void Append(const ObjectType* T)
	{
		for(int i = 0; i < sizeof(T); i++)
		{
			this->Array.push_back(T[i]);	
		}
	}

	void Append(const WwiseStandardArray T)
	{
		for(auto Elem : T)
		{
			this->Array.push_back(Elem);	
		}
	}

	template<typename... ArgsType>
	int Emplace(ArgsType&&... T)
	{
		this->Array.emplace_back(T...);
		return this->Array.size();
	}

	void Add(const ObjectType& T)
	{
		this->Array.push_back(T);	
	}

	void Add(ObjectType&& T)
	{
		this->Array.push_back(std::move(T));	
	}

	void Add(const WwiseStandardArray& T)
	{
		for(auto Elem: T.Array)
		{
			this->Array.push_back(T);
		}
	}

	const ObjectType* GetData() const
	{
		if(this->Array.empty())
		{
			return nullptr;
		}
		return this->Array.data();
	}

	const auto& operator[](unsigned int Index) const
	{
		return this->Array[Index];
	}

	auto& operator[](unsigned int Index)
	{
		return this->Array[Index];
	}
	
	const auto& operator()(const ObjectType& Object) const
	{
		return Object;
	}
};

