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

enum EStringCompareType
{
	CaseSensitive,
	IgnoreCase,
};


template <typename T>
class IWwiseString
{
public:
	T String;
	
	virtual ~IWwiseString() = default;

	virtual bool IsEmpty() const = 0;

	virtual unsigned int Size() const = 0;

	virtual unsigned int AllocatedSize() const = 0;
	
	bool operator==(const IWwiseString& Rhs) const
	{
		return this->String == Rhs.String;
	}

	bool operator<(const IWwiseString& Rhs) const
	{
		return this->String < Rhs.String;
	}
};
