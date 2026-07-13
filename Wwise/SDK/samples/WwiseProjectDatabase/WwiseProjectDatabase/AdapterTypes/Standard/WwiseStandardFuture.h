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

#include "AdapterTypes/WwiseFuture.h"

#include <future>

template<typename ReturnType, typename StandardFuture = std::future<ReturnType>>
class WwiseStandardFuture : public IWwiseFuture<StandardFuture>
{
public:
	WwiseStandardFuture() : IWwiseSharedFuture<StandardFuture>(){}
	
	WwiseStandardFuture(StandardFuture&& Future) : IWwiseFuture<StandardFuture>(std::move(Future))
	{}

	ReturnType Get()
	{
		return this->Future.get();
	}
};

template<typename ReturnType, typename StandardSharedFuture = std::shared_future<ReturnType>>
class WwiseStandardSharedFuture : public IWwiseSharedFuture<StandardSharedFuture>
{
public:
	WwiseStandardSharedFuture() : IWwiseSharedFuture<StandardSharedFuture>(){}
	
	WwiseStandardSharedFuture(StandardSharedFuture&& Future) : IWwiseSharedFuture<StandardSharedFuture>(std::move(Future))
	{}

	ReturnType Get() const
	{
		return this->Future.get();
	}
};
