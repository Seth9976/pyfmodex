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

template <typename MapType>
class IWwiseMap
{
public:
	IWwiseMap()
	{
		
	}

	IWwiseMap(const IWwiseMap& Other)
		: Map(Other.Map)
	{
	}
	
	IWwiseMap(const MapType& InMap)
	{
		Map = InMap;
	}
	
	virtual ~IWwiseMap();
	
	MapType Map;

	virtual unsigned int Size() const
	{
		return 0;
	}

	virtual void Reserve(int NumberOfElements)
	{
	}

	IWwiseMap(IWwiseMap&& Other) noexcept
		: Map(std::move(Other.Map))
	{
	}

	IWwiseMap& operator=(const IWwiseMap& Other)
	{
		if (this == &Other)
		{
			return *this;
		}
		Map.clear();
		Map = MapType(Other.Map);

		return *this;
	}

	IWwiseMap& operator=(IWwiseMap&& Other) noexcept
	{
		if (this == &Other)
		{
			return *this;
		}
		Map = std::move(Other.Map);
		return *this;
	}

	virtual void Empty(unsigned int ExpectedUsage = 0)
	{
		
	}

	virtual bool IsValidIndex(int Index) const
	{
		return Index >= 0 && Index < (int)Size();
	}
	
	friend bool operator==(const IWwiseMap& X, const IWwiseMap& Y)
	{
		return X.Map == Y.Map;
	}

	auto begin() const
	{
		return Map.begin();
	}
	
	auto end() const
	{
		return Map.end();
	}

	auto begin()
	{
		return Map.begin();
	}
	
	auto end()
	{
		return Map.end();
	}
};

template <typename MapType>
IWwiseMap<MapType>::~IWwiseMap()
{
}
