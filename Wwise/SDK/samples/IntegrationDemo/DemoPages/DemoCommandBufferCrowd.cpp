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

// DemoCommandBufferCrowd.cpp
/// \file
/// Defines all methods declared in DemoCommandBufferCrowd.h

#include "stdafx.h"
#include "DemoCommandBufferCrowd.h"
#include <AK/SoundEngine/Common/AkCommandBuffer.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h>    // Sound engine
#include "Helpers.h"
#include "IntegrationDemo.h"
#include "Menu.h"
#include <AK/SoundEngine/Common/AkAtomic.h>

#define GAME_OBJECT_FIRST_ID 10 // Avoids collision with other registered game objects
#define PLAY_COMMAND_BUFFER_DEMO 1191850276U

DemoCommandBufferCrowd::DemoCommandBufferCrowd(Menu* in_ParentMenu)
	: Page(in_ParentMenu, "Command Buffer with multiple commands")
{
	m_szHelp =
		"This demo shows how to use the Command Buffer API to send high volume of data in bulk to the sound engine. "
		"As you enter the page you will see a crowd of people walking towards you from the left. Each "
		"small circle represents a game object emitter with a custom position sent to the sound engine. "
		"Add more game objects to the scene with <<UG_BUTTON4>> and see how the Sound Engine reacts! "
		"If you encounter voice starvation, you have reached the processing limit of the CPU; use <<UG_BUTTON3>> to remove some of the game objects. "
		"With the arrow keys you can move the listener on the X - Z plane."
		"Note that this demo is best run in Profile or Release configuration to better represent the processing limit of the CPU.";
}

bool DemoCommandBufferCrowd::Init()
{
	// Init our MultiBuffer
	for (int i = 0; i < NUM_OF_BUFFER; i++)
	{
		s_multiBuffer.buffers[i] = AK_CommandBuffer_Create(BUFFER_SIZE);
		AKASSERT(s_multiBuffer.buffers[i] != nullptr);

		if (s_multiBuffer.buffers[i] == nullptr)
		{
			return false;
		}

		AkAtomicStore32(&s_multiBuffer.isBufferAvailable[i], true);
	}
	s_multiBuffer.bufferIndex = 0;

	// Load the sound bank
	AkBankID bankID; // Not used
	if (AK::SoundEngine::LoadBank("Command_Buffer_Demo.bnk", bankID) != AK_Success)
	{
		SetLoadFileErrorMessage("Command_Buffer_Demo.bnk");
		return false;
	}

	InitGameObjects();

	return Page::Init();
}


void DemoCommandBufferCrowd::InitGameObjects()
{
	// Here we could use our MultiBuffer but we can save some memory inside the buffers since we only call postEvent and RegisterGameObject once
	const size_t buffer_size =
		// Add space for the header of the buffer and the AkCommand_EndOfBuffer located at the end of the buffer
		AK_CommandBuffer_MinSize() +
		// Add space for the NPCs RegisterObject command
		(MAX_NUM_OF_NPC * AK_CommandBuffer_CmdSize(AkCommand_RegisterGameObject)) +
		// Add space for the NPCs Name
		(MAX_NUM_OF_NPC * sizeof(char) * 32) +
		// Add space for the NPCs PostEvent command
		(MAX_NUM_OF_NPC * AK_CommandBuffer_CmdSize(AkCommand_PostEvent)) +
		// Add space for all the NPCs SetPosition command
		(MAX_NUM_OF_NPC * AK_CommandBuffer_CmdSize(AkCommand_SetPosition)) +
		// Add space for the listener SetPosition command
		(1 * AK_CommandBuffer_CmdSize(AkCommand_SetPosition));

	void* pInitBuffer = AK_CommandBuffer_Create(buffer_size);
	AKASSERT(pInitBuffer != nullptr);

	float screenWidth = (float)m_pParentMenu->GetWidth();
	float screenHeight = (float)m_pParentMenu->GetHeight();

	// Init every potential NPC in advance to avoid doing it at runtime when new game objects are registered
	for (int i = 0; i < MAX_NUM_OF_NPC; i++) {
		m_NPCs[i].id = GAME_OBJECT_FIRST_ID + i;
		AKPLATFORM::SafeStrFormat(m_NPCs[i].name, 8, "NPC_%d", GAME_OBJECT_FIRST_ID + i);

		float randomNumberX = ((float)rand() / (float)RAND_MAX * (screenWidth));
		float randomNumberY = ((float)rand() / (float)RAND_MAX * (screenHeight - 220)) + 90.0f;

		m_NPCs[i].x = (i >= m_visibleNPC) ? randomNumberX : randomNumberX - (screenWidth + 50);
		m_NPCs[i].y = randomNumberY;

		if ((float)rand() / (float)RAND_MAX < 0.02f)
		{
			m_NPCs[i].speed = 8;
		}
		else {
			m_NPCs[i].speed = 5;
		}
	}

	// register only half the NPCs to the Sound Engine
	for (int i = 0; i < INITIAL_NUM_OF_NPC; i++)
	{
		RegisterGameObject(pInitBuffer, m_NPCs[i], screenHeight, screenWidth);
	}
	// Set Listener initial position
	AkVector64 position = { screenWidth/2, screenHeight/2, 0.0 };

	AkCmd_SetPosition* setPositionCmd = static_cast<AkCmd_SetPosition*>(AK_CommandBuffer_Add(pInitBuffer, AkCommand_SetPosition));
	AKASSERT(setPositionCmd != nullptr);

	setPositionCmd->gameObjectID = LISTENER_ID;
	setPositionCmd->flags = AkSetPositionFlags_Default;
	setPositionCmd->position.position = position;
	setPositionCmd->position.orientationFront = { 0, 0, 1 };
	setPositionCmd->position.orientationTop = { 0, 1, 0 };

	SubmitInitAndReleaseCommandBuffer(pInitBuffer); // Buffer is released in the callback
}

