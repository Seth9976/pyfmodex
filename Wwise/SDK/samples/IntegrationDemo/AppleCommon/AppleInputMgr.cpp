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

// AppleInputMgr.cpp
/// \file 
/// Defines the methods declared in AppleInputMgr.h

#include "stdafx.h"
#include "AppleInputMgr.h"
#include "GControllerMgr.h"
#include "Platform.h"
#include <AK/SoundEngine/Common/AkSoundEngine.h>

bool AppleInputMgr::Init(
	AkOSChar* in_szErrorBuffer,
	unsigned int in_unErrorBufferCharCount
)
{
	m_defaultBtnState = 0;
	memset(m_defaultSticksState, 0, sizeof(m_defaultSticksState));

#ifndef INTEGRATIONDEMO_BYPASS_SDL
	m_SDLInput.Init(in_szErrorBuffer, in_unErrorBufferCharCount);
#else
	m_pInput = new UniversalInput();
#endif

	GControllerMgr::Init();
	SetupDevices(); //This resets the input created by SDLInputMgr

	return true;
}

UniversalInput* AppleInputMgr::UniversalInputAdapter() const
{
#ifndef INTEGRATIONDEMO_BYPASS_SDL
	return m_SDLInput.UniversalInputAdapter();
#else
	return m_pInput;
#endif
}

void AppleInputMgr::RemoveDevices()
{
	for (int i = 0; i < UG_MAX_GAMEPADS; i++)
		UniversalInputAdapter()->RemoveDevice(i + 1);
}

void AppleInputMgr::SetupDevices()
{
	if (UniversalInputAdapter()->NumDevices() > 0)
		RemoveDevices();

	m_controllerNum = GControllerMgr::NumControllers();
	if (m_controllerNum > 0)
	{
		for (int i = 0; i < UG_MAX_GAMEPADS; i++)
			if (GControllerMgr::IsConnected(i) || (i == 0))
			{
				UniversalInputAdapter()->AddDevice(i + 1, UGDeviceType_GAMEPAD, AK::SoundEngine::GetDeviceIDFromPlayerIndex(i));
			}
	}
	else
	{
		UniversalInputAdapter()->AddDevice(1 , UGDeviceType_GAMEPAD);
	}
}

void AppleInputMgr::Update()
{
	if (m_controllerNum != GControllerMgr::NumControllers())
	{
		RemoveDevices();
		SetupDevices();
	}

	int nControllers = std::max(1, GControllerMgr::NumControllers() + 1); 
	for (int i = 0; i < nControllers; i++)
	{
		UGBtnState btnState;
		UGStickState sticksState[2];

		if ((i == 0) && !GControllerMgr::IsConnected(i) && m_OverrideFirstGamepadInput)
		{
			btnState = m_defaultBtnState;
			sticksState[UG_STICKLEFT] = m_defaultSticksState[UG_STICKLEFT];
			sticksState[UG_STICKRIGHT] = m_defaultSticksState[UG_STICKRIGHT];
            UniversalInputAdapter()->SetState(i + 1, true, btnState, sticksState);
		}
		else if(GControllerMgr::IsConnected(i))
		{
			GControllerMgr::GetControllerState(i, &btnState, sticksState);
			UniversalInputAdapter()->SetState(i + 1, true, btnState, sticksState);
		}
	}

#ifndef INTEGRATIONDEMO_BYPASS_SDL
    //SDL events must be pumped even if we will discard inputs...
	//... Unless we're in gamesim, because no SDL instance is running.
	//Remove this ifdef when gamesim supports SDL rendering on apple platforms.
	m_SDLInput.Update();
#endif
	m_defaultBtnState = 0;
}

void AppleInputMgr::Release()
{
#ifndef INTEGRATIONDEMO_BYPASS_SDL
	m_SDLInput.Release();
#else
	delete m_pInput;
#endif
	GControllerMgr::Term();
}

void AppleInputMgr::SetDefaultState(UGBtnState in_BtnState, UGStickState * in_StickStates)
{
	m_defaultBtnState = in_BtnState;
	m_defaultSticksState[UG_STICKLEFT] = in_StickStates[UG_STICKLEFT];
	m_defaultSticksState[UG_STICKRIGHT] = in_StickStates[UG_STICKRIGHT];
}

void AppleInputMgr::GetDefaultState(UGBtnState& out_BtnState, UGStickState *& out_StickStates)
{
	out_BtnState = m_defaultBtnState;
	out_StickStates = m_defaultSticksState;
}

bool AppleInputMgr::ShouldQuit() const
{
#ifndef INTEGRATIONDEMO_BYPASS_SDL
    return m_SDLInput.ShouldQuit();
#else
    return false;
#endif
}
