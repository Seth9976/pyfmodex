/*******************************************************************************
The content of this file includes portions of the proprietary AUDIOKINETIC Wwise
Technology released in source code form as part of the game integration package.
The content of this file may not be used without valid licenses to the
AUDIOKINETIC Wwise Technology.
Note that the use of the game engine is subject to the Standard(R) Engine End User
License Agreement at https://www.Standardengine.com/en-US/eula/Standard
 
License Usage
 
Licensees holding valid licenses to the AUDIOKINETIC Wwise Technology may use
this file in accordance with the end user license agreement provided with the
software or, alternatively, in accordance with the terms contained
in a written agreement between you and Audiokinetic Inc.
Copyright (c) 2026 Audiokinetic Inc.
*******************************************************************************/
#pragma once

#include "AdapterTypes/Standard/WwiseStandardArray.h"
#include "AdapterTypes/Standard/WwiseStandardGuid.h"
#include "AdapterTypes/Standard/WwiseStandardJsonObject.h"
#include "AdapterTypes/Standard/WwiseStandardMap.h"
#include "AdapterTypes/Standard/WwiseStandardMultiMap.h"
#include "AdapterTypes/Standard/WwiseStandardPair.h"
#include "AdapterTypes/Standard/WwiseStandardSet.h"
#include "AdapterTypes/Standard/WwiseStandardSharedPtr.h"
#include "AdapterTypes/Standard/WwiseStandardTask.h"
#include "AdapterTypes/Standard/WwiseStandardAsync.h"
#include "AdapterTypes/Standard/WwiseStandardParallelFor.h"
#include "AdapterTypes/Standard/WwiseStandardFuture.h"

template<typename Type>
using WwiseDBArray = WwiseStandardArray<Type>;
	
using WwiseDBGuid = WwiseStandardGuid;
	
using WwiseDBJsonObject = WwiseStandardJsonObject;
	
template<typename Key, typename Value>
using WwiseDBMap = WwiseStandardMap<Key, Value>;
	
template<typename Key, typename Value>
using WwiseDBMultiMap = WwiseStandardMultiMap<Key, Value>;
	
template <typename  FirstType, typename SecondType>
using WwiseDBPair = WwiseStandardPair<FirstType, SecondType>;
	
template <typename ObjectType>
using WwiseDBSet = WwiseStandardSet<ObjectType>;
	
using WwiseDBString = WwiseStandardString;

template <typename... Args>
using WwiseDBTuple = WwiseStandardTuple<Args...>;

template<typename Type>
using WwiseDBSharedPtr = WwiseStandardSharedPtr<Type>;

template<typename Type>
using WwiseDBTask = WwiseStandardTask<Type>;

template<typename Type>
using WwiseDBFuture = WwiseStandardFuture<Type>;

template<typename Type>
using WwiseDBSharedFuture = WwiseStandardSharedFuture<Type>;

namespace WwiseDBAsync = WwiseStandardAsync;

using namespace WwiseDBAsync;