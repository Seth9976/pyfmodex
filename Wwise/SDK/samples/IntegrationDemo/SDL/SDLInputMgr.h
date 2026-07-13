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

#if defined(AK_USE_SYSTEM_SDL)
#include <SDL2/SDL_scancode.h>
#else
#include <SDL_scancode.h>
#endif

#include "UniversalInput.h"
#include "IAkInputMgr.h"

class UniversalInput;
union SDL_Event;

class SDLInputMgr : public IAkInputMgr
{
public:

	AK_CALLBACK( void, EventCallbackFunc )(const SDL_Event& in_event);

#ifdef AK_ANDROID
	const int kVirtualGamepadId = 1; // Android only supports the virtual gamepad, so it can be the "primary" controller for Multiplayer demo pages.
#else
	const int kVirtualGamepadId = 0; // Virtual gamepad is 0, other plugged-in gamepads are 1-4.
#endif

	SDLInputMgr();

	/// SDLInputMgr class destructor
	~SDLInputMgr();

	virtual bool Init(AkOSChar* in_szErrorBuffer, unsigned int in_unErrorBufferCharCount) override;

	virtual UniversalInput* UniversalInputAdapter() const override;

	virtual void Update() override;

	virtual void Release() override;

#if defined(INTEGRATIONDEMO_VIRTUAL_GAMEPAD_OVERLAY)

	virtual void AddVirtualGamepad(VirtualGamepadLayout * pLayout) override;
	
	virtual void RemoveVirtualGamepad() override;

#endif

	virtual bool ShouldQuit() const override { return m_bShouldQuit; }
	
	AkUInt32 GetControllerOutputId(int in_iPlayerIndex) const override { return 0; }

	void SetEventHandler(EventCallbackFunc pEventHandler) { m_pEventHandler = pEventHandler; }

private:
	UGBtnState ScanCodeToBtnState( SDL_Scancode in_scanCode ) const;

	/// Pointer to the Universal Input object to interface with the Menu System
	/// \sa UniversalInput
	UniversalInput* m_pUInput = nullptr;
	
#if defined(INTEGRATIONDEMO_VIRTUAL_GAMEPAD_OVERLAY)
	VirtualGamepadLayout * m_pGamepadLayout = nullptr;
#endif

	UGBtnState m_btnKeyboard = 0;

	EventCallbackFunc m_pEventHandler = nullptr;

	bool m_bDidMotion = false;
	bool m_bShouldQuit = false;
};
