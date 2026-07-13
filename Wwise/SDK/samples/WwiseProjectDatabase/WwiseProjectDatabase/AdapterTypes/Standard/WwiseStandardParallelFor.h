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

#include <execution>

#ifdef __cpp_lib_execution
#define WWISE_STANDARD_PARALLEL_FOR_IS_PARALLEL 
#endif

#include "AdapterTypes/Standard/WwiseStandardMap.h"
#include "AdapterTypes/Standard/WwiseStandardArray.h"

namespace WwiseStandardAsync
{
	// This is only parallel when when std Execution policies are implemented
	template<typename ArrayType, typename CallableType>
	void WwiseParallelFor(const WwiseStandardArray<ArrayType>& Array, CallableType&& Type)
	{
#ifdef WWISE_STANDARD_PARALLEL_FOR_IS_PARALLEL
		std::for_each(std::execution::par, Array.begin(), Array.end(), std::move(Type));
#else
		std::for_each(Array.begin(), Array.end(), std::move(Type));
#endif
	}

	// This is only parallel when std Execution policies are implemented
	template<typename KeyType, typename ValueType, typename CallableType>
	void WwiseParallelFor(const WwiseStandardMap<KeyType, ValueType>& Map, CallableType&& Type)
	{
#ifdef WWISE_STANDARD_PARALLEL_FOR_IS_PARALLEL
		std::for_each(std::execution::par, Map.begin(), Map.end(), std::move(Type));
#else
		std::for_each(Map.begin(), Map.end(), std::move(Type));
#endif
	}
}