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

#include <future>

#include "AdapterTypes/Standard/WwiseStandardFuture.h"

namespace WwiseStandardAsync
{
	template<typename CallableType>
	auto WwiseAsync(CallableType&& Type) -> WwiseStandardFuture<decltype(std::forward<CallableType>(Type)())>
	{
		return WwiseStandardFuture<decltype(std::forward<CallableType>(Type)())>(std::async(std::launch::async, std::move(Type)));
	}

	template<typename CallableType>
	auto WwiseAsyncShared(CallableType&& Type) -> WwiseStandardSharedFuture<decltype(std::forward<CallableType>(Type)())>
	{
		return WwiseStandardSharedFuture<decltype(std::forward<CallableType>(Type)())>(std::shared_future<decltype(std::forward<CallableType>(Type)())>(std::async(std::launch::async, std::move(Type))));
	}
}
