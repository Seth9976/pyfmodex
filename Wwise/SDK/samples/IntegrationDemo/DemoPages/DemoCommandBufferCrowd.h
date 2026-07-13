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

#include <Page.h>
#include <MovableChip.h>
#include <AK/SoundEngine/Common/AkAtomicTypes.h>

int constexpr MAX_NUM_OF_NPC = 501;
int constexpr INITIAL_NUM_OF_NPC = 51;

int constexpr NUM_OF_BUFFER = 3;
int constexpr NPC_INCREASE = 25;

// Memory for each buffer, this will be enough for all our use case
size_t constexpr BUFFER_SIZE = 36'000;


struct NPCInfo {
	float x, y;
	float speed;
	int id;
	char name[32];
};

struct MultiBuffer {
	void* buffers[NUM_OF_BUFFER];
	AkAtomic32 isBufferAvailable[NUM_OF_BUFFER];
	int bufferIndex;
};

class DemoCommandBufferCrowd : public Page
{
public:

	/// DemoCommandBufferCrowd class constructor.
	DemoCommandBufferCrowd(
		Menu* in_pParentMenu	///< - Pointer to the Menu that the page belongs to
	);

	/// Override of the Parent's Init() method.
	virtual bool Init();

	/// Override of the Parent's Release() method.
	virtual void Release();

	/// Override of the Parent's Update() method.
	virtual bool Update();

	/// Override of the Parent's Draw() method.
	virtual void Draw();

	/// Override of the Parent's OnPointerEvent(PointerEventType in_eType, int in_x, int in_y) method.
	virtual bool OnPointerEvent(PointerEventType in_eType, int in_x, int in_y);

	inline static MultiBuffer s_multiBuffer = {};

private:

	/// Initializes the controls on the page.
	virtual void InitControls();
	void InitGameObjects();
	void RegisterGameObject(void* out_pBuffer, const NPCInfo& in_NPC, const float in_screenHeight, const float in_screenWidth);
	void UnregisterGameObject(void* out_pBuffer, const int id);

	void UpdateGameObjPos(void* out_pBuffer);
	void AddNPC(void* out_pBuffer);
	void RemoveNPC(void* out_pBuffer);

	void* GetAvailableBuffer();
	void SubmitInitAndReleaseCommandBuffer(void* io_pBuffer);
	void SubmitCommandBuffer(void* io_pBuffer);
	static void OnInitAndReleaseCommandBufferDone(void* io_pCookie);
	static void OnCommandBufferDone(void* in_pCookie);

	void MoveNPC(NPCInfo& out_pNPC);

	MovableChip* m_pChip = nullptr;
	NPCInfo m_NPCs[MAX_NUM_OF_NPC] = {};

	float m_fWidth = 0.f;
	float m_fHeight = 0.f;
	int m_visibleNPC = INITIAL_NUM_OF_NPC;
};