void DemoCommandBufferCrowd::RegisterGameObject(void* out_pBuffer, const NPCInfo& in_NPC, const float in_screenHeight, const float in_screenWidth)
{
		AkCmd_RegisterGameObject* registerGameObjectCmd = static_cast<AkCmd_RegisterGameObject*>(AK_CommandBuffer_Add(out_pBuffer, AkCommand_RegisterGameObject));
		AKASSERT(registerGameObjectCmd != nullptr);

		registerGameObjectCmd->gameObjectID = in_NPC.id;
		AK_CommandBuffer_AddString(out_pBuffer, in_NPC.name); // Adds extra payload data (a name) to the game object

		AkVector64 npcPosition;
		npcPosition.Y = 0;
		npcPosition.X = IntegrationDemoHelpers::PixelsToAKPos_X(in_screenHeight, in_NPC.x);
		npcPosition.Z = IntegrationDemoHelpers::PixelsToAKPos_Y(in_screenWidth, in_NPC.y);

		AkCmd_SetPosition* setPositionCmd = static_cast<AkCmd_SetPosition*>(AK_CommandBuffer_Add(out_pBuffer, AkCommand_SetPosition));
		AKASSERT(setPositionCmd != nullptr);

		setPositionCmd->gameObjectID = in_NPC.id;
		setPositionCmd->flags = AkSetPositionFlags_Default;
		setPositionCmd->position.position = npcPosition;
		setPositionCmd->position.orientationFront = { 0, 0, 1 };
		setPositionCmd->position.orientationTop = { 0, 1, 0 };

		AkCmd_PostEvent* postEventCmd = static_cast<AkCmd_PostEvent*>(AK_CommandBuffer_Add(out_pBuffer, AkCommand_PostEvent));
		AKASSERT(postEventCmd != nullptr);

		postEventCmd->eventID = PLAY_COMMAND_BUFFER_DEMO;
		postEventCmd->playingID = AK_SoundEngine_GeneratePlayingID();
		postEventCmd->gameObjectID = in_NPC.id;
}


void DemoCommandBufferCrowd::UnregisterGameObject(void* out_pBuffer, const int id)
{
	AkCmd_StopAll* stopAllCmd = static_cast<AkCmd_StopAll*>(AK_CommandBuffer_Add(out_pBuffer, AkCommand_StopAll));
	AKASSERT(stopAllCmd != nullptr);

	stopAllCmd->gameObjectID = id;

	AkCmd_RegisterGameObject* unregisterGameObjectCmd = static_cast<AkCmd_RegisterGameObject*>(AK_CommandBuffer_Add(out_pBuffer, AkCommand_UnregisterGameObject));
	AKASSERT(unregisterGameObjectCmd != nullptr);

	unregisterGameObjectCmd->gameObjectID = id;
}

