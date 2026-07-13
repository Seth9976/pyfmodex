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

#include <string>
#include <AK/SoundEngine/Common/AkAtomic.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include "IntegrationDemo.h"

#include "DemoAutobanks.h"

static const char* kEventName = "Play_Hello_Reverb";
static const AkUniqueID kMediaID = 399670944;
static const AkOSChar * kMediaFilename = AKTEXT("399670944.wem");

static const char* kLabels[] = {
	"Load media using PrepareEvent",
	"Un-prepare the event",
	"Load media using SetMedia",
	"Unset media",
	"Post event"
};

DemoAutobanks::DemoAutobanks(Menu* in_ParentMenu)
	: Page(in_ParentMenu, "Autobanks Demo")
	, m_pMediaBuffer(nullptr)
	, m_uMediaSize(0)
	, m_bPrepared(false)
	, m_bMediaSet(false)
{
	m_szHelp =
		"This demo demonstrates the pros and cons of automatic event bank generation.\n\n"
		"When this option is selected in the Project Settings, Wwise will generate individual "
		"banks for any events that are not already contained in a manually created banks. "
		"However, these auto-banks do not contain any media, only structure and event data. "
		"To load media associated with these events, the game must call either "
		"AK::SoundEngine::PrepareEvent or AK::SoundEngine::SetMedia.\n"

		"Both approaches are demonstrated on this page.\n"

		"Note that while using SetMedia gives the game more control over how memory is managed,"
		"the media memory cannot be freed until the sound has finished playing.\n"

		"If the game attempts to unset the media while it is in use, the sound engine will "
		"stop all sounds using this media; however, it takes at least one audio rendering pass "
		"before the media memory can be freed safely, so the game must wait and try again later. ";
}

bool DemoAutobanks::Init()
{
	// Register the "Human" game object
	AK::SoundEngine::RegisterGameObj(GAME_OBJECT_HUMAN, "Human");

	std::string bankName = std::string(kEventName) + std::string(".bnk");
	AK::SoundEngine::LoadBank(bankName.c_str(), m_bankID, AkBankType_Event);

	return Page::Init();
}

void DemoAutobanks::Release()
{
	AK::SoundEngine::StopAll(GAME_OBJECT_HUMAN);

	if (m_bPrepared)
		TogglePrepared();
	if (m_bMediaSet)
		ToggleSetMedia();

	AK::SoundEngine::UnloadBank(m_bankID, nullptr, AkBankType_Event);

	AK::SoundEngine::UnregisterGameObj(GAME_OBJECT_HUMAN);
	Page::Release();
}

void DemoAutobanks::InitControls()
{
	m_btnPrepare = new ButtonControl(*this);
	m_btnPrepare->SetLabel(kLabels[0]);
	m_btnPrepare->SetDelegate((PageMFP)&DemoAutobanks::OnPreparePressed);
	m_Controls.push_back(m_btnPrepare);

	m_btnSetMedia = new ButtonControl(*this);
	m_btnSetMedia->SetLabel(kLabels[2]);
	m_btnSetMedia->SetDelegate((PageMFP)&DemoAutobanks::OnSetMediaPressed);
	m_Controls.push_back(m_btnSetMedia);

	ButtonControl* newBtn;
	newBtn = new ButtonControl(*this);
	newBtn->SetLabel(kLabels[4]);
	newBtn->SetDelegate((PageMFP)&DemoAutobanks::OnPlayPressed);
	m_Controls.push_back(newBtn);
}

void DemoAutobanks::TogglePrepared()
{
	if (!m_bPrepared)
	{
		AK::SoundEngine::PrepareEvent(AK::SoundEngine::Preparation_Load, &kEventName, 1);
		m_btnPrepare->SetLabel(kLabels[1]);
		m_bPrepared = true;
	}
	else
	{
		AK::SoundEngine::PrepareEvent(AK::SoundEngine::Preparation_Unload, &kEventName, 1);
		m_btnPrepare->SetLabel(kLabels[0]);
		m_bPrepared = false;
	}
}

