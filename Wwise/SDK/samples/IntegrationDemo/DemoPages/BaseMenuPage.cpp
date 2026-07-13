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

#include "BaseMenuPage.h"
#include "Menu.h"

#ifdef INTEGRATIONDEMO_APP_LIFECYCLE
#include "DemoAppLifecycle.h"
#endif

/////////////////////////////////////////////////////////////////////
// BaseMenuPage Public Methods
/////////////////////////////////////////////////////////////////////

BaseMenuPage::BaseMenuPage( Menu* in_ParentMenu ):Page( in_ParentMenu, "Wwise Integration Demo" )
{
	m_szHelp  = "Welcome to the Wwise Integration Demonstration! This "
				"application contains samples that demonstrate how to "
				"integrate various Wwise features and concepts into a "
				"game.\n\n"
				"Navigation:\n"
				"  - UP / DOWN on the <<DIRECTIONAL_TYPE>>: move between items on the page.\n"
				"  - <<ACTION_BUTTON>>: activate the selected item.\n"
				"  - <<BACK_BUTTON>>: go back a page.\n\n"
				"Controls:\n"
				"  Certain controls (toggles, sliders) allow you to "
				"change values. Press LEFT/RIGHT on the <<DIRECTIONAL_TYPE>> "
				"to change these controls' values.\n\n"
#ifdef AK_ENABLE_RESOURCE_MONITOR
				"Pressing <<RESOURCE_BUTTON>> at any time will toggle ON/OFF the resource monitor.\n"
#endif
				"Pressing <<HELP_BUTTON>> at any time will display help for the current page.\n";
}

bool BaseMenuPage::Init()
{
	// Load the Init sound bank
	// NOTE: The Init sound bank must be the first bank loaded by Wwise!
	AkBankID bankID;
	if ( AK::SoundEngine::LoadBank( "Init.bnk", bankID ) != AK_Success )
	{
		SetLoadFileErrorMessage( "Init.bnk" );
	}

	if ( AK::SoundEngine::LoadBank( "PauseResume.bnk", bankID ) != AK_Success )
	{
		SetLoadFileErrorMessage( "PauseResume.bnk" );
	}

	// Initialize the page
	return Page::Init();
}

void BaseMenuPage::Release()
{
	// Unload the init soundbank
	AK::SoundEngine::UnloadBank( "Init.bnk", NULL );
	AK::SoundEngine::UnloadBank( "PauseResume.bnk", NULL );

	// Release the page
	Page::Release();
}

bool BaseMenuPage::Update()
{
	// Avoid returning false when the "back" button is pressed, it would immediately quit the app.
	Page::Update();
	return true;
}

void BaseMenuPage::InitControls()
{
	ButtonControl* newBtn;

	// Add button linking to the Dialogue Demos menu
	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "Dialogue Demos" );
	newBtn->SetDelegate( (PageMFP)&BaseMenuPage::OpenDialogueDemosPage );
	m_Controls.push_back( newBtn );
	
	// Add button linking to RTPC Demo (Car Engine)
	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "RTPC Demo (Car Engine)" );
	newBtn->SetDelegate( (PageMFP)&BaseMenuPage::OpenRTPCDemoPage );
	m_Controls.push_back( newBtn );
	
	// Add button linking to Footsteps demo
	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "Footsteps Demo" );
	newBtn->SetDelegate( (PageMFP)&BaseMenuPage::OpenFootstepsDemoPage );
	m_Controls.push_back( newBtn );

	// Add button linking to Markers demo
	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "Subtitles/Markers Demo" );
	newBtn->SetDelegate( (PageMFP)&BaseMenuPage::OpenMarkersDemoPage );
	m_Controls.push_back( newBtn );

	// Add button linking to Music/MIDI demo menu
	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "Music/MIDI Demos" );
	newBtn->SetDelegate( (PageMFP)&BaseMenuPage::OpenMusicMIDIDemoPage );
	m_Controls.push_back( newBtn );

	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "Positioning Demos" );
	newBtn->SetDelegate( (PageMFP)&BaseMenuPage::OpenPositioningDemoPage );
	m_Controls.push_back( newBtn );

	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "Spatial Audio Demos" );
	newBtn->SetDelegate( (PageMFP)&BaseMenuPage::OpenSpatialAudioDemoPage );
	m_Controls.push_back( newBtn );

	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "Bank & Event Loading Demos" );
	newBtn->SetDelegate( (PageMFP)&BaseMenuPage::OpenBankLoadingMenuPage );
	m_Controls.push_back( newBtn );

#if defined INTEGRATIONDEMO_MICROPHONE
	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "Microphone Demo" );
	newBtn->SetDelegate( (PageMFP)&BaseMenuPage::OpenMicrophoneDemoPage );
	m_Controls.push_back( newBtn );
#endif // defined INTEGRATIONDEMO_MICROPHONE
	
#ifdef AK_PLATFORM_SUPPORTS_MOTION
	// Add button linking to Motion demo
	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "Motion Demo" );
	newBtn->SetDelegate( (PageMFP)&BaseMenuPage::OpenMotionDemoPage );
	m_Controls.push_back( newBtn );
#endif

	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("Partner Plug-ins Demos");
	newBtn->SetDelegate((PageMFP)&BaseMenuPage::OpenPartnerPluginsMenuPage);
	m_Controls.push_back(newBtn);

