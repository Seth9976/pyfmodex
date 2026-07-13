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

template<typename FutureType>
class IWwiseFuture
{
public:
	FutureType Future;

	IWwiseFuture(){}
	
	IWwiseFuture(FutureType&& Future)
	: Future(std::move(Future))
	{}
};

template<typename FutureType>
class IWwiseSharedFuture
{
public:
	FutureType Future;

	IWwiseSharedFuture(){}
	
	IWwiseSharedFuture(FutureType&& Future)
	: Future(std::move(Future))
	{}
};
