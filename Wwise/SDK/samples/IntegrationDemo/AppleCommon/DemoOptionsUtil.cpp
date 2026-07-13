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
#include "IntegrationDemo.h"
#include "DemoOptionsUtil.h"
#include "../MenuSystem/ToggleControl.h"
#include <AK/SoundEngine/Common/AkSoundEngine.h>

#if defined(AK_IOS)
#include "AudioSessionUtils.h"
#endif

const char* s_labelVerboseDebugOutput = "Verbose Debug Output";

namespace
{
	constexpr AkUInt32 AmbisonicOrderToNumChannels(AkUInt32 order)
	{
		return (order + 1) * (order + 1);
	}
}

namespace demoOptionsUtil
{	
	void PreInitWwise() {}
	void PostInitWwise() {}

	AkUInt32 GetDefaultSpeakerConfigs(const char**& out_pNames, AkChannelConfig*& out_pConfigs)
	{
		static const char* names[] = {
			"Automatic",
			"Mono",
			"Stereo",
			"5.1",
			"7.1",
			"7.1.4",
			"Ambisonic 3rd order",
			"Ambisonic 4th order",
			"Ambisonic 5th order"
		};
		const size_t uNumNames = sizeof(names) / sizeof(names[0]);

		int i = 0;
		static AkChannelConfig configs[uNumNames];
		configs[i++].SetStandard(0);
		configs[i++].SetStandard(AK_SPEAKER_SETUP_MONO);
		configs[i++].SetStandard(AK_SPEAKER_SETUP_STEREO);
		configs[i++].SetStandard(AK_SPEAKER_SETUP_5POINT1);
		configs[i++].SetStandard(AK_SPEAKER_SETUP_7POINT1);
		configs[i++].SetStandard(AK_SPEAKER_SETUP_DOLBY_7_1_4);
		configs[i++].SetAmbisonic(::AmbisonicOrderToNumChannels(3));
		configs[i++].SetAmbisonic(::AmbisonicOrderToNumChannels(4));
		configs[i++].SetAmbisonic(::AmbisonicOrderToNumChannels(5));

		out_pNames = names;
		out_pConfigs = configs;
		
		return i;
	}

	void InitPlatformSpecificControls(std::vector<Control*>& in_Controls, Page* in_pParent)
	{
		//Add platform specific controls
		int idx = 0;

		ToggleControl* newToggle = new ToggleControl(*in_pParent);
		AddSampleRateOption(newToggle, AK::SoundEngine::GetGlobalPluginContext()->GetPlatformInitSettings()->uSampleRate);
		in_Controls.push_back(newToggle);
		
		AK::IAkGlobalPluginContext* pGlobalCtx = AK::SoundEngine::GetGlobalPluginContext();
		const AkPlatformInitSettings* pfSettings = pGlobalCtx->GetPlatformInitSettings();
		
		// Audio API
		newToggle = new ToggleControl(*in_pParent);
		newToggle->SetLabel(s_labelAudioAPI);
		newToggle->AddOption("Automatic", (void*)AkAudioAPI_Default);
		newToggle->AddOption("AVAudioEngine", (void*)AkAudioAPI_AVAudioEngine);
		newToggle->AddOption("AudioUnit", (void*)AkAudioAPI_AudioUnit);
		
		switch (pfSettings->eAudioAPI)
		{
		case AkAudioAPI_Default: idx = 0; break;
		case AkAudioAPI_AVAudioEngine: idx = 1; break;
		case AkAudioAPI_AudioUnit: idx = 2; break;
		};

		newToggle->SetSelectedIndex(idx);
		in_Controls.push_back(newToggle);
		
		
#if defined(AK_IOS)
		InitializeAudioSessionControls(in_Controls, in_pParent, pfSettings);
#endif

		newToggle = new ToggleControl(*in_pParent);
		newToggle->SetLabel(s_labelVerboseDebugOutput);
		newToggle->AddOption("No", (void*)0);
		newToggle->AddOption("Yes", (void*)1);
		newToggle->SetSelectedIndex(pfSettings->bVerboseSystemOutput);
		in_Controls.push_back(newToggle);
	}

	void FillPlatformSpecificSettings(std::vector<Control*>& in_Controls, AkPlatformInitSettings* io_settings)
	{	
		for (size_t i = 0; i < in_Controls.size(); i++)
		{
			if (in_Controls[i]->GetLabel() == s_labelRate)
				io_settings->uSampleRate = (AkUInt32)(AkUIntPtr)((ToggleControl*)in_Controls[i])->SelectedValue();
			else if (in_Controls[i]->GetLabel() == s_labelAudioAPI)
				io_settings->eAudioAPI = (AkAudioAPI)(AkUIntPtr)((ToggleControl*)in_Controls[i])->SelectedValue();
			else if (in_Controls[i]->GetLabel() == s_labelVerboseDebugOutput)
				io_settings->bVerboseSystemOutput = (AkUIntPtr)((ToggleControl*)in_Controls[i])->SelectedValue() != 0;
		}
		
#if defined(AK_IOS)
		FillAudioSessionSettings(in_Controls, io_settings);
#endif
	}
	
	void UpdatePlatformSpecificControls(std::vector<Control*>& in_Controls)
	{

	}
}
