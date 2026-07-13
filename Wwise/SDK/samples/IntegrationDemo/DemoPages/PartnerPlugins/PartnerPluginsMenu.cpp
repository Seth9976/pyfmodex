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

#include "PartnerPluginsMenu.h"
#include "Menu.h"

#if defined AK_PLATFORM_SUPPORTS_GME
#include "PartnerPlugins/GME/DemoGMEBaseMenuPage.h"
#endif

#include "PartnerPlugins/speechEngine/DemoSpeechEngine.h"

PartnerPluginsMenu::PartnerPluginsMenu(Menu* in_ParentMenu)
	: Page(in_ParentMenu, "Partner Plug-ins Demos Menu")
{
}

void PartnerPluginsMenu::InitControls()
{
	ButtonControl* newBtn;

#ifdef AK_PLATFORM_SUPPORTS_GME
	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("Tencent GME");
	newBtn->SetDelegate((PageMFP)&PartnerPluginsMenu::OpenGMEDemoPage);
	m_Controls.push_back(newBtn);
#endif

#ifdef AK_PLATFORM_SUPPORTS_SPEECH_ENGINE
	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("ReadSpeaker speechEngine");
	newBtn->SetDelegate((PageMFP)&PartnerPluginsMenu::OpenSpeechEngineDemoPage);
	m_Controls.push_back(newBtn);
#endif
}

#ifdef AK_PLATFORM_SUPPORTS_GME
void PartnerPluginsMenu::OpenGMEDemoPage(void*, ControlEvent*)
{
	m_pParentMenu->StackPage(new GMEBaseMenuPage(m_pParentMenu));
}
#endif

#ifdef AK_PLATFORM_SUPPORTS_SPEECH_ENGINE
void PartnerPluginsMenu::OpenSpeechEngineDemoPage(void* in_pSender, ControlEvent* in_pEvent)
{
	CreateDemoSpeechEngine(m_pParentMenu);
}
#endif

Page* CreateDemoPartnerPluginsMenu(Menu* in_pParentMenu)
{
	return  new PartnerPluginsMenu(in_pParentMenu);
}