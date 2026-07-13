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

// DemoSpeechEngine.cpp
/// \file 
/// Defines the methods declared in DemoSpeechEngine.h.

#include "stdafx.h"

#if defined AK_PLATFORM_SUPPORTS_SPEECH_ENGINE

#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SoundEngine/Common/AkCallback.h>

#include "Menu.h"
#include "Helpers.h"
#include "DemoSpeechEngine.h"
#include "Platform.h"

// For statically linked platforms where the plug-in is supported, link with the plugin if the speechEngine plugin is installed locally.
#if defined(INTDEMO_STATIC_LINK_PREMIUM_PLUGINS) && __has_include(<AK/Plugin/SpeechEngineSourceFactory.h>)
#include <AK/Plugin/SpeechEngineSourceFactory.h>
#pragma comment(lib, "vtapi")
#pragma comment(lib, "SpeechEngineSource")
#endif

#include <string>

#define SPEECH_ENGINE_DLL_NAME AKTEXT("SpeechEngine")

namespace
{
	const std::string kText1 = "Hello! Welcome to the speechEngine plug-in integration demo page.";
	const std::string kText2 = "This is generated using on-device text-to-speech technology.";
	const std::string kText3 = "This is a sentence pause. And this, is a comma pause.";

	const AkUInt32 kCompanyID = 312;
	const AkUInt32 kPluginID = 1;

	const AkUInt32 kBeginWordMarkerID = 'bwrd';
	const AkUInt32 kBeginPhonemeMarkerID = 'bphm';
	const AkUInt32 kEndPhonemeMarkerID = 'ephm';
	const AkUInt32 kEndWordMarkerID = 'ewrd';
}

/// Class representing the speechEngine Demo page.
/// This demo shows how to integrate and interact with the ReadSpeaker speechEngine plug-in
/// for on-device, high-quality text to speech functionality in your application.
/// 
class DemoSpeechEngine : public Page
{
public:

	/// DemoSpeechEngine class constructor
	DemoSpeechEngine(Menu* in_ParentMenu)
		: Page(in_ParentMenu, "speechEngine Demo")
		, m_strCurrentWord()
		, m_strVisemeIDs()
		, m_strUserSelectedText(::kText1)
		, m_strSynthesizedText()
		, m_bAudioPlaying(false)
		, m_iPlayingID(0)
	{
		m_szHelp = "This demo shows how to send custom data to the "
			"speechEngine plug-in as input for the text-to-speech "
			"synthesis. It also shows how to get notified by the generated "
			"audio markers and how to parse them for use in the game engine.\n"
			"Press the \"Generate Speech\" button to trigger the synthesis. ";
	}

	/// Initializes the demo.
	/// \return True if successful and False otherwise.
	bool Init() override
	{
#ifndef INTDEMO_STATIC_LINK_PREMIUM_PLUGINS
		// Register the plug-in
		AKRESULT registerResult = AK::SoundEngine::RegisterPluginDLL(SPEECH_ENGINE_DLL_NAME);
		if (registerResult == AK_Success)
#endif
		{
			// Load the sound bank
			AkBankID bankID; // Not used
			if (AK::SoundEngine::LoadBank("speechEngine.bnk", bankID) != AK_Success)
			{
				SetLoadFileErrorMessage("speechEngine.bnk");
				return false;
			}

			// Register the "Speaker" game object
			AK::SoundEngine::RegisterGameObj(GAME_OBJECT_SPEAKER, "Speaker");

			// Initialize the marker playback variables
			m_bAudioPlaying = false;
		}
#ifndef INTDEMO_STATIC_LINK_PREMIUM_PLUGINS
		else
		{
			SetErrorMessage("Please install the speechEngine plug-in to use this demo.");
			return false;
		}
#endif

		// Initialize the page
		return Page::Init();
	}

	/// Releases resources used by the demo.
	void Release() override
	{
		// Make sure we do not get called back after page destruction
		AK::SoundEngine::CancelEventCallback(m_iPlayingID);

		// Stop the sound playing
		AK::SoundEngine::StopPlayingID(m_iPlayingID);

		// Unregister the "Speaker" game object
		AK::SoundEngine::UnregisterGameObj(GAME_OBJECT_SPEAKER);

		// Unload the sound bank
		AK::SoundEngine::UnloadBank("speechEngine.bnk", NULL);

		// Releases the page
		Page::Release();
	}

