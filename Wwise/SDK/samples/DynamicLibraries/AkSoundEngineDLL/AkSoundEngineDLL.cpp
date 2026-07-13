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
#include "AkSoundEngineDLL.h"
#include <AK/Tools/Common/AkAssert.h>

#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/Comm/AkCommunication.h>

#include "AkDefaultIOHookDeferred.h"

#include <AK/Plugin/AkVorbisDecoderFactory.h>
#include <AK/Plugin/AkOpusDecoderFactory.h>

namespace
{
	struct DefaultSettings
	{
		AkMemSettings			memSettings;
		AkStreamMgrSettings		stmSettings;
		AkDeviceSettings		deviceSettings;
		AkInitSettings			initSettings;
		AkPlatformInitSettings	platformInitSettings;
#ifndef AK_OPTIMIZED
		AkCommSettings			commSettings;
#endif
	};

	DefaultSettings* k_defaultSettings = nullptr;

	AkCommSettings* GetCommSettings()
	{
#ifndef AK_OPTIMIZED
		AK::Comm::GetDefaultInitSettings(k_defaultSettings->commSettings);
		AKPLATFORM::SafeStrCpy(
			k_defaultSettings->commSettings.szAppNetworkName,
			"DefaultInit",
			AK_COMM_SETTINGS_MAX_STRING_SIZE
		);
		return &k_defaultSettings->commSettings;
#else
		return nullptr;
#endif
	}

}

namespace AK
{
	namespace SOUNDENGINE_DLL
	{
		CAkDefaultIOHookDeferred m_lowLevelIO;
#ifndef AK_OPTIMIZED
		bool m_requiresCommTerm = false;
#endif

		//-----------------------------------------------------------------------------------------
		// Sound Engine initialization.
		//-----------------------------------------------------------------------------------------
		AKRESULT Init(
			AkMemSettings* in_pMemSettings,
			AkStreamMgrSettings* in_pStmSettings,
			AkDeviceSettings* in_pDefaultDeviceSettings,
			AkInitSettings* in_pSettings,
			AkPlatformInitSettings* in_pPlatformSettings,
			AkCommSettings* in_pCommSettings
		)
		{
			// Check required arguments.
			if (!in_pMemSettings ||
				!in_pStmSettings ||
				!in_pDefaultDeviceSettings)
			{
				AKASSERT(!"Invalid arguments");
				return AK_InvalidParameter;
			}

			// Create and initialise an instance of our memory manager.
			if (MemoryMgr::Init(in_pMemSettings) != AK_Success)
			{
				AKASSERT(!"Could not create the memory manager.");
				return AK_Fail;
			}

			// Create and initialise an instance of the default stream manager.
			if (!StreamMgr::Create(*in_pStmSettings))
			{
				AKASSERT(!"Could not create the Stream Manager");
				return AK_Fail;
			}

			// Create an IO device.
			if (m_lowLevelIO.Init(*in_pDefaultDeviceSettings) != AK_Success)
			{
				AKASSERT(!"Cannot create streaming I/O device");
				return AK_Fail;
			}

			// Initialize sound engine.
			if (SoundEngine::Init(in_pSettings, in_pPlatformSettings) != AK_Success)
			{
				AKASSERT(!"Cannot initialize sound engine");
				return AK_Fail;
			}

#ifndef AK_OPTIMIZED
			AkCommSettings settingsComm;
			if (in_pCommSettings == nullptr)
			{
				AK::Comm::GetDefaultInitSettings(settingsComm);
				in_pCommSettings = &settingsComm;
			}

			m_requiresCommTerm = false;
			// Initialize communication.
			if (AK::Comm::Init(*in_pCommSettings) != AK_Success)
			{
				AKASSERT(!"Cannot initialize communication");
				return AK_Fail;
			}
			else
			{
				m_requiresCommTerm = true;
			}
#endif

			return AK_Success;
		}