void DemoCommandBufferCrowd::InitControls()
{
	m_pChip = new MovableChip(*this);
	m_pChip->SetLabel("<L>");
	m_pChip->UseJoystick(UG_STICKRIGHT);
	m_pChip->SetMaxSpeed(5.0f);
	m_pChip->SetNonLinear();

	m_fWidth = (float)m_pParentMenu->GetWidth() - (float)m_pChip->GetRightBottomMargin();
	m_fHeight = (float)m_pParentMenu->GetHeight() - (float)m_pChip->GetRightBottomMargin();
}

bool DemoCommandBufferCrowd::Update()
{
	bool bRedraw = false;
	void* pBuffer = nullptr;

	UniversalInput::Iterator it;
	for (it = m_pParentMenu->Input()->Begin(); it != m_pParentMenu->Input()->End(); it++)
	{
		// Skip this input device if it's not connected
		if (!it->IsConnected())
			continue;

		m_pChip->Update(*it);


		if ((*it).IsButtonTriggered(UG_BUTTON3))
		{
			pBuffer = GetAvailableBuffer();
			RemoveNPC(pBuffer);
			SubmitCommandBuffer(pBuffer);
		}
		else if ((*it).IsButtonTriggered(UG_BUTTON4))
		{
			pBuffer = GetAvailableBuffer();
			AddNPC(pBuffer);
			SubmitCommandBuffer(pBuffer);
		}

		bRedraw = true;
	}

	if (bRedraw)
	{
		pBuffer = GetAvailableBuffer();
		UpdateGameObjPos(pBuffer);
		SubmitCommandBuffer(pBuffer);
	}

	return Page::Update();
}

void DemoCommandBufferCrowd::UpdateGameObjPos(void* out_pBuffer)
{
	float x, y;
	m_pChip->GetPos(x, y);

	// Update Listener position
	// Converting X-Y UI into X-Z world plan.
	AkVector64 position;
	position.X = IntegrationDemoHelpers::PixelsToAKPos_X(m_fWidth, x);
	position.Y = 0;
	position.Z = IntegrationDemoHelpers::PixelsToAKPos_Y(m_fHeight, y);

	AkCmd_SetPosition* setPositionCmd = static_cast<AkCmd_SetPosition*>(AK_CommandBuffer_Add(out_pBuffer, AkCommand_SetPosition));
	AKASSERT(setPositionCmd != nullptr);

	setPositionCmd->gameObjectID = LISTENER_ID;
	setPositionCmd->flags = AkSetPositionFlags_Default;
	setPositionCmd->position.position = position;
	setPositionCmd->position.orientationFront = { 0, 0, 1 };
	setPositionCmd->position.orientationTop = { 0, 1, 0 };

	// Update NPCs position
	AkVector64 npcPosition;
	npcPosition.Y = 0;

	for (int i = 0; i < m_visibleNPC; i++)
	{
		MoveNPC(m_NPCs[i]);

		// Converting X-Y UI into X-Z world plan.
		npcPosition.X = IntegrationDemoHelpers::PixelsToAKPos_X(m_fWidth, m_NPCs[i].x);
		npcPosition.Z = IntegrationDemoHelpers::PixelsToAKPos_Y(m_fHeight, m_NPCs[i].y);

		AkCmd_SetPosition* setPositionCmd = static_cast<AkCmd_SetPosition*>(AK_CommandBuffer_Add(out_pBuffer, AkCommand_SetPosition));
		AKASSERT(setPositionCmd != nullptr);

		setPositionCmd->gameObjectID = m_NPCs[i].id;
		setPositionCmd->flags = AkSetPositionFlags_Default;
		setPositionCmd->position.position = npcPosition;
		setPositionCmd->position.orientationFront = { 0, 0, 1 };
		setPositionCmd->position.orientationTop = { 0, 1, 0 };
	}
}

void DemoCommandBufferCrowd::AddNPC(void* out_pBuffer)
{
	if (m_visibleNPC + NPC_INCREASE <= MAX_NUM_OF_NPC)
	{
		for (int i = 0; i < NPC_INCREASE; i++)
		{
			RegisterGameObject(out_pBuffer, m_NPCs[m_visibleNPC], m_fHeight, m_fWidth);

			m_visibleNPC += 1;
		}
	}
}

void DemoCommandBufferCrowd::RemoveNPC(void* out_pBuffer)
{
	if (m_visibleNPC - NPC_INCREASE >= 0)
	{
		for (int i = 0; i < NPC_INCREASE; i++)
		{
			m_visibleNPC -= 1;
			UnregisterGameObject(out_pBuffer, m_NPCs[m_visibleNPC].id);
		}
	}
}