	/// Override of the Page::Draw() method.
	void Draw() override
	{
		Page::Draw();

		if (m_bAudioPlaying)
		{
			int iTextPosX = m_pParentMenu->GetWidth() / 4;
			int iTextPosY = static_cast<int>(static_cast<float>(m_pParentMenu->GetHeight()) / 1.5f);

			DrawTextOnScreen("Current word:", iTextPosX, iTextPosY, DrawStyle_Text);
			DrawTextOnScreen(m_strCurrentWord.c_str(), iTextPosX + 250, iTextPosY, DrawStyle_Text);
			DrawTextOnScreen("Viseme IDs for word:", iTextPosX, iTextPosY + 30, DrawStyle_Text);
			DrawTextOnScreen(m_strVisemeIDs.c_str(), iTextPosX + 250, iTextPosY + 30, DrawStyle_Text);

			DrawTextOnScreen("Marker position:", iTextPosX, iTextPosY + 60, DrawStyle_Text);
			DrawTextOnScreen(m_strCurrentMarkerPosition.c_str(), iTextPosX + 250, iTextPosY + 60, DrawStyle_Text);
		}
	}

private:

	/// Initializes the controls on the page.
	void InitControls() override
	{
		ButtonControl* newBtn;
		ToggleControl* newToggle;
		NumericControl* newNumeric;

		// Create the "Generate Speech" button
		newBtn = new ButtonControl(*this);
		newBtn->SetLabel("Generate Speech");
		newBtn->SetDelegate((PageMFP)&DemoSpeechEngine::GenerateSpeechButton_Pressed);
		m_Controls.push_back(newBtn);

		// Add the "Text" toggle
		newToggle = new ToggleControl(*this);
		newToggle->SetLabel("Text:");
		newToggle->AddOption(kText1.c_str());
		newToggle->AddOption(kText2.c_str());
		newToggle->AddOption(kText3.c_str());
		newToggle->SetDelegate((PageMFP)&DemoSpeechEngine::TextToggle_SelectionChanged);
		m_Controls.push_back(newToggle);

		// Add the "Voice" toggle
		newToggle = new ToggleControl(*this);
		newToggle->SetLabel("Voice:");
		newToggle->AddOption("English/American/Sophie");
		newToggle->SetDelegate((PageMFP)&DemoSpeechEngine::VoiceToggle_SelectionChanged);
		m_Controls.push_back(newToggle);

		// Add the Pitch Numeric control
		newNumeric = new NumericControl(*this);
		newNumeric->SetLabel("Pitch:");
		newNumeric->SetMinValue(50);
		newNumeric->SetMaxValue(200);
		newNumeric->SetIncrement(1);
		newNumeric->SetValue(125);
		newNumeric->SetDelegate((PageMFP)&DemoSpeechEngine::PitchNumeric_ValueChanged);
		newNumeric->CallDelegate(NULL); // Force the RTPC to the correct initial value
		m_Controls.push_back(newNumeric);

		// Add the Speed Numeric control
		newNumeric = new NumericControl(*this);
		newNumeric->SetLabel("Speed:");
		newNumeric->SetMinValue(50);
		newNumeric->SetMaxValue(400);
		newNumeric->SetIncrement(1);
		newNumeric->SetValue(125);
		newNumeric->SetDelegate((PageMFP)&DemoSpeechEngine::SpeedNumeric_ValueChanged);
		newNumeric->CallDelegate(NULL); // Force the RTPC to the correct initial value
		m_Controls.push_back(newNumeric);

		// Add the Sentence Pause Numeric control
		newNumeric = new NumericControl(*this);
		newNumeric->SetLabel("SentencePause:");
		newNumeric->SetMinValue(0);
		newNumeric->SetMaxValue(3000);
		newNumeric->SetIncrement(25);
		newNumeric->SetInitialIncrement(1);
		newNumeric->SetValue(0);
		newNumeric->SetDelegate((PageMFP)&DemoSpeechEngine::SentencePauseNumeric_ValueChanged);
		newNumeric->CallDelegate(NULL); // Force the RTPC to the correct initial value
		m_Controls.push_back(newNumeric);

		// Add the Comma Pause Numeric control
		newNumeric = new NumericControl(*this);
		newNumeric->SetLabel("Comma Pause:");
		newNumeric->SetMinValue(0);
		newNumeric->SetMaxValue(3000);
		newNumeric->SetIncrement(25);
		newNumeric->SetInitialIncrement(1);
		newNumeric->SetValue(0);
		newNumeric->SetDelegate((PageMFP)&DemoSpeechEngine::CommaPauseNumeric_ValueChanged);
		newNumeric->CallDelegate(NULL); // Force the RTPC to the correct initial value
		m_Controls.push_back(newNumeric);
	}

