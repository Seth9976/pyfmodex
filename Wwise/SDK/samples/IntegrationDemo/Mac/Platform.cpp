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

// Platform.cpp
/// \file 
/// Contains definitions for functions declared in Platform.h

#include "stdafx.h"
#include "Platform.h"
#include "UniversalInput.h"
#include <AK/Tools/Common/AkPlatformFuncs.h>

class RenderingEngine;
RenderingEngine*	g_renderingEngine = NULL;

UInt32 g_uSamplesPerFrame = 512;
