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

// BaseMenuPage.cpp
/// \file 
/// Defines the methods declared in BaseMenuPage.h

#include"stdafx.h"

#include <AK/SoundEngine/Common/AkSoundEngine.h>    // Sound engine

#include "GMEDefines.h"
#include "DemoGMEBaseMenuPage.h"
#include "IntegrationDemo.h"
#include "Menu.h"
#include "DemoGMEVoiceChatPage.h"
#include "DemoGMEVoiceMessagePage.h"
#include "Platform.h"

#ifdef INTDEMO_GME
#include <AK/Plugin/TencentGMEFactory.h>
#endif

/////////////////////////////////////////////////////////////////////
// BaseMenuPage Public Methods
/////////////////////////////////////////////////////////////////////

GMEBaseMenuPage::GMEBaseMenuPage( Menu* in_ParentMenu ):Page( in_ParentMenu, "GME Integration Demo" )
{
	m_szHelp  = "GME Integration Demo.\n\n"
	            "Refer to the documentation available in the Wwise Launcher.\n"
	            "On the Plug-ins page, open the Help (?) menu and select \"Documentation\"\n";
}

bool GMEBaseMenuPage::Init()
{
#ifdef INTDEMO_GME
	AkOutputSettings newSettings = IntegrationDemo::Instance().GetInitSettings().settingsMainOutput;
	newSettings.audioDeviceShareset = AK::SoundEngine::GetIDFromString("GME_System");
	if (AK::SoundEngine::ReplaceOutput(newSettings, 0/*Main Output*/) != AK_Success)
	{
		m_szWarning = "Generate Soundbanks!";
	}
	RegisterCallback();
#else
	m_szWarning = "Install GME, generate SoundBanks and rebuild IntegrationDemo to use this demo.";
#endif // INTDEMO_GME

	// Initialize the page
	return Page::Init();
}

void GMEBaseMenuPage::Release()
{
#ifdef INTDEMO_GME
	ReleaseCallback();
	AkOutputSettings newSettings = IntegrationDemo::Instance().GetInitSettings().settingsMainOutput;
	newSettings.audioDeviceShareset = AK::SoundEngine::GetIDFromString("System");
	AKVERIFY(AK_Success == AK::SoundEngine::ReplaceOutput(newSettings, 0/*Main Output*/));
#endif // INTDEMO_GME

	// Release the page
	Page::Release();
}

bool GMEBaseMenuPage::Update()
{
	return Page::Update();
}

void GMEBaseMenuPage::InitControls()
{
	ButtonControl* newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "Enter Voice Chat Page..." );
	newBtn->SetDelegate( (PageMFP)&GMEBaseMenuPage::OpenGMEVoiceChatPage );
	m_Controls.push_back( newBtn );

	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "Enter Voice Message Page..." );
	newBtn->SetDelegate( (PageMFP)&GMEBaseMenuPage::OpenGMEVoiceMessagePage);
	m_Controls.push_back( newBtn );

	ToggleControl* mRegion = new ToggleControl(*this);
	mRegion->SetLabel("Set GME Connection Region:");
	mRegion->AddOption("Default");
	mRegion->AddOption("International");
	mRegion->AddOption("AWS");
	mRegion->SetDelegate((PageMFP)&GMEBaseMenuPage::SetRegionButton_ValueChanged);
	m_Controls.push_back(mRegion);
}

bool GMEBaseMenuPage::RegisterCallback()
{
	return AK::SoundEngine::RegisterGlobalCallback(&SuspendResumeCallback, AkGlobalCallbackLocation_Suspend | AkGlobalCallbackLocation_WakeupFromSuspend, this) == AK_Success;
}

void GMEBaseMenuPage::ReleaseCallback()
{
	AK::SoundEngine::UnregisterGlobalCallback(&SuspendResumeCallback, AkGlobalCallbackLocation_Suspend | AkGlobalCallbackLocation_WakeupFromSuspend);
}

void GMEBaseMenuPage::SuspendResumeCallback(AK::IAkGlobalPluginContext* in_pContext, AkGlobalCallbackLocation in_eLocation, void* in_pCookie)
{
	switch (in_eLocation)
	{
	case AkGlobalCallbackLocation_Suspend:
#ifdef INTDEMO_GME
		GMEWWisePlugin_Pause();
#endif // INTDEMO_GME
		break;

	case AkGlobalCallbackLocation_WakeupFromSuspend:
#ifdef INTDEMO_GME
		GMEWWisePlugin_Resume();
#endif // INTDEMO_GME
		break;

	default:
		break;
	}
}

/////////////////////////////////////////////////////////////////////
// BaseMenuPage Private Methods
/////////////////////////////////////////////////////////////////////

void GMEBaseMenuPage::OpenGMEVoiceChatPage( void*, ControlEvent* )
{
#ifdef INTDEMO_GME
	DemoGMEVoiceChatPage* pg = new DemoGMEVoiceChatPage( m_pParentMenu );
	m_pParentMenu->StackPage( pg );
#else
	DisplayWarning();
#endif // INTDEMO_GME
}


void GMEBaseMenuPage::OpenGMEVoiceMessagePage( void*, ControlEvent* )
{
#ifdef INTDEMO_GME
	DemoGMEVoiceMessagePage* pg = new DemoGMEVoiceMessagePage( m_pParentMenu );
	m_pParentMenu->StackPage( pg );
#else
	DisplayWarning();
#endif // INTDEMO_GME
}


void GMEBaseMenuPage::SetRegionButton_ValueChanged(void* in_pSender, ControlEvent* in_pEvent)
{
#ifdef INTDEMO_GME
	ToggleControl* sender = (ToggleControl*)in_pSender;
	string szRegion = sender->SelectedText();
	if (!szRegion.compare("Default"))
	{
		GMEWWisePlugin_SetRegion(REGION_DEFAULT);
	}
	else if (!szRegion.compare("International"))
	{
		GMEWWisePlugin_SetRegion(REGION_INTERNATIONAL);
	}
	else if (!szRegion.compare("AWS"))
	{
		GMEWWisePlugin_SetRegion(REGION_AWS);
	}
#else
	DisplayWarning();
#endif // INTDEMO_GME
}
