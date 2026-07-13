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

template <typename SetType>
class IWwiseSet
{
public:
	IWwiseSet()
	{
		
	}
	
	IWwiseSet(const SetType& InSet)
	{
		Set = InSet;
	}
	virtual ~IWwiseSet() = default;
	
	SetType Set;

	virtual unsigned int Size() const
	{
		return 0;
	}

	virtual void Reserve(int NumberOfElements)
	{
		
	}

	virtual void Empty(int ExpectedUsage = 0)
	{
		
	}

	virtual bool IsValidIndex(int Index) const
	{
		return Index >= 0 && Index < (int)Size();
	}
	
	friend bool operator==(const IWwiseSet& X, const IWwiseSet& Y)
	{
		return X.Set == Y.Set;
	}

	auto begin() const
	{
		return Set.begin();
	}
	
	auto end() const
	{
		return Set.end();
	}

	auto begin()
	{
		return Set.begin();
	}
	
	auto end()
	{
		return Set.end();
	}
};