#ifdef INTEGRATIONDEMO_APP_LIFECYCLE
	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "Application Lifecycle Demo" );
	newBtn->SetDelegate( (PageMFP)&BaseMenuPage::OpenAppLifecycleDemoPage );
	m_Controls.push_back( newBtn );
#endif

	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("Channels Demo");
	newBtn->SetDelegate((PageMFP)&BaseMenuPage::OpenChannelsDemoPage);
	m_Controls.push_back(newBtn);

	// Add button linking to the Command Buffer Demo
	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("Command Buffer Demo");
	newBtn->SetDelegate((PageMFP)&BaseMenuPage::OpenCommandBufferDemoPage);
	m_Controls.push_back(newBtn);

	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("Options");
	newBtn->SetDelegate((PageMFP)&BaseMenuPage::OpenOptionsDemoPage);
	m_Controls.push_back(newBtn);

	//TODO: add other demo links here

	// Add an Exit button
	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "Exit" );
	newBtn->SetDelegate( (PageMFP)&BaseMenuPage::ExitButton_OnPress );
	m_Controls.push_back( newBtn );
}


/////////////////////////////////////////////////////////////////////
// BaseMenuPage Private Methods
/////////////////////////////////////////////////////////////////////

void BaseMenuPage::ExitButton_OnPress(void*, ControlEvent*)
{
	m_pParentMenu->QuitApplication();
}

Page* CreateDemoBankLoadingMenu(Menu* in_pParentMenu);
Page* CreateDemoChannels(Menu* in_pParentMenu);
Page* CreateDemoCommandBufferCrowd(Menu* in_pParentMenu);
Page* CreateDemoDialogueMenu(Menu* in_pParentMenu);
Page* CreateDemoFootstepsManyVariables(Menu* in_pParentMenu);
Page* CreateDemoMarkers(Menu* in_pParentMenu);
Page* CreateDemoMusicMidiMenu(Menu* in_pParentMenu);
Page* CreateDemoOptions(Menu* in_pParentMenu);
Page* CreateDemoPartnerPluginsMenu(Menu* in_pParentMenu);
Page* CreateDemoPositioningMenu(Menu* in_pParentMenu);
Page* CreateDemoRtpcCarEngine(Menu* in_pParentMenu);
Page* CreateDemoSpatialAudioMenu(Menu* in_pParentMenu);

#if defined INTEGRATIONDEMO_MICROPHONE
Page* CreateDemoMicrophone(Menu* in_pParentMenu);
#endif

#if defined(AK_PLATFORM_SUPPORTS_MOTION)
Page* CreateDemoMotionPage(Menu* in_pParentMenu);
#endif

#if defined(INTEGRATIONDEMO_APP_LIFECYCLE)
Page* CreateDemoAppLifeCycle(Menu* in_pParentMenu);
#endif

void BaseMenuPage::OpenBankLoadingMenuPage(void*, ControlEvent*)
{
	Page* pg = CreateDemoBankLoadingMenu(m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}

void BaseMenuPage::OpenChannelsDemoPage(void*, ControlEvent*)
{
	Page* pg = CreateDemoChannels(m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}

void BaseMenuPage::OpenCommandBufferDemoPage(void*, ControlEvent*)
{
	Page* pg = CreateDemoCommandBufferCrowd(m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}

void BaseMenuPage::OpenDialogueDemosPage(void*, ControlEvent*)
{
	Page* pg = CreateDemoDialogueMenu(m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}

void BaseMenuPage::OpenFootstepsDemoPage(void*, ControlEvent*)
{
	Page* pg = CreateDemoFootstepsManyVariables(m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}

void BaseMenuPage::OpenMarkersDemoPage(void*, ControlEvent*)
{
	Page* pg = CreateDemoMarkers(m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}

void BaseMenuPage::OpenMusicMIDIDemoPage(void*, ControlEvent*)
{
	Page* pg = CreateDemoMusicMidiMenu(m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}

void BaseMenuPage::OpenOptionsDemoPage(void*, ControlEvent*)
{
	Page* pg = CreateDemoOptions(m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}

void BaseMenuPage::OpenPartnerPluginsMenuPage(void* in_pSender, ControlEvent* in_pEvent)
{
	Page* pg = CreateDemoPartnerPluginsMenu(m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}

void BaseMenuPage::OpenPositioningDemoPage(void*, ControlEvent*)
{
	Page* pg = CreateDemoPositioningMenu(m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}

void BaseMenuPage::OpenRTPCDemoPage(void*, ControlEvent*)
{
	Page* pg = CreateDemoRtpcCarEngine(m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}

void BaseMenuPage::OpenSpatialAudioDemoPage(void*, ControlEvent*)
{
	Page* pg = CreateDemoSpatialAudioMenu(m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}

#if defined INTEGRATIONDEMO_MICROPHONE
void BaseMenuPage::OpenMicrophoneDemoPage(void*, ControlEvent*)
{
	Page* pg = CreateDemoMicrophone(m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}
#endif

#if defined(AK_PLATFORM_SUPPORTS_MOTION)
void BaseMenuPage::OpenMotionDemoPage(void*, ControlEvent*)
{
	Page* pg = CreateDemoMotionPage(m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}
#endif

#if defined(INTEGRATIONDEMO_APP_LIFECYCLE)
void BaseMenuPage::OpenAppLifecycleDemoPage(void*, ControlEvent*)
{
	Page* pg = CreateDemoAppLifeCycle(m_pParentMenu);
	m_pParentMenu->StackPage(pg);
}
#endif


