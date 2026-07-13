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

// GControllerMgr.h
/// \file
/// Common implementation (iOS and Mac OS X) to support Game Controllerfor

#pragma once

#include "UniversalInput.h"

class GControllerMgr
{
public:
	static void Init();
	static void Term();
	static bool IsConnected(int index);
	static int NumControllers();
	static void GetControllerState(int index, UGBtnState *btnState, UGStickState *stickState);
};