void DemoAutobanks::ToggleSetMedia()
{
	if (!m_bMediaSet)
	{
		// Read media into memory manually; here we are using the sound engine's stream manager because it is cross-platform, but the game engine could read the file by any means available on the current platform.
		AkFileSystemFlags flags(AKCOMPANYID_AUDIOKINETIC, AKCODECID_VORBIS, 0, nullptr, true, AK_INVALID_FILE_ID);
		flags.uDirectoryHash = kMediaID;
		AkFileOpenData fileOpenData = AkFileOpenData(kMediaFilename, &flags);
		
		AK::IAkStdStream* pStream = nullptr;
		AKRESULT eResult = AK::IAkStreamMgr::Get()->CreateStd(fileOpenData, pStream, true);
		if (eResult == AK_Success)
		{
			AkStreamInfo info;
			pStream->GetInfo(info);
			m_pMediaBuffer = AkMalign(AkMemID_Media, (size_t)info.uSize, AK_BANK_PLATFORM_DATA_ALIGNMENT);
			if (m_pMediaBuffer != nullptr)
			{
				AkUInt32 sizeRead = 0;
				eResult = pStream->Read(m_pMediaBuffer, (AkUInt32)info.uSize, true, AK_DEFAULT_PRIORITY, 0.0f, sizeRead);
				if (eResult == AK_Success)
				{
					AKASSERT(sizeRead == info.uSize);
					m_uMediaSize = (AkUInt32)info.uSize;
					AkSourceSettings source;
					source.sourceID = kMediaID;
					source.pMediaMemory = (AkUInt8*)m_pMediaBuffer;
					source.uMediaSize = m_uMediaSize;
					AKVERIFY(AK_Success == AK::SoundEngine::SetMedia(&source, 1));
					m_btnSetMedia->SetLabel(kLabels[3]);
					m_bMediaSet = true;
				}
				else
				{
					SetErrorMessage("Failed to allocate enough memory for loose media file!");
					m_uMediaSize = 0;
				}
			}
			else
			{
				SetErrorMessage("Failed to allocate enough memory for loose media file!");
				m_uMediaSize = 0;
			}

			pStream->Destroy();
		}
		else
		{
			SetErrorMessage("Failed to open loose media file!");
			m_uMediaSize = 0;
		}
	}
	else
	{
		AkSourceSettings source;
		source.sourceID = kMediaID;
		source.pMediaMemory = (AkUInt8*)m_pMediaBuffer;
		source.uMediaSize = m_uMediaSize;
		AKRESULT eResult = AK_ResourceInUse;
		while (eResult == AK_ResourceInUse)
		{
			eResult = AK::SoundEngine::TryUnsetMedia(&source, 1, nullptr);
			if (eResult == AK_ResourceInUse)
			{
				// Media is currently being used by an active sound, so we CANNOT free the memory yet!
				// The previous call to stop may have stopped the sound, but the convolution reverb effect is in its tail. The first call to TryUnsetMedia might end the reverberation tail abruptly.
				// Here you can expect the call to TryUnsetMedia to have requested the media in use
				SetErrorMessage("Media is in use, waiting...!");
#ifndef AK_SUPPORT_THREADS
				AK::SoundEngine::ProcessBanks();
				AK::SoundEngine::RenderAudio();
#endif
				AkThreadYield();
			}
		}
		if (eResult == AK_Success)
		{
			// Media was unset, we can free the memory.
			AkFree(AkMemID_Media, m_pMediaBuffer);
			m_pMediaBuffer = nullptr;
			m_uMediaSize = 0;
			m_btnSetMedia->SetLabel(kLabels[2]);
			m_bMediaSet = false;
		}
		else
		{
			AKASSERT(!"Unexpected error in TryUnsetMedia!");
		}
	}
}

void DemoAutobanks::OnPreparePressed(void* in_pSender, ControlEvent* in_pEvent)
{
	TogglePrepared();
}

void DemoAutobanks::OnSetMediaPressed(void* in_pSender, ControlEvent* in_pEvent)
{
	ToggleSetMedia();
}

void DemoAutobanks::OnPlayPressed(void* in_pSender, ControlEvent* in_pEvent)
{
	AK::SoundEngine::PostEvent(kEventName, GAME_OBJECT_HUMAN);
}
