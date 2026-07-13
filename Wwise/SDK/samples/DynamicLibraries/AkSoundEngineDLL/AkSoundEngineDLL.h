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

#ifndef AK_SOUND_ENGINE_DLL_H_
#define AK_SOUND_ENGINE_DLL_H_

#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/SoundEngine/Common/AkMemoryMgrModule.h>
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/SoundEngine/Common/AkSoundEngineExport.h>
#include <AK/Comm/AkCommunication.h>

namespace AK
{
	namespace SOUNDENGINE_DLL
	{
		AKSOUNDENGINE_API AKRESULT Init(
			AkMemSettings *     in_pMemSettings,
			AkStreamMgrSettings *  in_pStmSettings,
			AkDeviceSettings *  in_pDefaultDeviceSettings,
			AkInitSettings *    in_pSettings,
			AkPlatformInitSettings * in_pPlatformSettings,
			AkCommSettings *	in_pCommSettings = nullptr
			);

		AKSOUNDENGINE_API AKRESULT InitDefault(
			const AkOSChar* in_pszPluginPath,
			const AkOSChar* in_pszBasePath,
			const AkOSChar* in_pszLanguage
		);

		AKSOUNDENGINE_API void     Term();

		AKSOUNDENGINE_API void     Tick();

		// File system interface.
		AKSOUNDENGINE_API AKRESULT SetBasePath(
			const AkOSChar*   in_pszBasePath
			);
	}
}

#ifdef __cplusplus
extern "C" {
#endif

AK_EXTERNAPIFUNC(int, AK_SoundEngineDLL_InitDefault)(
	const char* in_pluginPath,
	const char* in_basePath,
	const char* in_language
);
AK_EXTERNAPIFUNC(void, AK_SoundEngineDLL_Term)(void);
AK_EXTERNAPIFUNC(int, AK_SoundEngineDLL_LoadBank)(const char* in_bankName);
AK_EXTERNAPIFUNC(void, AK_SoundEngineDLL_Tick)(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif //AK_SOUND_ENGINE_DLL_H_
