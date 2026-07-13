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

#include "AdapterTypes/WwiseDataTypesAdapter.h"
#include "AdapterTypes/WwiseWrapperTypes.h"

enum class WwiseMetadataMediaLocation : WwiseDBShortId;

struct WwiseCRefMedia
{
    const char* name;
    const char* path;
    WwiseDBShortId shortId;
    const char* Language;
    bool bStreaming;
    WwiseMetadataMediaLocation Location;
    const char* CachePath;
};


struct WwiseCRefCustomPlugin
{
    char* name;
    char* path;
    WwiseDBGuid guid;
    WwiseDBShortId shortId;
};

struct WwiseCRefPluginShareSet
{
    char* name;
    char* path;
    WwiseDBGuid guid;
    WwiseDBShortId shortId;
};

struct WwiseCRefAudioDevice
{
    char* name;
    char* path;
    WwiseDBGuid guid;
    WwiseDBShortId shortId;
    WwiseCRefMedia* medias;
};

struct WwiseCRefDialogueEvent
{
    char* name;
    char* path;
    WwiseDBGuid guid;
    WwiseDBShortId shortId;
};

struct WwiseCRefDialogueArgument
{
    char* name;
    char* path;
    WwiseDBGuid guid;
    WwiseDBShortId shortId;
};

struct WwiseCRefBus
{
    const char* busName;
    const char* busPath;
    const WwiseDBGuid* busGuid;
    WwiseDBShortId busShortId;
};

struct WwiseCRefAuxBus
{
    const char* auxBusName;
    const char* auxBusPath;
    const WwiseDBGuid* auxBusGuid;
    WwiseDBShortId auxBusShortId;
};

struct WwiseCRefGameParameter
{
    const char* gameParameterName;
    const char* gameParameterPath;
    const WwiseDBGuid* gameParameterGuid;
    WwiseDBShortId gameParameterShortId;
};

struct WwiseCRefState
{
    const char* stateName;
    const char* statePath;
    const WwiseDBGuid* stateGuid;
    WwiseDBShortId stateShortId;
    WwiseDBShortId groupShortId;
};

struct WwiseCRefGroup
{
    const char* groupName;
    const char* groupPath;
    const WwiseDBGuid* groupGuid;
    WwiseDBShortId groupShortId;
};

struct WwiseCRefSwitch
{
    const char* switchName;
    const char* switchPath;
    const WwiseDBGuid* switchGuid;
    WwiseDBShortId switchShortId;
    WwiseDBShortId groupShortId;
};

struct WwiseCRefTrigger
{
    const char* triggerName;
    const char* triggerPath;
    const WwiseDBGuid* triggerGuid;
    WwiseDBShortId triggerShortId;
};

struct WwiseCRefExternalSource
{
    char* name;
    char* path;
    WwiseDBGuid guid;
    WwiseDBShortId shortId;
};

struct WwiseCRefAcousticTexture
{
    const char* acousticTextureName;
    const char* acousticTexturePath;
    const WwiseDBGuid* acousticTextureGuid;
    WwiseDBShortId acousticTextureShortId;
};

struct WwiseCRefEvent
{
    const char* name;
    const char* path;
    const WwiseDBGuid* guid;
    WwiseDBShortId shortId;
    float maxAttenuation = 0;
    float minDuration = 0;
    float maxDuration = 0;
    WwiseCRefMedia* medias = nullptr;
    unsigned int mediasCount = 0;

    ~WwiseCRefEvent()
    {
        delete[] medias;
    }
};

struct WwiseCRefSoundBank
{
public:
    const char* name;
    const char* path;
    const char* objectPath;
    const char* language;
    WwiseDBShortId languageId;
    const WwiseDBGuid* guid;
    WwiseDBShortId shortId;
    bool bIsUserBank = false;
    bool bIsInitBank = false;
    bool bIsValid = false;

    WwiseCRefMedia* medias = nullptr;
    unsigned int mediasCount = 0;
    WwiseCRefEvent* events = nullptr;
    unsigned int eventsCount = 0;

    ~WwiseCRefSoundBank()
    {
        delete[] events;
        delete[] medias;
    }
};

struct WwiseCRefPlatform
{
    const char* name;
    const WwiseDBGuid* guid;
};