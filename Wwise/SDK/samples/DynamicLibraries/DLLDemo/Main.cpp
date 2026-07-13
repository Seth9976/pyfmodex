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

#include "stdafx.h"
#include "../AkSoundEngineDLL/AkSoundEngineDLL.h"

#include <AK/SoundEngine/Common/IAkPluginServicePlatformFuncs.h>

#if defined(AK_WIN)
#include "./Windows/PlatformDefinitions.h"
#elif defined(AK_LINUX)
#include "./Linux/PlatformDefinitions.h"
#elif defined(AK_APPLE)
#include "./Mac/PlatformDefinitions.h"
#endif

static const AkGameObjectID LISTENER_ID = 10000;
static const AkGameObjectID GAME_OBJECT_HUMAN = 100;
#define LOOP_DURATION 100

int main()
{
	//
	//Init
	//

	AKRESULT res = AK::SOUNDENGINE_DLL::InitDefault(DLL_PATH, SOUNDBANK_PATH, CURRENT_LANGUAGE);

	AK::IAkPluginServicePlatformFuncs* pPlatformFuncs = AK_GET_PLUGIN_SERVICE_PLATFORMFUNCS(AK::SoundEngine::GetGlobalPluginContext());

	if (res != AK_Success)
	{
		return 1;
	}

	// Register Listener
	AK::SoundEngine::RegisterGameObj(LISTENER_ID, "Listener (Default)");
	AK::SoundEngine::SetDefaultListeners(&LISTENER_ID, 1);

	AkBankID bankID;
	res = AK::SoundEngine::LoadBank("Init.bnk", bankID);
	res = AK::SoundEngine::LoadBank("DLLDemo.bnk", bankID);

	//
	// Main
	//

	AK::SoundEngine::RegisterGameObj(GAME_OBJECT_HUMAN, "Human");
	AkPlayingID m_uCurrentPlayingID = AK::SoundEngine::PostEvent("Play_Hello", GAME_OBJECT_HUMAN, 0);

	for (int i = 0; i < LOOP_DURATION; ++i)
	{
		AK::SOUNDENGINE_DLL::Tick();
		pPlatformFuncs->Sleep(16); // Simulate 60 fps
	}

	//
	// Term
	//

	AK::SoundEngine::StopPlayingID(m_uCurrentPlayingID, 0);

	AK::SoundEngine::UnloadBank("DLLDemo.bnk", NULL);
	AK::SoundEngine::UnloadBank("Init.bnk", NULL);

	AK::SoundEngine::UnregisterGameObj(GAME_OBJECT_HUMAN);
	AK::SoundEngine::UnregisterGameObj(LISTENER_ID);

	AK::SOUNDENGINE_DLL::Term();

	return 0;
}
