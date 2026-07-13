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

#include "Page.h"

class DemoGMEBasePage : public Page
{
public:
	DemoGMEBasePage(Menu* in_ParentMenu);

protected:
	void AddIdControls();

private:
	void RoomID_ValueChanged(void* in_pSender, ControlEvent* in_pEvent);
	void OpenID_ValueChanged(void* in_pSender, ControlEvent* in_pEvent);
};

class DemoGMEVoiceChatPage : public DemoGMEBasePage
{
public:

	/// DemoGMEVoiceChatPage class constructor
	DemoGMEVoiceChatPage( Menu* in_ParentMenu );

	/// Initializes the demo.
	/// \return True if successful and False otherwise.
	bool Init() override;

	/// Releases resources used by the demo.
	void Release() override;

private:

	/// Initializes the controls on the page.
	void InitControls() override;

	void StreamProfile_ValueChanged(void* in_pSender, ControlEvent* in_pEvent);

	void TeamID_ValueChanged(void* in_pSender, ControlEvent* in_pEvent);
	void TeamMode_ValueChanged(void* in_pSender, ControlEvent* in_pEvent);
	void RecvRange_ValueChanged(void* in_pSender, ControlEvent* in_pEvent);
	void PositionX_ValueChanged(void* in_pSender, ControlEvent* in_pEvent);

	void PlayLocalEffectButton_Pressed(void* in_pSender, ControlEvent* in_pEvent);
	void PlayLocalMusicButton_Pressed(void* in_pSender, ControlEvent* in_pEvent);
	void GMESendButton_Pressed(void* in_pSender, ControlEvent* in_pEvent);
	void PlaySendMusicButton_Pressed(void* in_pSender, ControlEvent* in_pEvent);

	void GMERecAllButton_Pressed(void* in_pSender, ControlEvent* in_pEvent);
	void GMERec1Button_Pressed(void* in_pSender, ControlEvent* in_pEvent);
	void GMERec2Button_Pressed(void* in_pSender, ControlEvent* in_pEvent);
	void GMESFXVolumeButton_Pressed(void* in_pSender, ControlEvent* in_pEvent);
	void GMEReceiveVolumeButton_Pressed(void* in_pSender, ControlEvent* in_pEvent);
	void GMESendVolumeButton_Pressed(void* in_pSender, ControlEvent* in_pEvent);
	void GMESetServerAudioRouteButton_Pressed(void* in_pSender, ControlEvent* in_pEvent);

	int m_nLocalEffectStatus;
	int m_nLocalMusicStatus;
	int m_nSendStatus;
	int m_nSendMusicStatus;
	int m_nReceiveAllStatus;
	int m_nReceive1Status;
	int m_nReceive2Status;

	AkUInt64 m_recv1ID;
	AkUInt64 m_recv2ID;
};
