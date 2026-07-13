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

// DemoSpeechEngine.h
/// \file
/// Contains the function to create the speechEngine demo page.

#pragma once

#ifdef AK_PLATFORM_SUPPORTS_SPEECH_ENGINE

#include "Page.h"

Page* CreateDemoSpeechEngine(Menu* in_parentMenu);

#endif
