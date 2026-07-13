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

#include "DemoGMEVoiceChatPage.h"

class DemoGMEVoiceMessagePage : public DemoGMEBasePage
{
public:
	DemoGMEVoiceMessagePage (Menu* in_ParentMenu);
	bool Init() override;
	void Release() override;
	void Draw() override;
	bool Update() override;

private:
	/// Initializes the controls on the page.
	void InitControls() override;

	void PlayGameSoundButton_Pressed(void* in_pSender, ControlEvent* in_pEvent);
	void GMESendButton_Pressed(void* in_pSender, ControlEvent* in_pEvent);
	void GMERecAllButton_Pressed(void* in_pSender, ControlEvent* in_pEvent);

	void StartRecordButton_Pressed(void* in_pSender, ControlEvent* in_pEvent);
	void PlayoutButton_Pressed(void* in_pSender, ControlEvent* in_pEvent);
	void ASRLanguage_ValueChanged(void* in_pSender, ControlEvent* in_pEvent);
	void ASRButton_Pressed(void* in_pSender, ControlEvent* in_pEvent);
	void VoiceDurationButton_Pressed(void* in_pSender, ControlEvent* in_pEvent);

	int m_nLocalMusicStatus;
	int m_nSendStatus;
	int m_nReceiveAllStatus;
	int m_nRecordStatus;
	int m_nPlayStatus;
	int m_nPlayIndexStatus;
	int fileIndex;

	ButtonControl* mPlayMessageBtn;
	ButtonControl* mRecordButton;

	std::string outPutString;
	std::string fileID;
	std::string text;
	std::string language;
};
