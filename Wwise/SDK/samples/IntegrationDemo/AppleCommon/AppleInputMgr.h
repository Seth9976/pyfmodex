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

#include "UniversalInput.h"
#include "IAkInputMgr.h"
#ifndef INTEGRATIONDEMO_BYPASS_SDL
#include "SDLInputMgr.h"
#endif

class AppleInputMgr : public IAkInputMgr
{
public:

	virtual bool Init(AkOSChar* in_szErrorBuffer, unsigned int in_unErrorBufferCharCount) override;

	virtual UniversalInput* UniversalInputAdapter() const override;

	virtual void Update() override;

	virtual void Release() override;

    virtual bool ShouldQuit() const override;
	
	virtual AkUInt32 GetControllerOutputId(int in_iPlayerIndex) const override { return (AkUInt32)in_iPlayerIndex; }

	// Sets the controller #1 state for when no gamepads are connected
	// Use for alternative methods of input like keyboards and touch overlays
	void SetDefaultState(UGBtnState in_BtnState, UGStickState * in_StickStates);
	
	void GetDefaultState(UGBtnState& out_BtnState, UGStickState *& out_StickStates);

	void SetGamepadOverlayEnabled(bool in_Enable) { m_OverrideFirstGamepadInput = in_Enable; }
	
private:

	void RemoveDevices();
	void SetupDevices();

	bool m_OverrideFirstGamepadInput = false;

	int m_controllerNum;
#ifndef INTEGRATIONDEMO_BYPASS_SDL
	SDLInputMgr m_SDLInput; //UniversalInput is retrieved from SDLInputMgr to preserve gamepad support
#else
	UniversalInput * m_pInput;
#endif

	UGBtnState m_defaultBtnState;
	UGStickState m_defaultSticksState[2];
};