	/// Delegate function for the "Generate Speech" button.
	void GenerateSpeechButton_Pressed(void* in_pSender, ControlEvent* in_pEvent)
	{
		if (!m_bAudioPlaying)
		{
			ButtonControl* sender = (ButtonControl*)in_pSender;

			m_bAudioPlaying = true;
			m_strSynthesizedText = m_strUserSelectedText;
			sender->SetLabel("--Playing synthesized speech--");

			// Post the PLAY_SPEECHENGINE event and ask for marker event notifications
			m_iPlayingID = AK::SoundEngine::PostEvent(
				"Play_speechEngine",
				GAME_OBJECT_SPEAKER,
				AK_EndOfEvent | AK_Marker,
				&DemoSpeechEngine::MarkersCallback,
				this);

			// While the SFX or Sound Voice in the Sound Bank could already contain the text to synthesize,
			// if this text is determined at runtime during the game, we must instead sent it to the plug-in as custom game data.
			AK::SoundEngine::SendPluginCustomGameData(
				AK_INVALID_UNIQUE_ID,
				GAME_OBJECT_SPEAKER,
				AkPluginTypeSource,
				::kCompanyID,
				::kPluginID,
				(void*)m_strSynthesizedText.c_str(),
				static_cast<AkUInt32>(m_strSynthesizedText.length()) + 1);
		}
	}

	/// Delegate function for the "Text" toggle.
	void TextToggle_SelectionChanged(void* in_pSender, ControlEvent* in_pEvent)
	{
		ToggleControl* sender = (ToggleControl*)in_pSender;
		m_strUserSelectedText = sender->SelectedText();
	}

	/// Delegate function for the "Voice" toggle.
	void VoiceToggle_SelectionChanged(void* in_pSender, ControlEvent* in_pEvent)
	{
		ToggleControl* sender = (ToggleControl*)in_pSender;
		string szVoice = sender->SelectedText();
	}

	/// Delegate function for the "Pitch" numeric control.
	void PitchNumeric_ValueChanged(void* in_pSender, ControlEvent*)
	{
		NumericControl* sender = (NumericControl*)in_pSender;
		AK::SoundEngine::SetRTPCValue("speechEngine_Pitch", (AkRtpcValue)sender->GetValue(), GAME_OBJECT_SPEAKER);
	}

	/// Delegate function for the "Speed" numeric control.
	void SpeedNumeric_ValueChanged(void* in_pSender, ControlEvent*)
	{
		NumericControl* sender = (NumericControl*)in_pSender;
		AK::SoundEngine::SetRTPCValue("speechEngine_Speed", (AkRtpcValue)sender->GetValue(), GAME_OBJECT_SPEAKER);
	}

	/// Delegate function for the "Sentence Pause" numeric control.
	void SentencePauseNumeric_ValueChanged(void* in_pSender, ControlEvent*)
	{
		NumericControl* sender = (NumericControl*)in_pSender;
		AK::SoundEngine::SetRTPCValue("speechEngine_SentencePause", (AkRtpcValue)sender->GetValue(), GAME_OBJECT_SPEAKER);
	}

	/// Delegate function for the "Comma Pause" numeric control.
	void CommaPauseNumeric_ValueChanged(void* in_pSender, ControlEvent*)
	{
		NumericControl* sender = (NumericControl*)in_pSender;
		AK::SoundEngine::SetRTPCValue("speechEngine_CommaPause", (AkRtpcValue)sender->GetValue(), GAME_OBJECT_SPEAKER);
	}

