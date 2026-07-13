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

#if defined(WWISE_DB_STANDARD_TYPES)
#include "AdapterTypes/Standard/WwiseStandardTypes.h"
#elif defined(WWISE_DB_UNREAL_TYPES)
#include "AdapterTypes/Unreal/WwiseUnrealTypes.h"
#else
#define WWISE_DB_STANDARD_TYPES
#include "AdapterTypes/Standard/WwiseStandardTypes.h"
#warning No Adapter Types Defined, falling back on Wwise Standard Types
#endif