void DemoCommandBufferCrowd::Draw()
{
	const int iTextWidth = 40;	//Approx.

	char strBuf[50];
	int iLineHeight = GetLineHeight(DrawStyle_Text);
	static int s_nOffset = 2 * iLineHeight;
	int iPosX = m_pParentMenu->GetWidth() / 60;
	int iPosY = m_pParentMenu->GetHeight() - s_nOffset;

	// Draw only the visible NPCs
	for (int i = 0; i < m_visibleNPC; i++)
	{
		DrawTextOnScreen("o",(int)m_NPCs[i].x, (int)m_NPCs[i].y, DrawStyle_Selected);
	}

	snprintf(strBuf, 50, "Game Objects: %i", m_visibleNPC);
	DrawTextOnScreen(strBuf, iPosX, iPosY, DrawStyle_Text);

	iPosX = (int)(m_pParentMenu->GetWidth() * 0.4);
	iPosY = m_pParentMenu->GetHeight() - 6 * iLineHeight + 3 * iLineHeight;

	DrawTextOnScreen("(Press <<UG_BUTTON3>> to remove Game Objects...)", iPosX, iPosY, DrawStyle_Text);
	iPosY -= iLineHeight;
	DrawTextOnScreen("(Press <<UG_BUTTON4>> to add Game Objects...)", iPosX, iPosY, DrawStyle_Text);

	m_pChip->Draw();

	Page::Draw();
}

bool DemoCommandBufferCrowd::OnPointerEvent(PointerEventType in_eType, int in_x, int in_y)
{
	if (in_eType == PointerEventType_Moved)
	{
		m_pChip->SetPos((float)in_x, (float)in_y);
	}

	return Page::OnPointerEvent(in_eType, in_x, in_y);
}


void DemoCommandBufferCrowd::Release()
{
	const size_t buffer_size =
		// Add space for the header of the buffer and the AkCommand_EndOfBuffer located at the end of the buffer
		AK_CommandBuffer_MinSize() +
		// Add space for the NPCs StopAll command
		(MAX_NUM_OF_NPC * AK_CommandBuffer_CmdSize(AkCommand_StopAll)) +
		// Add space for the NPCs UnregisterGameObject command
		(MAX_NUM_OF_NPC * AK_CommandBuffer_CmdSize(AkCommand_UnregisterGameObject)) +
		// Add space for the listener SetPosition command
		(1 * AK_CommandBuffer_CmdSize(AkCommand_SetPosition));

	void* pReleaseBuffer = AK_CommandBuffer_Create(buffer_size);
	AKASSERT(pReleaseBuffer != nullptr);

	AkCmd_SetPosition* setPositionCmd = static_cast<AkCmd_SetPosition*>(AK_CommandBuffer_Add(pReleaseBuffer, AkCommand_SetPosition));
	AKASSERT(setPositionCmd != nullptr);

	setPositionCmd->gameObjectID = LISTENER_ID;
	setPositionCmd->flags = AkSetPositionFlags_Default;
	setPositionCmd->position.position = { 0, 0, 0 };
	setPositionCmd->position.orientationFront = { 0, 0, 1 };
	setPositionCmd->position.orientationTop = { 0, 1, 0 };

	for (int i = 0; i < m_visibleNPC; i++)
	{
		UnregisterGameObject(pReleaseBuffer, m_NPCs[i].id);
	}

	SubmitInitAndReleaseCommandBuffer(pReleaseBuffer);

	// Stall until all buffers are available
	bool canBreak = false;
	while (!canBreak)
	{
		AK::SoundEngine::RenderAudio(); // Poke the Sound Engine

		canBreak = true;
		for (int i = 0; i < NUM_OF_BUFFER; i++)
		{
			if (!AkAtomicLoad32(&s_multiBuffer.isBufferAvailable[i]))
			{
				canBreak = false;
				AKPLATFORM::AkSleep(10);
				break;
			}
		}
	}

	// Destroy all our buffers
	for (size_t i = 0; i < NUM_OF_BUFFER; i++)
	{
		AK_CommandBuffer_Destroy(s_multiBuffer.buffers[i]);
	}

	AK::SoundEngine::UnloadBank("Command_Buffer_Demo.bnk", NULL);

	Page::Release();
}