		AKRESULT InitDefault(
			const AkOSChar* in_pszPluginPath,
			const AkOSChar* in_pszBasePath,
			const AkOSChar* in_pszLanguage)
		{
			k_defaultSettings = new DefaultSettings();
			AK::MemoryMgr::GetDefaultSettings(k_defaultSettings->memSettings);
			AK::StreamMgr::GetDefaultSettings(k_defaultSettings->stmSettings);
			AK::StreamMgr::GetDefaultDeviceSettings(k_defaultSettings->deviceSettings);
			AK::SoundEngine::GetDefaultPlatformInitSettings(k_defaultSettings->platformInitSettings);
			AK::SoundEngine::GetDefaultInitSettings(k_defaultSettings->initSettings);
			k_defaultSettings->initSettings.szPluginDLLPath = in_pszPluginPath;

			AKRESULT res = AK::SOUNDENGINE_DLL::Init(
				&k_defaultSettings->memSettings,
				&k_defaultSettings->stmSettings,
				&k_defaultSettings->deviceSettings,
				&k_defaultSettings->initSettings,
				&k_defaultSettings->platformInitSettings,
				GetCommSettings()
			);

			if (res != AK_Success)
			{
				AKASSERT(!"Cannot initialize SoundEngine.");
				return AK_Fail;
			}

			if (AK::SOUNDENGINE_DLL::SetBasePath(in_pszBasePath) != AK_Success)
			{
				AKASSERT(!"Cannot set Soundbank path.");
				return AK_Fail;
			}

			// Set default language
			return AK::StreamMgr::SetCurrentLanguage(in_pszLanguage);
		}

		//-----------------------------------------------------------------------------------------
		// Sound Engine termination.
		//-----------------------------------------------------------------------------------------
		void Term()
		{
#ifndef AK_OPTIMIZED
			if (m_requiresCommTerm)
				Comm::Term();
#endif

			SoundEngine::Term();

			m_lowLevelIO.Term();
			if (IAkStreamMgr::Get())
				IAkStreamMgr::Get()->Destroy();

			MemoryMgr::Term();

			if (k_defaultSettings != nullptr)
			{
				delete k_defaultSettings;
				k_defaultSettings = nullptr;
			}
		}

		//-----------------------------------------------------------------------------------------
		// Sound Engine periodic update.
		//-----------------------------------------------------------------------------------------
		void Tick()
		{
			SoundEngine::RenderAudio();
		}

		//-----------------------------------------------------------------------------------------
		// Access to LowLevelIO's file localization.
		//-----------------------------------------------------------------------------------------
		// File system interface.
		AKRESULT SetBasePath(const AkOSChar* in_pszBasePath)
		{
			return m_lowLevelIO.SetBasePath(in_pszBasePath);
		}
	}
}

#if defined AK_XBOX
BOOL APIENTRY DllMain( HANDLE hModule, 
	DWORD  ul_reason_for_call, 
	LPVOID lpReserved )
{
	switch ( ul_reason_for_call )
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;

	}
	return TRUE;
}
#endif

extern "C" int AK_SoundEngineDLL_InitDefault(
	const char* in_pluginPath,
	const char* in_basePath,
	const char* in_language)
{
	AkOSChar pluginPath[AK_MAX_PATH], basePath[AK_MAX_PATH], language[AK_MAX_PATH];
	AK_UTF8_TO_OSCHAR(pluginPath, in_pluginPath, AK_MAX_PATH);
	AK_UTF8_TO_OSCHAR(basePath, in_basePath, AK_MAX_PATH);
	AK_UTF8_TO_OSCHAR(language, in_language, AK_MAX_PATH);
	return (int)AK::SOUNDENGINE_DLL::InitDefault(pluginPath, basePath, language);
}

extern "C" void AK_SoundEngineDLL_Term(void)
{
	AK::SOUNDENGINE_DLL::Term();
}

extern "C" int AK_SoundEngineDLL_LoadBank(const char* in_bankName)
{
	AkBankID bankID;
	return (int)AK::SoundEngine::LoadBank(in_bankName, bankID);
}

extern "C" void AK_SoundEngineDLL_Tick(void)
{
	AK::SOUNDENGINE_DLL::Tick();
}
