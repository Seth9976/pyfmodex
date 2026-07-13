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

#include "Metadata/WwiseMetadataForwardDeclarations.h"
#include "AdapterTypes/WwiseDataTypesAdapter.h"
#include "AdapterTypes/WwiseWrapperTypes.h"

#include "AdapterTypes/WwiseMap.h"
#include "AdapterTypes/WwisePair.h"
#include "AdapterTypes/WwiseString.h"

using WwiseMetadataSharedRootFilePtr = WwiseDBSharedPtr<WwiseMetadataRootFile>;
using WwiseMetadataSharedRootFileConstPtr = WwiseDBSharedPtr<WwiseMetadataRootFile>;
using WwiseMetadataFileMap = WwiseDBMap<WwiseDBString, WwiseMetadataSharedRootFilePtr>;

using WwiseMetadataStateWithGroup = WwiseDBPair<WwiseMetadataStateGroup, WwiseMetadataState>;
using WwiseMetadataSwitchWithGroup = WwiseDBPair<WwiseMetadataSwitchGroup, WwiseMetadataSwitch>;
