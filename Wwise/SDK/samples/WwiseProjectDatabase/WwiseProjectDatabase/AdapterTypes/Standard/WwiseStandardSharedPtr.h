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

#include "AdapterTypes/WwiseSharedPtr.h"

template<typename Type, typename StandardSharedPtr = std::shared_ptr<Type>>
class WwiseStandardSharedPtr : public IWwiseSharedPtr<StandardSharedPtr>
{
public:
	WwiseStandardSharedPtr() {}

	~WwiseStandardSharedPtr()
	{
		if(this->SharedPtr)
		{
			this->SharedPtr.reset();
		}
	}
	
	WwiseStandardSharedPtr(StandardSharedPtr& InSharedPtr) : IWwiseSharedPtr<StandardSharedPtr>(std::move(InSharedPtr)){}

	WwiseStandardSharedPtr(const WwiseStandardSharedPtr<Type>& InSharedPtr) : IWwiseSharedPtr<StandardSharedPtr>(std::move(InSharedPtr.SharedPtr)){}

	WwiseStandardSharedPtr(WwiseStandardSharedPtr<Type>&& InSharedPtr) noexcept	: IWwiseSharedPtr<StandardSharedPtr>(std::move(InSharedPtr.SharedPtr)){}

	WwiseStandardSharedPtr& operator=(const WwiseStandardSharedPtr& other) = default;
	WwiseStandardSharedPtr& operator=(WwiseStandardSharedPtr&& other) noexcept = default;

	WwiseStandardSharedPtr(Type* InPtr)
	{
		this->SharedPtr = StandardSharedPtr(InPtr);
	}


	template<typename... ArgsType>
	void Make_Shared(ArgsType&&... Args)
	{
		this->SharedPtr = std::make_shared<Type>(Args...);
	}

	Type* Get() const
	{
		return this->SharedPtr.get();
	}

	bool IsValid() const
	{
		return this->SharedPtr != nullptr;
	}

	bool operator!() const
	{
		return !this->SharedPtr;
	}
	
	Type operator*() const
	{
		return *this->SharedPtr;
	}

	Type* operator->() const
	{
		return this->SharedPtr.get();
	}

	bool operator==(std::nullptr_t Null) const
	{
		return this->SharedPtr == Null;
	}

	bool operator!=(std::nullptr_t Null) const
	{
		return this->SharedPtr != Null;
	}

	operator bool() const
	{
		return this->SharedPtr != nullptr;
	}
};