void DemoCommandBufferCrowd::MoveNPC(NPCInfo& out_pNPC)
{

	// Move npc to other side of the screen at a random position when it exits from the right
	if (out_pNPC.x > m_fWidth + 10)
	{
		out_pNPC.x = -10;
		out_pNPC.y = ((float)rand() / (float)RAND_MAX * (m_fHeight - 220)) + 90.0f;
	}

	float randomNumberX = (float)((float)rand() / (float)RAND_MAX - 0.5f) * 3;
	float randomNumberY = (float)((float)rand() / (float)RAND_MAX - 0.5f);

	// Make sure the NPC stays inside a restrained area and doesn't overlap with the text
	if ((out_pNPC.y + randomNumberY) < 100)
	{
		out_pNPC.y += 5;
	}
	if ((out_pNPC.y + randomNumberY) > m_fHeight - 140)
	{
		out_pNPC.y -= 5;
	}

	float playerX, playerY;
	m_pChip->GetPos(playerX, playerY);

	float dxToPlayer = out_pNPC.x - playerX;
	float dyToPlayer = out_pNPC.y - playerY;
	float distanceToPlayer = (float)sqrt(dxToPlayer * dxToPlayer + dyToPlayer * dyToPlayer);

	// Make the NPC move away from the player within a certain radius
    if (distanceToPlayer < 40) {
		out_pNPC.y += (dyToPlayer / (distanceToPlayer/out_pNPC.speed));
	}

	out_pNPC.x += randomNumberX + out_pNPC.speed;
	out_pNPC.y += randomNumberY;
}

// COMMAND BUFFER UTILITIES

void* DemoCommandBufferCrowd::GetAvailableBuffer()
{
	bool areAllBufferUsed = true;

	while (areAllBufferUsed) {
		for (int i = 0; i < NUM_OF_BUFFER; i++)
		{
			if (AkAtomicLoad32(&s_multiBuffer.isBufferAvailable[i]))
			{
				s_multiBuffer.bufferIndex = i;
				areAllBufferUsed = false;

				return s_multiBuffer.buffers[i];
			}
		}

		AKPLATFORM::OutputDebugMsgV("No more buffer available, stalling...\n");
		AKPLATFORM::AkSleep(10);
	}

	return nullptr;
}

void DemoCommandBufferCrowd::SubmitInitAndReleaseCommandBuffer(void* io_pBuffer)
{
	AKASSERT(io_pBuffer != nullptr);
	AkCommandBufferHeader* pHeader = static_cast<AkCommandBufferHeader*>(io_pBuffer);
	pHeader->completionCallback = OnInitAndReleaseCommandBufferDone;
	pHeader->completionCallbackCookie = static_cast<void*>(io_pBuffer); // Index
	AK_CommandBuffer_Submit(io_pBuffer);
}

void DemoCommandBufferCrowd::SubmitCommandBuffer(void* io_pBuffer)
{
	AKASSERT(io_pBuffer != nullptr);

	AkAtomicStore32(&s_multiBuffer.isBufferAvailable[s_multiBuffer.bufferIndex], false);

	AkCommandBufferHeader* pHeader = static_cast<AkCommandBufferHeader*>(io_pBuffer);
	pHeader->completionCallback = OnCommandBufferDone;
	pHeader->completionCallbackCookie = (void*)((AkUIntPtr)s_multiBuffer.bufferIndex);
	AK_CommandBuffer_Submit(io_pBuffer);
}

void DemoCommandBufferCrowd::OnInitAndReleaseCommandBufferDone(void* io_pCookie)
{
	AkCommandBufferIterator it;
	AK_CommandBuffer_Begin(io_pCookie, &it);
	while (AK_CommandBuffer_Next(&it))
	{
		if (it.header->result != AK_Success)
		{
			AKPLATFORM::OutputDebugMsgV("Command with code %d failed: Result = %d\n", it.header->code, it.header->result);
		}
	}

	AK_CommandBuffer_Destroy(io_pCookie);
}

void DemoCommandBufferCrowd::OnCommandBufferDone(void* in_pCookie)
{
	AkUIntPtr bufferIndex = (AkUIntPtr)(in_pCookie); // Here the cookie is just the index of the processed buffer
	void* pBuffer = s_multiBuffer.buffers[bufferIndex];
	AKASSERT(pBuffer != nullptr);

	// Reset the buffer and make it available for reuse
	AK_CommandBuffer_Init(pBuffer, BUFFER_SIZE);
	AkAtomicStore32(&s_multiBuffer.isBufferAvailable[bufferIndex], true);
}

Page* CreateDemoCommandBufferCrowd(Menu* in_pParentMenu)
{
	return new DemoCommandBufferCrowd(in_pParentMenu);
}