	/// Callback method for the events raised by Wwise while playing audio and encountering audio markers.
	static void MarkersCallback(
		AkCallbackType in_eType,  ///< The type of the callback
		AkEventCallbackInfo* in_pEventInfo, ///< Basic information about the event
		void* in_pCallbackInfo,   ///< Structure containing info about the markers
		void* in_pCookie          ///< Custom cookie
	)
	{
		DemoSpeechEngine* pPage = (DemoSpeechEngine*)in_pCookie;
		if (in_eType == AK_Marker)
		{
			// As the synthesized speech is played, audio markers will always be received in a similar pattern:
			// BEGIN WORD, BEGIN PHONEME, END PHONEME, ..., BEGIN PHONEME, END PHONEME, END WORD
			// Repeating for each word depending on their length until the final word and phoneme which represents the closing of the mouth.

			AkMarkerCallbackInfo* pMarkerCallbackInfo = (AkMarkerCallbackInfo*)in_pCallbackInfo;

			// An audio marker has the following pieces of important information.
			AkUInt32 uMarkerID = pMarkerCallbackInfo->uIdentifier; // An ID which identify which kind of text-to-speech information it represents.
			AkUInt32 uMarkerPositionInAudioSamples = pMarkerCallbackInfo->uPosition; // The position of the marker in the audio data stream in samples.
			const char* szMarkerLabel = pMarkerCallbackInfo->strLabel; // Additional data based on the marker ID, encoded as a string of characters. See below for details.

			switch (uMarkerID)
			{
			case ::kBeginWordMarkerID:
			{
				// This marker is inserted at the start of a new word.
				// Its label contains the start and end position (delimited by a space) of the word in characters in the text.
				// This could be useful for example, to highlight the word in text shown on screen as it is spoken.

				std::string strMarkerLabel(szMarkerLabel);
				auto delimPos = strMarkerLabel.find(' ');
				if (delimPos != std::string::npos)
				{
					unsigned long uStartOfWord = std::stoul(strMarkerLabel.substr(0, delimPos));
					unsigned long uEndOfWord = std::stoul(strMarkerLabel.substr(delimPos));

					pPage->m_strCurrentWord = pPage->m_strSynthesizedText.substr(uStartOfWord, uEndOfWord - uStartOfWord + 1);
				}

				break;
			}

			case ::kBeginPhonemeMarkerID:
			{
				// This marker is inserted at the beginning of a new phoneme in a word.
				// Its label contains the viseme ID for the phoneme.
				// This could be useful for example, to animate the mouth of the character that is speaking.

				std::string strMarkerLabel(szMarkerLabel);
				pPage->m_strVisemeIDs.append(" " + strMarkerLabel);

				break;
			}

			case ::kEndPhonemeMarkerID:
			{
				// This marker is inserted at the end of a phoneme.
				// Its label does not contain anything and is null.

				break;
			}

			case ::kEndWordMarkerID:
			{
				// This marker is inserted at the end of a word.
				// Its label does not contain anything and is null.

				pPage->m_strVisemeIDs.clear();
				break;
			}
			}

			pPage->m_strCurrentMarkerPosition = std::to_string(uMarkerPositionInAudioSamples);
		}
		else if (in_eType == AK_EndOfEvent)
		{
			pPage->m_Controls[0]->SetLabel("Generate Speech");
			pPage->m_bAudioPlaying = false;
		}
		else
		{
			pPage->SetErrorMessage("Unsupported event occurred");
		}
	}

private:

	/// Tracks the current word that is being played back based on the information found in the audio markers.
	std::string m_strCurrentWord;

	/// Displays all the viseme IDs of the word currently being played.
	std::string m_strVisemeIDs;

	/// Tracks the position in the audio stream of the current audio marker.
	std::string m_strCurrentMarkerPosition;

	/// The input text selected by the user.
	std::string m_strUserSelectedText;

	/// The text that was sent to the speechEngine for audio synthesis and marker generation.
	std::string m_strSynthesizedText;

	/// Whether the audio is currently playing or not.
	bool m_bAudioPlaying;

	/// Holds the playing ID of the launched PLAY_SPEECHENGINE event.
	AkPlayingID m_iPlayingID;

	/// Game Object ID for the "Speaker".
	static const AkGameObjectID GAME_OBJECT_SPEAKER = 100;
};

Page* CreateDemoSpeechEngine(Menu* in_parentMenu)
{
	DemoSpeechEngine* pg = new DemoSpeechEngine(in_parentMenu);
	in_parentMenu->StackPage(pg);

	return pg;
}
#endif