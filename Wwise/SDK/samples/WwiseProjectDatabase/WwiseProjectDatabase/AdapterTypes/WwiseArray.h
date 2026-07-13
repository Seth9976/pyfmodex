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

template <typename ArrayType>
class IWwiseArray
{
public:
	IWwiseArray()
	{
		
	}
	
	IWwiseArray(const ArrayType& InArray)
	{
		Array = InArray;
	}

	virtual ~IWwiseArray() {}
	
	ArrayType Array;

	virtual unsigned int Size() const = 0;

	virtual void Reserve(unsigned int NumberOfElements) = 0;

	virtual void Empty(unsigned int ExpectedUsage = 0) = 0;

	virtual unsigned int GetTypeSize() const = 0;

	virtual bool IsValidIndex(unsigned int Index) const
	{
		return Index < Size();
	}

	virtual void Pop() = 0;
	
	friend bool operator==(const IWwiseArray& X, const IWwiseArray& Y)
	{
		return X.Array == Y.Array;
	}

	auto begin() const
	{
		return Array.begin();
	}
	
	auto end() const
	{
		return Array.end();
	}

	auto begin()
	{
		return Array.begin();
	}
	
	auto end()
	{
		return Array.end();
	}
};