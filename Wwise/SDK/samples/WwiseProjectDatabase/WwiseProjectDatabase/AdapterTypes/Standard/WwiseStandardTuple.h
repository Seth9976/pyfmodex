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
#include "AdapterTypes/WwiseTuple.h"
#include <tuple>

template <typename... Types>
class WwiseStandardTuple : public IWwiseTuple<std::tuple<Types...>>
{
public:
	WwiseStandardTuple()
	{
		this->Tuple = std::tuple<Types...>();
	}

	WwiseStandardTuple(const Types&... InTypes)
	{
		this->Tuple = std::tuple<Types...>(InTypes...);
	}

	WwiseStandardTuple(const std::tuple<Types...>& InTuple)
	{
		this->Tuple = InTuple;
	}

	template <int N>
	auto Get() const
	{
		return std::get<N>(this->Tuple);
	}

	friend bool operator==(const WwiseStandardTuple<Types...>& Lhs, const WwiseStandardTuple<Types...>& Rhs)
	{
		return Lhs.Tuple == Rhs.Tuple;
	}
};
