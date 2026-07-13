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

#include "WwiseProjectDatabase.h"
#include "WwiseDataStructure.h"

#ifdef _WIN32
#include <windows.h>
#endif

WwiseDataStructure DataStructure;
WwiseDBSharedPlatformId CurrentPlatform;
WwiseDBSharedLanguageId CurrentLanguage;
bool IsInitialized;

/*
* Private Utilities
*/

void GetCurrentPlatformSoundBanks(WwiseSoundBankGlobalIdsMap& outBanks)
{
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        outBanks = DataStructure.Platforms[CurrentPlatform].SoundBanks;
    }
}

void GetCurrentPlatformMedias(WwiseMediaGlobalIdsMap& outMedias)
{
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        outMedias = DataStructure.Platforms[CurrentPlatform].MediaFiles;
    }
}

void GetCurrentPlatformEvents(WwiseEventGlobalIdsMap& outMedias)
{
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        outMedias = DataStructure.Platforms[CurrentPlatform].Events;
    }
}

void GetCurrentPlatformPlugins(WwisePluginLibGlobalIdsMap& outPlugins)
{
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        outPlugins = DataStructure.Platforms[CurrentPlatform].PluginLibs;
    }
}

void GetCurrentPlatformSwitches(WwiseSwitchGlobalIdsMap& outSwitches)
{
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        outSwitches = DataStructure.Platforms[CurrentPlatform].Switches;
    }
}

void GetCurrentPlatformSwitchGroups(WwiseSwitchGroupGlobalIdsMap& outSwitchGroups)
{
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        outSwitchGroups = DataStructure.Platforms[CurrentPlatform].SwitchGroups;
    }
}

void GetCurrentPlatformGameParameters(WwiseGameParameterGlobalIdsMap& outGameParameters)
{
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        outGameParameters = DataStructure.Platforms[CurrentPlatform].GameParameters;
    }
}

void GetCurrentPlatformStateGroups(WwiseStateGroupGlobalIdsMap& outStateGroups)
{
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        outStateGroups = DataStructure.Platforms[CurrentPlatform].StateGroups;
    }
}

void GetCurrentPlatformStates(WwiseStateGlobalIdsMap& outStates)
{
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        outStates = DataStructure.Platforms[CurrentPlatform].States;
    }
}

void GetCurrentPlatformAcousticTextures(WwiseAcousticTextureGlobalIdsMap& outAcousticTextures)
{
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        outAcousticTextures = DataStructure.Platforms[CurrentPlatform].AcousticTextures;
    }
}

void GetCurrentPlatformTriggers(WwiseTriggerGlobalIdsMap& outTriggers)
{
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        outTriggers = DataStructure.Platforms[CurrentPlatform].Triggers;
    }
}

void GetCurrentPlatformBuses(WwiseBusGlobalIdsMap& outBusses)
{
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        outBusses = DataStructure.Platforms[CurrentPlatform].Busses;
    }
}

void GetCurrentPlatformAuxBuses(WwiseAuxBusGlobalIdsMap& outAuxBusses)
{
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        outAuxBusses = DataStructure.Platforms[CurrentPlatform].AuxBusses;
    }
}

void FillMedia(const WwiseRefMedia& InMedia, WwiseCRefMedia& OutCMedias)
{
    if(!InMedia.GetMedia())
        return;
    
    OutCMedias.name = **InMedia.MediaShortName();
    OutCMedias.shortId = InMedia.MediaId();
    OutCMedias.path = **InMedia.MediaPath();
    OutCMedias.Language = *InMedia.GetMedia()->Language;
    OutCMedias.CachePath = *InMedia.GetMedia()->CachePath;
    OutCMedias.bStreaming = InMedia.GetMedia()->bStreaming;
    OutCMedias.Location = InMedia.GetMedia()->Location;
}

void FillEvent(const WwiseRefEvent& WwiseRef, WwiseCRefEvent& OutWwiseCRef, WwiseMediaGlobalIdsMap& MediasMap)
{
    if(!WwiseRef.GetEvent())
        return;
    
    OutWwiseCRef.guid = WwiseRef.EventGuid();
    OutWwiseCRef.shortId = WwiseRef.EventId();
    OutWwiseCRef.name = **WwiseRef.EventName();
    OutWwiseCRef.path = **WwiseRef.EventObjectPath();
    OutWwiseCRef.maxAttenuation = WwiseRef.MaxAttenuation();
    WwiseMetadataEventDurationType durationType;
    float minDuration;
    float maxDuration;
    WwiseRef.GetDuration(durationType, minDuration, maxDuration);
    OutWwiseCRef.minDuration = minDuration;
    OutWwiseCRef.maxDuration = maxDuration;

    WwiseMediaIdsMap Medias;
    WwiseRef.GetAllMedia(MediasMap, Medias);
    if (Medias.Size() > 0)
    {
        int count = 0;
        OutWwiseCRef.medias = new WwiseCRefMedia[Medias.Size()];
        for (const auto& Media : Medias)
        {
            FillMedia(Media.second, OutWwiseCRef.medias[count]);
            count++;
        }
        OutWwiseCRef.mediasCount = count;
    }
}

void FillEvent(const WwiseRefEvent& WwiseRef, WwiseCRefEvent& OutWwiseCRef)
{
    WwiseMediaGlobalIdsMap MediasMap;
    GetCurrentPlatformMedias(MediasMap);
    FillEvent(WwiseRef, OutWwiseCRef, MediasMap);
}

void FillSoundBank(const WwiseRefSoundBank& WwiseRef, WwiseCRefSoundBank& OutWwiseCRef, WwiseEventGlobalIdsMap& EventsMap, WwiseMediaGlobalIdsMap& MediasMap)
{
    if(!WwiseRef.GetSoundBank())
        return;
    
    OutWwiseCRef.guid = WwiseRef.SoundBankGuid();
    OutWwiseCRef.language = **WwiseRef.SoundBankLanguage();
    OutWwiseCRef.languageId = WwiseRef.LanguageId;
    OutWwiseCRef.shortId = WwiseRef.SoundBankId();
    OutWwiseCRef.name = **WwiseRef.SoundBankShortName();
    OutWwiseCRef.objectPath = **WwiseRef.SoundBankObjectPath();
    OutWwiseCRef.path = **WwiseRef.SoundBankPath();
    OutWwiseCRef.bIsInitBank = WwiseRef.IsInitBank();
    OutWwiseCRef.bIsUserBank = WwiseRef.IsUserBank();
    OutWwiseCRef.bIsValid = true;

    auto Medias = WwiseRef.GetSoundBankMedia(MediasMap);
    if (Medias.Size() > 0)
    {
        int count = 0;
        OutWwiseCRef.medias = new WwiseCRefMedia[Medias.Size()];
        for (const auto& Media : Medias)
        {
            FillMedia(Media.second, OutWwiseCRef.medias[count]);
            count++;
        }
        OutWwiseCRef.mediasCount = count;
    }

    auto Events = WwiseRef.GetSoundBankEvents(EventsMap);
    if (Events.Size() > 0)
    {
        OutWwiseCRef.events = new WwiseCRefEvent[Events.Size()];
        int count = 0;
        for (auto& Event : Events)
        {
            FillEvent(Event.second, OutWwiseCRef.events[count], MediasMap);
            count++;
        }
        OutWwiseCRef.eventsCount = count;
    }
}

void FillSoundBank(const WwiseRefSoundBank& WwiseRef, WwiseCRefSoundBank& OutWwiseCRef)
{
    WwiseMediaGlobalIdsMap MediasMap;
    GetCurrentPlatformMedias(MediasMap);
    WwiseEventGlobalIdsMap EventsMap;
    GetCurrentPlatformEvents(EventsMap);
    FillSoundBank(WwiseRef, OutWwiseCRef, EventsMap, MediasMap);
}

void FillPlugin(const WwiseRefPluginLib& WwiseRef, WwiseCRefPlugin& OutWwiseCRef)
{
    OutWwiseCRef.id = WwiseRef.PluginLibId();
    OutWwiseCRef.name = **WwiseRef.PluginLibName();
    OutWwiseCRef.dll = **WwiseRef.PluginLibDLL();
    OutWwiseCRef.staticLib = **WwiseRef.PluginLibStaticLib();
}

void FillSwitchGroup(const WwiseRefSwitchGroup& WwiseRef, WwiseCRefGroup& OutWwiseCRef)
{
    OutWwiseCRef.groupShortId = WwiseRef.SwitchGroupId();
    OutWwiseCRef.groupName = **WwiseRef.SwitchGroupName();
    OutWwiseCRef.groupGuid = WwiseRef.SwitchGroupGuid();
    OutWwiseCRef.groupPath = **WwiseRef.SwitchGroupObjectPath();
}

void FillSwitch(const WwiseRefSwitch& WwiseRef, WwiseCRefSwitch& OutWwiseCRef)
{
    OutWwiseCRef.switchShortId = WwiseRef.SwitchId();
    OutWwiseCRef.switchName = **WwiseRef.SwitchName();
    OutWwiseCRef.switchGuid = WwiseRef.SwitchGuid();
    OutWwiseCRef.switchPath = **WwiseRef.SwitchObjectPath();
    OutWwiseCRef.groupShortId = WwiseRef.GetSwitchGroup()->Id;
}

void FillStateGroup(const WwiseRefStateGroup& WwiseRef, WwiseCRefGroup& OutWwiseCRef)
{
    OutWwiseCRef.groupShortId = WwiseRef.StateGroupId();
    OutWwiseCRef.groupName = **WwiseRef.StateGroupName();
    OutWwiseCRef.groupGuid = WwiseRef.StateGroupGuid();
    OutWwiseCRef.groupPath = **WwiseRef.StateGroupObjectPath();
}

void FillState(const WwiseRefState& WwiseRef, WwiseCRefState& OutWwiseCRef)
{
    OutWwiseCRef.stateShortId = WwiseRef.StateId();
    OutWwiseCRef.stateName = **WwiseRef.StateName();
    OutWwiseCRef.stateGuid = WwiseRef.StateGuid();
    OutWwiseCRef.statePath = **WwiseRef.StateObjectPath();
    OutWwiseCRef.groupShortId = WwiseRef.GetStateGroup()->Id;
}

void FillAcousticTexture(const WwiseRefAcousticTexture& WwiseRef, WwiseCRefAcousticTexture& OutWwiseCRef)
{
    OutWwiseCRef.acousticTextureShortId = WwiseRef.AcousticTextureId();
    OutWwiseCRef.acousticTextureName = **WwiseRef.AcousticTextureName();
    OutWwiseCRef.acousticTextureGuid = WwiseRef.AcousticTextureGuid();
    OutWwiseCRef.acousticTexturePath = **WwiseRef.AcousticTextureObjectPath();
}

void FillGameParameter(const WwiseRefGameParameter& WwiseRef, WwiseCRefGameParameter& OutWwiseCRef)
{
    OutWwiseCRef.gameParameterShortId = WwiseRef.GameParameterId();
    OutWwiseCRef.gameParameterName = **WwiseRef.GameParameterName();
    OutWwiseCRef.gameParameterGuid = WwiseRef.GameParameterGuid();
    OutWwiseCRef.gameParameterPath = **WwiseRef.GameParameterObjectPath();
}

void FillTrigger(const WwiseRefTrigger& WwiseRef, WwiseCRefTrigger& OutWwiseCRef)
{
    OutWwiseCRef.triggerShortId = WwiseRef.TriggerId();
    OutWwiseCRef.triggerName = **WwiseRef.TriggerName();
    OutWwiseCRef.triggerGuid = WwiseRef.TriggerGuid();
    OutWwiseCRef.triggerPath = **WwiseRef.TriggerObjectPath();
}

void FillBus(const WwiseRefBus& WwiseRef, WwiseCRefBus& OutWwiseCRef)
{
    OutWwiseCRef.busShortId = WwiseRef.BusId();
    OutWwiseCRef.busName = **WwiseRef.BusName();
    OutWwiseCRef.busGuid = WwiseRef.BusGuid();
    OutWwiseCRef.busPath = **WwiseRef.BusObjectPath();
}

void FillAuxBus(const WwiseRefAuxBus& WwiseRef, WwiseCRefAuxBus& OutWwiseCRef)
{
    OutWwiseCRef.auxBusShortId = WwiseRef.AuxBusId();
    OutWwiseCRef.auxBusName = **WwiseRef.AuxBusName();
    OutWwiseCRef.auxBusGuid = WwiseRef.AuxBusGuid();
    OutWwiseCRef.auxBusPath = **WwiseRef.AuxBusObjectPath();
}

WwiseCRefEvent* GetEventRef(const WwiseDBObjectInfo& Info)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseRefEvent Event;
    if (!DataStructure.Platforms.Contains(CurrentPlatform))
        return nullptr;
    if (DataStructure.Platforms[CurrentPlatform].GetRef(Event, CurrentLanguage, Info))
    {
        WwiseCRefEvent* CEvent = new WwiseCRefEvent();
        FillEvent(Event, *CEvent);
        return CEvent;
    }
    return nullptr;
}

WwiseCRefSoundBank* GetSoundBankRef(const WwiseDBObjectInfo& Info)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseRefSoundBank SoundBank;
    if (!DataStructure.Platforms.Contains(CurrentPlatform))
        return nullptr;
    if (DataStructure.Platforms[CurrentPlatform].GetRef(SoundBank, CurrentLanguage, Info))
    {
        WwiseCRefSoundBank* CSoundBank = new WwiseCRefSoundBank();
        FillSoundBank(SoundBank, *CSoundBank);
        return CSoundBank;
    }
    return nullptr;
}

WwiseCRefGroup* GetSwitchGroupRef(const WwiseDBObjectInfo& Info)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseRefSwitchGroup Group;
    if (!DataStructure.Platforms.Contains(CurrentPlatform))
        return nullptr;
    if (DataStructure.Platforms[CurrentPlatform].GetRef(Group, CurrentLanguage, Info))
    {
        WwiseCRefGroup* CGroup = new WwiseCRefGroup();
        FillSwitchGroup(Group, *CGroup);
        return CGroup;
    }
    return nullptr;
}

WwiseCRefSwitch* GetSwitchRef(const WwiseDBObjectInfo& Info)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseRefSwitch Switch;
    if (!DataStructure.Platforms.Contains(CurrentPlatform))
        return nullptr;
    if (DataStructure.Platforms[CurrentPlatform].GetRef(Switch, CurrentLanguage, Info))
    {
        WwiseCRefSwitch* CSwitch = new WwiseCRefSwitch();
        FillSwitch(Switch, *CSwitch);
        return CSwitch;
    }
    return nullptr;
}

WwiseCRefGroup* GetStateGroupRef(const WwiseDBObjectInfo& Info)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseRefStateGroup Group;
    if (!DataStructure.Platforms.Contains(CurrentPlatform))
        return nullptr;
    if (DataStructure.Platforms[CurrentPlatform].GetRef(Group, CurrentLanguage, Info))
    {
        WwiseCRefGroup* CGroup = new WwiseCRefGroup();
        FillStateGroup(Group, *CGroup);
        return CGroup;
    }
    return nullptr;
}

WwiseCRefState* GetStateRef(const WwiseDBObjectInfo& Info)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseRefState State;
    if (!DataStructure.Platforms.Contains(CurrentPlatform))
        return nullptr;
    if (DataStructure.Platforms[CurrentPlatform].GetRef(State, CurrentLanguage, Info))
    {
        WwiseCRefState* CState = new WwiseCRefState();
        FillState(State, *CState);
        return CState;
    }
    return nullptr;
}

WwiseCRefAcousticTexture* GetAcousticTextureRef(const WwiseDBObjectInfo& Info)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseRefAcousticTexture AcousticTexture;
    if (!DataStructure.Platforms.Contains(CurrentPlatform))
        return nullptr;
    if (DataStructure.Platforms[CurrentPlatform].GetRef(AcousticTexture, CurrentLanguage, Info))
    {
        WwiseCRefAcousticTexture* CAcousticTexture = new WwiseCRefAcousticTexture();
        FillAcousticTexture(AcousticTexture, *CAcousticTexture);
        return CAcousticTexture;
    }
    return nullptr;
}

WwiseCRefGameParameter* GetGameParameterRef(const WwiseDBObjectInfo& Info)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseRefGameParameter GameParameter;
    if (!DataStructure.Platforms.Contains(CurrentPlatform))
        return nullptr;
    if (DataStructure.Platforms[CurrentPlatform].GetRef(GameParameter, CurrentLanguage, Info))
    {
        WwiseCRefGameParameter* CGameParameter = new WwiseCRefGameParameter();
        FillGameParameter(GameParameter, *CGameParameter);
        return CGameParameter;
    }
    return nullptr;
}

WwiseCRefTrigger* GetTriggerRef(const WwiseDBObjectInfo& Info)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseRefTrigger Trigger;
    if (!DataStructure.Platforms.Contains(CurrentPlatform))
        return nullptr;
    if (DataStructure.Platforms[CurrentPlatform].GetRef(Trigger, CurrentLanguage, Info))
    {
        WwiseCRefTrigger* CTrigger = new WwiseCRefTrigger();
        FillTrigger(Trigger, *CTrigger);
        return CTrigger;
    }
    return nullptr;
}

WwiseCRefBus* GetBusRef(const WwiseDBObjectInfo& Info)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseRefBus Bus;
    if (!DataStructure.Platforms.Contains(CurrentPlatform))
        return nullptr;
    if (DataStructure.Platforms[CurrentPlatform].GetRef(Bus, CurrentLanguage, Info))
    {
        WwiseCRefBus* CBus = new WwiseCRefBus();
        FillBus(Bus, *CBus);
        return CBus;
    }
    return nullptr;
}

WwiseCRefAuxBus* GetAuxBusRef(const WwiseDBObjectInfo& Info)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseRefAuxBus AuxBus;
    if (!DataStructure.Platforms.Contains(CurrentPlatform))
        return nullptr;
    if (DataStructure.Platforms[CurrentPlatform].GetRef(AuxBus, CurrentLanguage, Info))
    {
        WwiseCRefAuxBus* CAuxBus = new WwiseCRefAuxBus();
        FillAuxBus(AuxBus, *CAuxBus);
        return CAuxBus;
    }
    return nullptr;
}

WwiseDBString GetStandardPath(const char* InDirectoryPath)
{
    WwiseDBString standardFilePath = InDirectoryPath;
    standardFilePath.MakeStandardFilename();
    if(!standardFilePath.IsEmpty() && standardFilePath.String.back() != '/' && standardFilePath.String.back() != '\\')
    {
        standardFilePath.String.append("/");
    }
    return standardFilePath;
}

/*
* Exposed Utilities  
*/

void Init(const char* InDirectoryPath, const char* InDirectoryPlatformName)
{
#ifdef _WIN32
        // 1. Get the required size for the UTF-16 string
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, InDirectoryPath, -1, nullptr, 0);
        if (size_needed == 0) {
            return; // Conversion failed
        }

        // 2. Allocate the wstring (subtract 1 for the null terminator)
        std::wstring wstr(size_needed - 1, 0);

        // 3. Convert directly into the wstring buffer
        MultiByteToWideChar(CP_UTF8, 0, InDirectoryPath, -1, &wstr[0], size_needed);

        // 4. Move the wstring directly into the path to avoid a second allocation
        std::filesystem::path filePath(std::move(wstr));
#else
        // Mac / Linux: The OS natively expects UTF-8 char strings for paths.
        // Zero overhead, no conversion necessary.
        std::filesystem::path filePath(InDirectoryPath);
#endif
    IsInitialized = false;
    const auto platformAsStandardString = WwiseStandardString(InDirectoryPlatformName);
    DataStructure = WwiseDataStructure(filePath, &platformAsStandardString);
    const WwiseRefPlatform* RootPlatformByName = DataStructure.RootData.PlatformNames.Find(InDirectoryPlatformName);
    if (RootPlatformByName)
    {
        CurrentPlatform = WwiseDBSharedPlatformId(*RootPlatformByName->PlatformGuid(), InDirectoryPlatformName);
        CurrentLanguage = DataStructure.RootData.DefaultLanguage;
    }
    IsInitialized = true;
}

void SetCurrentPlatform(const char* InDirectoryPlatformName)
{
    if (!IsInitialized)
    {
        return;
    }
    WwiseRefPlatform* platform = DataStructure.RootData.PlatformNames.Find(InDirectoryPlatformName);
    if (platform)
    {
        CurrentPlatform = WwiseDBSharedPlatformId(*platform->PlatformGuid(), InDirectoryPlatformName);
    }
}

void SetCurrentLanguage(const char* InLanguageName)
{
    if (!IsInitialized)
    {
        return;
    }
    WwiseRefLanguage* language = DataStructure.RootData.LanguageNames.Find(InLanguageName);
    if (language)
    {
        CurrentLanguage = WwiseDBSharedLanguageId(language->LanguageId(), InLanguageName, WwiseDBLanguageRequirement::IsOptional);
    }
}

void GetGuidInt(void* InGuid, unsigned int& A, unsigned int& B, unsigned int& C, unsigned int& D)
{
    WwiseDBGuid* Guid = (WwiseDBGuid*)InGuid;
    if(Guid)
    {
        Guid->GetGuidValues(A, B, C, D);
    }
}

/*
    SoundBank
*/

const WwiseCRefSoundBank* GetAllSoundBanksRef()
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseSoundBankGlobalIdsMap SoundBanks;
    GetCurrentPlatformSoundBanks(SoundBanks);
    WwiseCRefSoundBank* allSoundBanks = new WwiseCRefSoundBank[SoundBanks.Size()];
    int index = 0;
    WwiseMediaGlobalIdsMap MediasMap;
    GetCurrentPlatformMedias(MediasMap);
    WwiseEventGlobalIdsMap EventsMap;
    GetCurrentPlatformEvents(EventsMap);
    for (auto const& SoundBank : SoundBanks)
    {
        FillSoundBank(SoundBank.second, allSoundBanks[index], EventsMap, MediasMap);
        ++index;
    }
    return allSoundBanks;
}

const unsigned int GetSoundBankCount()
{
    if (!IsInitialized)
    {
        return 0;
    }
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        return DataStructure.Platforms[CurrentPlatform].SoundBanks.Size();
    }
    return 0;
}

const WwiseCRefSoundBank* GetSoundBankRefIndex(void* InSoundBanksArrayRefPtr, int index)
{
    WwiseCRefSoundBank* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)InSoundBanksArrayRefPtr;
    if (cRef)
    {
        WwiseSoundBankGlobalIdsMap SoundBanks;
        GetCurrentPlatformSoundBanks(SoundBanks);
        if ((unsigned int)index < SoundBanks.Size())
        {
            result = &cRef[index];
        }
    }
    return result;
}

void DeleteSoundBanksArrayRef(void* InSoundBanksArrayRefPtr)
{
    WwiseCRefSoundBank* arg1 = (WwiseCRefSoundBank*)InSoundBanksArrayRefPtr;
    {
        delete[] arg1;
    }
}


const WwiseCRefSoundBank* GetSoundBankRefString(const char* InName, const char* InType)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    return GetSoundBankRef(WwiseDBObjectInfo(WwiseDBGuid(), 0, InName, InType));
}

const char* GetSoundBankName(void* InSoundBankRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->name;
    }
    return result;
}

const char* GetSoundBankPath(void* InSoundBankRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->path;
    }
    return result;
}

const char* GetSoundBankObjectPath(void* InSoundBankRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->objectPath;
    }
    return result;
}

const char* GetSoundBankLanguage(void* InSoundBankRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->language;
    }
    return result;
}

const WwiseDBShortId GetSoundBankLanguageId(void* InSoundBankRefPtr)
{
    WwiseDBShortId result = -1;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->languageId;
    }
    return result;
}

const WwiseDBGuid* GetSoundBankGuid(void* InSoundBankRefPtr)
{
    const WwiseDBGuid* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->guid;
    }
    return result;
}

const WwiseDBShortId GetSoundBankShortId(void* InSoundBankRefPtr)
{
    WwiseDBShortId result = -1;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->shortId;
    }
    return result;
}

 const bool IsUserBank(void* InSoundBankRefPtr)
{
    bool result = 0;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->bIsUserBank;
    }
    return result;
}

 const bool IsInitBank(void* InSoundBankRefPtr)
{
    bool result = 0;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->bIsInitBank;
    }
    return result;
}

 const bool IsSoundBankValid(void* InSoundBankRefPtr)
{
    bool result = 0;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->bIsValid;
    }
    return result;
}

 const WwiseCRefMedia* GetSoundBankMedia(void* InSoundBankRefPtr, int index)
{
    WwiseCRefMedia* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;

    if (cRef)
    {
        if ((unsigned int)index < cRef->mediasCount)
        {
            result = &cRef->medias[index];
        }
    }
    return result;
}

 const unsigned int GetSoundBankMediasCount(void* InSoundBankRefPtr)
{
    unsigned int result = -1;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->mediasCount;
    }
    return result;
}

 const WwiseCRefEvent* GetSoundBankEvent(void* InSoundBankRefPtr, int index)
{
    WwiseCRefEvent* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    
    if (cRef)
    {
        if ((unsigned int)index < cRef->eventsCount)
        {
            result = &cRef->events[index];
        }
    }
    return result;
}

 const unsigned int GetSoundBankEventsCount(void* InSoundBankRefPtr)
{
    unsigned int result = -1;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefSoundBank* cRef = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    if (cRef)
    {
        result = cRef->eventsCount;
    }
    return result;
}


 void DeleteSoundBankRef(void* InSoundBankRefPtr)
{
    WwiseCRefSoundBank* arg1 = (WwiseCRefSoundBank*)InSoundBankRefPtr;
    {
        delete arg1;
    }
}

/*
    Media
*/

 const char* GetMediaName(void* InMediaRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefMedia* cRef = (WwiseCRefMedia*)InMediaRefPtr;
    if(cRef)
    { 
        result = cRef->name;
    }

    return result;
}

 const char* GetMediaPath(void* InMediaRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefMedia* cRef = (WwiseCRefMedia*)InMediaRefPtr;
    if (cRef)
    {
        result = cRef->path;
    }
    return result;
}

 const WwiseDBShortId GetMediaShortId(void* InMediaRefPtr)
{
    WwiseDBShortId result = -1;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefMedia* cRef = (WwiseCRefMedia*)InMediaRefPtr;
    if (cRef)
    {
        result = cRef->shortId;
    }
    return result;
}

 const char* GetMediaLanguage(void* InMediaRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefMedia* cRef = (WwiseCRefMedia*)InMediaRefPtr;
    if (cRef)
    {
        result = cRef->Language;
    }
    return result;
}

 const bool GetMediaIsStreaming(void* InMediaRefPtr)
{
    bool result = false;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefMedia* cRef = (WwiseCRefMedia*)InMediaRefPtr;
    if (cRef)
    {
        result = cRef->bStreaming;
    }
    return result;
}

 const WwiseDBShortId GetMediaLocation(void* InMediaRefPtr)
{
    WwiseDBShortId result = -1;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefMedia* cRef = (WwiseCRefMedia*)InMediaRefPtr;
    if (cRef)
    {
        result = (WwiseDBShortId)cRef->Location;
    }
    return result;
}

 const char* GetMediaCachePath(void* InMediaRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefMedia* cRef = (WwiseCRefMedia*)InMediaRefPtr;
    if (cRef)
    {
        result = cRef->CachePath;
    }
    return result;
}

 void DeleteMediaRef(void* InMediaRefPtr)
{
    WwiseCRefMedia* arg1 = (WwiseCRefMedia*)InMediaRefPtr;
    {
        delete arg1;
    }
}

/*
    Event
*/

unsigned int GetEventCount()
{
    if (!IsInitialized)
    {
        return 0;
    }
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        return DataStructure.Platforms[CurrentPlatform].Events.Size();
    }
    return 0;
}

const WwiseCRefEvent* GetAllEventsRef()
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseEventGlobalIdsMap Events;
    GetCurrentPlatformEvents(Events);
    WwiseCRefEvent* allEvents = new WwiseCRefEvent[Events.Size()];
    int index = 0;
    WwiseMediaGlobalIdsMap MediasMap;
    GetCurrentPlatformMedias(MediasMap);
    for (auto const& Event : Events)
    {
        FillEvent(Event.second, allEvents[index], MediasMap);
        ++index;
    }
    return allEvents;
}

const WwiseCRefEvent* GetEventRefString(const char* InString)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    return GetEventRef(WwiseDBObjectInfo(WwiseDBGuid(), 0, InString, ""));
}

 const char* GetEventName(void* InEventRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefEvent* cRef = (WwiseCRefEvent*)InEventRefPtr;
    if (cRef)
    {
        result = cRef->name;
    }
    return result;
}

 const char* GetEventPath(void* InEventRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefEvent* cRef = (WwiseCRefEvent*)InEventRefPtr;
    if (cRef)
    {
        result = cRef->path;
    }
    return result;
}

 const WwiseDBGuid* GetEventGuid(void* InEventRefPtr)
{
    const WwiseDBGuid* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefEvent* cRef = (WwiseCRefEvent*)InEventRefPtr;
    if (cRef)
    {
        result = cRef->guid;
    }
    return result;
}

 const WwiseDBShortId GetEventShortId(void* InEventRefPtr)
{
    WwiseDBShortId result = -1;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefEvent* cRef = (WwiseCRefEvent*)InEventRefPtr;
    if (cRef)
    {
        result = cRef->shortId;
    }
    return result;
}

 const float GetEventMaxAttenuation(void* InEventRefPtr)
{
    float result = -1;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefEvent* cRef = (WwiseCRefEvent*)InEventRefPtr;
    if (cRef)
    {
        result = cRef->maxAttenuation;
    }
    return result;
}

 const float GetEventMinDuration(void* InEventRefPtr)
 {
     float result = -1;
    if (!IsInitialized)
    {
        return result;
    }
     WwiseCRefEvent* cRef = (WwiseCRefEvent*)InEventRefPtr;
     if (cRef)
     {
         result = cRef->minDuration;
     }
     return result;
 }

 const float GetEventMaxDuration(void* InEventRefPtr)
 {
     float result = -1;
    if (!IsInitialized)
    {
        return result;
    }
     WwiseCRefEvent* cRef = (WwiseCRefEvent*)InEventRefPtr;
     if (cRef)
     {
         result = cRef->maxDuration;
     }
     return result;
 }

 const WwiseCRefMedia* GetEventMedia(void* InEventRefPtr, int index)
{
    WwiseCRefMedia* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefEvent* cRef = (WwiseCRefEvent*)InEventRefPtr;

    if (cRef)
    {
        if ((unsigned int)index < cRef->mediasCount)
        {
            result = &cRef->medias[index];
        }
    }
    return result;
}

 const unsigned int GetEventMediasCount(void* InEventRefPtr)
{
    unsigned int result = -1;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefEvent* cRef = (WwiseCRefEvent*)InEventRefPtr;
    if (cRef)
    {
        result = cRef->mediasCount;
    }
    return result;
}

 void DeleteEventRef(void* InEventRefPtr)
{
    WwiseCRefEvent* arg1 = (WwiseCRefEvent*)InEventRefPtr;
    {
        delete arg1;
    }
}

void DeleteEventArrayRef(void* InEventArrayRefPtr)
{
    WwiseCRefEvent* arg1 = (WwiseCRefEvent*)InEventArrayRefPtr;
    {
        delete[] arg1;
    }
}

const WwiseCRefEvent* GetEvent(void* InEventArrayRefPtr, int index)
{
    WwiseCRefEvent* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefEvent* cRef = (WwiseCRefEvent*)InEventArrayRefPtr;
    
    if (cRef)
    {
        if ((unsigned int)index < GetEventCount())
        {
            result = &cRef[index];
        }
    }
    return result;
}

const WwiseCRefPlatform* GetPlatformRef(const char* InString)
{
    auto platformRef = new WwiseCRefPlatform();
    if (!IsInitialized)
    {
        return platformRef;
    }
    for(auto platformName : DataStructure.RootData.PlatformNames)
    {
        if(*platformName.second.PlatformName() == InString)
        {
            platformRef->name = **platformName.second.PlatformName();
            platformRef->guid = platformName.second.PlatformGuid();
            return platformRef;
        }
    }
    return platformRef;
}

const char* GetPlatformName(void* InPlatformPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefPlatform* cRef = (WwiseCRefPlatform*)InPlatformPtr;
    if (cRef)
    {
        result = cRef->name;
    }
    return result;
}

const WwiseDBGuid* GetPlatformGuid(void* InPlatformPtr)
{
    const WwiseDBGuid* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefPlatform* cRef = (WwiseCRefPlatform*)InPlatformPtr;
    if (cRef)
    {
        result = cRef->guid;
    }
    return result;
}

void DeletePlatformRef(void* InPlatformPtr)
{
    WwiseCRefPlatform* arg1 = (WwiseCRefPlatform*)InPlatformPtr;
    {
        delete arg1;
    }
}

const WwiseCRefPlugin* GetAllPluginRef()
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    if(!DataStructure.Platforms.Contains(CurrentPlatform))
        return nullptr;
    WwisePluginLibGlobalIdsMap Plugins;
    GetCurrentPlatformPlugins(Plugins);
    WwiseCRefPlugin* allPlugins = new WwiseCRefPlugin[Plugins.Size()];
    int index = 0;
    for (auto const& Plugin : Plugins)
    {
        FillPlugin(Plugin.second, allPlugins[index]);
        ++index;
    }
    return allPlugins;
}

const unsigned int GetPluginCount()
{
    if (!IsInitialized)
    {
        return 0;
    }
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        return DataStructure.Platforms[CurrentPlatform].PluginLibs.Size();
    }
    return 0;
}

const WwiseCRefPlugin* GetPluginRefIndex(void* InPluginArrayRefPtr, int index)
{
    WwiseCRefPlugin* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefPlugin* cRef = (WwiseCRefPlugin*)InPluginArrayRefPtr;

    if (cRef)
    {
        if ((unsigned int)index < GetPluginCount())
            
        {
            result = &cRef[index];
        }
    }
    return result;
}

const WwiseDBShortId GetPluginId(void* InPluginRefPtr)
{
    WwiseDBShortId result = -1;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefPlugin* cRef = (WwiseCRefPlugin*)InPluginRefPtr;
    if (cRef)
    {
        result = cRef->id;
    }
    return result;
}

const char* GetPluginName(void* InPluginRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefPlugin* cRef = (WwiseCRefPlugin*)InPluginRefPtr;
    if (cRef)
    {
        result = cRef->name;
    }
    return result;
}

const char* GetPluginDLL(void* InPluginRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefPlugin* cRef = (WwiseCRefPlugin*)InPluginRefPtr;
    if (cRef)
    {
        result = cRef->dll;
    }
    return result;
}

const char* GetPluginStaticLib(void* InPluginRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefPlugin* cRef = (WwiseCRefPlugin*)InPluginRefPtr;
    if (cRef)
    {
        result = cRef->staticLib;
    }
    return result;
}

void DeletePluginRef(void* InPluginRefPtr)
{
    WwiseCRefPlugin* arg1 = (WwiseCRefPlugin*)InPluginRefPtr;
    {
        delete arg1;
    }
}

void DeletePluginArrayRef(void* InPluginArrayRefPtr)
{
    WwiseCRefPlugin* arg1 = (WwiseCRefPlugin*)InPluginArrayRefPtr;
    {
        delete[] arg1;
    }
}

const WwiseCRefGroup* GetAllSwitchGroupRef()
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseSwitchGroupGlobalIdsMap SwitchGroups;
    GetCurrentPlatformSwitchGroups(SwitchGroups);
    WwiseCRefGroup* allSwitchGroups = new WwiseCRefGroup[SwitchGroups.Size()];
    int index = 0;
    for (auto const& SwitchGroup : SwitchGroups)
    {
        FillSwitchGroup(SwitchGroup.second, allSwitchGroups[index]);
        ++index;
    }
    return allSwitchGroups;
}

const unsigned int GetSwitchGroupCount()
{
    if (!IsInitialized)
    {
        return 0;
    }
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        return DataStructure.Platforms[CurrentPlatform].SwitchGroups.Size();
    }
    return 0;
}

void DeleteSwitchGroupRef(void* InSwitchGroupRefPtr)
{
    WwiseCRefGroup* arg1 = (WwiseCRefGroup*)InSwitchGroupRefPtr;
    {
        delete arg1;
    }
}

void DeleteSwitchGroupArrayRef(void* InSwitchGroupArrayRefPtr)
{
    WwiseCRefGroup* arg1 = (WwiseCRefGroup*)InSwitchGroupArrayRefPtr;
    {
        delete[] arg1;
    }
}

const WwiseCRefGroup* GetSwitchGroup(void* InSwitchGroupArrayRefPtr, int index)
{
    WwiseCRefGroup* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefGroup* cRef = (WwiseCRefGroup*)InSwitchGroupArrayRefPtr;
    
    if (cRef)
    {
        if ((unsigned int)index < GetSwitchGroupCount())
        {
            result = &cRef[index];
        }
    }
    return result;
}

const char* GetSwitchGroupName(void* InSwitchGroupRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefGroup* cRef = (WwiseCRefGroup*)InSwitchGroupRefPtr;
    if (cRef)
    {
        result = cRef->groupName;
    }
    return result;
}

const char* GetSwitchGroupPath(void* InSwitchGroupRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefGroup* cRef = (WwiseCRefGroup*)InSwitchGroupRefPtr;
    if (cRef)
    {
        result = cRef->groupPath;
    }
    return result;
}

const WwiseDBGuid* GetSwitchGroupGuid(void* InSwitchGroupRefPtr)
{
    const WwiseDBGuid* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefSwitch* cRef = (WwiseCRefSwitch*)InSwitchGroupRefPtr;
    if (cRef)
    {
        result = cRef->switchGuid;
    }
    return result;
}

const WwiseDBShortId GetSwitchGroupShortId(void* InSwitchGroupRefPtr)
{
    WwiseDBShortId result = -1;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefGroup* cRef = (WwiseCRefGroup*)InSwitchGroupRefPtr;
    if (cRef)
    {
        result = cRef->groupShortId;
    }
    return result;
}

const WwiseCRefGroup* GetSwitchGroupRefString(const char* InString)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    return GetSwitchGroupRef(WwiseDBObjectInfo(WwiseDBGuid(), 0, InString, ""));
}

const WwiseCRefSwitch* GetAllSwitchRef()
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseSwitchGlobalIdsMap Switches;
    GetCurrentPlatformSwitches(Switches);
    WwiseCRefSwitch* allSwitches = new WwiseCRefSwitch[Switches.Size()];
    int index = 0;
    for (auto const& Switch : Switches)
    {
        FillSwitch(Switch.second, allSwitches[index]);
        ++index;
    }
    return allSwitches;
}

const unsigned int GetSwitchCount()
{
    if (!IsInitialized)
    {
        return 0;
    }
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        return DataStructure.Platforms[CurrentPlatform].Switches.Size();
    }
    return 0;
}

void DeleteSwitchRef(void* InSwitchRefPtr)
{
    WwiseCRefSwitch* arg1 = (WwiseCRefSwitch*)InSwitchRefPtr;
    {
        delete arg1;
    }
}

void DeleteSwitchArrayRef(void* InSwitchArrayRefPtr)
{
    WwiseCRefSwitch* arg1 = (WwiseCRefSwitch*)InSwitchArrayRefPtr;
    {
        delete[] arg1;
    }
}

const WwiseCRefSwitch* GetSwitch(void* InSwitchArrayRefPtr, int index)
{
    WwiseCRefSwitch* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefSwitch* cRef = (WwiseCRefSwitch*)InSwitchArrayRefPtr;
    
    if (cRef)
    {
        if ((unsigned int)index < GetSwitchCount())
        {
            result = &cRef[index];
        }
    }
    return result;
}

const char* GetSwitchName(void* InSwitchRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefSwitch* cRef = (WwiseCRefSwitch*)InSwitchRefPtr;
    if (cRef)
    {
        result = cRef->switchName;
    }
    return result;
}

const char* GetSwitchPath(void* InSwitchRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefSwitch* cRef = (WwiseCRefSwitch*)InSwitchRefPtr;
    if (cRef)
    {
        result = cRef->switchPath;
    }
    return result;
}

const WwiseDBGuid* GetSwitchGuid(void* InSwitchRefPtr)
{
    const WwiseDBGuid* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefSwitch* cRef = (WwiseCRefSwitch*)InSwitchRefPtr;
    if (cRef)
    {
        result = cRef->switchGuid;
    }
    return result;
}

const WwiseDBShortId GetSwitchShortId(void* InSwitchRefPtr)
{
    WwiseDBShortId result = -1;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefSwitch* cRef = (WwiseCRefSwitch*)InSwitchRefPtr;
    if (cRef)
    {
        result = cRef->switchShortId;
    }
    return result;
}

const WwiseDBShortId GetSwitchGroupId(void* InSwitchRefPtr)
{
    WwiseDBShortId result = -1;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefSwitch* cRef = (WwiseCRefSwitch*)InSwitchRefPtr;
    if (cRef)
    {
        result = cRef->groupShortId;
    }
    return result;
}

const WwiseCRefSwitch* GetSwitchRefString(const char* InString)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    return GetSwitchRef(WwiseDBObjectInfo(WwiseDBGuid(), 0, InString, ""));
}


const WwiseCRefGroup* GetAllStateGroupRef()
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseStateGroupGlobalIdsMap StateGroups;
    GetCurrentPlatformStateGroups(StateGroups);
    WwiseCRefGroup* allStateGroups = new WwiseCRefGroup[StateGroups.Size()];
    int index = 0;
    for (auto const& StateGroup : StateGroups)
    {
        FillStateGroup(StateGroup.second, allStateGroups[index]);
        ++index;
    }
    return allStateGroups;
}

const unsigned int GetStateGroupCount()
{
    if (!IsInitialized)
    {
        return 0;
    }
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        return DataStructure.Platforms[CurrentPlatform].StateGroups.Size();
    }
    return 0;
}

void DeleteStateGroupRef(void* InStateGroupRefPtr)
{
    WwiseCRefGroup* arg1 = (WwiseCRefGroup*)InStateGroupRefPtr;
    {
        delete arg1;
    }
}

void DeleteStateGroupArrayRef(void* InStateGroupArrayRefPtr)
{
    WwiseCRefGroup* arg1 = (WwiseCRefGroup*)InStateGroupArrayRefPtr;
    {
        delete[] arg1;
    }
}

const WwiseCRefGroup* GetStateGroup(void* InStateGroupArrayRefPtr, int index)
{
    WwiseCRefGroup* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefGroup* cRef = (WwiseCRefGroup*)InStateGroupArrayRefPtr;
    
    if (cRef)
    {
        if ((unsigned int)index < GetStateGroupCount())
        {
            result = &cRef[index];
        }
    }
    return result;
}

const char* GetStateGroupName(void* InStateGroupRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefGroup* cRef = (WwiseCRefGroup*)InStateGroupRefPtr;
    if (cRef)
    {
        result = cRef->groupName;
    }
    return result;
}

const char* GetStateGroupPath(void* InStateGroupRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefGroup* cRef = (WwiseCRefGroup*)InStateGroupRefPtr;
    if (cRef)
    {
        result = cRef->groupPath;
    }
    return result;
}

const WwiseDBGuid* GetStateGroupGuid(void* InStateGroupRefPtr)
{
    const WwiseDBGuid* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefGroup* cRef = (WwiseCRefGroup*)InStateGroupRefPtr;
    if (cRef)
    {
        result = cRef->groupGuid;
    }
    return result;
}

const WwiseDBShortId GetStateGroupShortId(void* InStateGroupRefPtr)
{
    WwiseDBShortId result = -1;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefGroup* cRef = (WwiseCRefGroup*)InStateGroupRefPtr;
    if (cRef)
    {
        result = cRef->groupShortId;
    }
    return result;
}

const WwiseCRefGroup* GetStateGroupRefString(const char* InString)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    return GetStateGroupRef(WwiseDBObjectInfo(WwiseDBGuid(), 0, InString, ""));
}

const WwiseCRefState* GetAllStateRef()
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseStateGlobalIdsMap States;
    GetCurrentPlatformStates(States);
    WwiseCRefState* allStates = new WwiseCRefState[States.Size()];
    int index = 0;
    for (auto const& State : States)
    {
        FillState(State.second, allStates[index]);
        ++index;
    }
    return allStates;
}

const unsigned int GetStateCount()
{
    if (!IsInitialized)
    {
        return 0;
    }
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        return DataStructure.Platforms[CurrentPlatform].States.Size();
    }
    return 0;
}

void DeleteStateRef(void* InStateRefPtr)
{
    WwiseCRefState* arg1 = (WwiseCRefState*)InStateRefPtr;
    {
        delete arg1;
    }
}

void DeleteStateArrayRef(void* InStateArrayRefPtr)
{
    WwiseCRefState* arg1 = (WwiseCRefState*)InStateArrayRefPtr;
    {
        delete[] arg1;
    }
}

const WwiseCRefState* GetState(void* InStateArrayRefPtr, int index)
{
    WwiseCRefState* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefState* cRef = (WwiseCRefState*)InStateArrayRefPtr;
    
    if (cRef)
    {
        if ((unsigned int)index < GetStateCount())
        {
            result = &cRef[index];
        }
    }
    return result;
}

const char* GetStateName(void* InStateRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefState* cRef = (WwiseCRefState*)InStateRefPtr;
    if (cRef)
    {
        result = cRef->stateName;
    }
    return result;
}

const char* GetStatePath(void* InStateRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefState* cRef = (WwiseCRefState*)InStateRefPtr;
    if (cRef)
    {
        result = cRef->statePath;
    }
    return result;
}

const WwiseDBGuid* GetStateGuid(void* InStateRefPtr)
{
    const WwiseDBGuid* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefState* cRef = (WwiseCRefState*)InStateRefPtr;
    if (cRef)
    {
        result = cRef->stateGuid;
    }
    return result;
}

const WwiseDBShortId GetStateShortId(void* InStateRefPtr)
{
    WwiseDBShortId result = -1;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefState* cRef = (WwiseCRefState*)InStateRefPtr;
    if (cRef)
    {
        result = cRef->stateShortId;
    }
    return result;
}

const WwiseDBShortId GetStateGroupId(void* InStateRefPtr)
{
    WwiseDBShortId result = -1;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefState* cRef = (WwiseCRefState*)InStateRefPtr;
    if (cRef)
    {
        result = cRef->groupShortId;
    }
    return result;
}

const WwiseCRefState* GetStateRefString(const char* InString)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    return GetStateRef(WwiseDBObjectInfo(WwiseDBGuid(), 0, InString, ""));
}

const WwiseCRefAcousticTexture* GetAllAcousticTextureRef()
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseAcousticTextureGlobalIdsMap AcousticTextures;
    GetCurrentPlatformAcousticTextures(AcousticTextures);
    WwiseCRefAcousticTexture* allAcousticTextures = new WwiseCRefAcousticTexture[AcousticTextures.Size()];
    int index = 0;
    for (auto const& AcousticTexture : AcousticTextures)
    {
        FillAcousticTexture(AcousticTexture.second, allAcousticTextures[index]);
        ++index;
    }
    return allAcousticTextures;
}

const unsigned int GetAcousticTextureCount()
{
    if (!IsInitialized)
    {
        return 0;
    }
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        return DataStructure.Platforms[CurrentPlatform].AcousticTextures.Size();
    }
    return 0;
}

void DeleteAcousticTextureRef(void* InAcousticTextureRefPtr)
{
    WwiseCRefAcousticTexture* arg1 = (WwiseCRefAcousticTexture*)InAcousticTextureRefPtr;
    {
        delete arg1;
    }
}

void DeleteAcousticTextureArrayRef(void* InAcousticTextureArrayRefPtr)
{
    WwiseCRefAcousticTexture* arg1 = (WwiseCRefAcousticTexture*)InAcousticTextureArrayRefPtr;
    {
        delete[] arg1;
    }
}

const WwiseCRefAcousticTexture* GetAcousticTexture(void* InAcousticTextureArrayRefPtr, int index)
{
    WwiseCRefAcousticTexture* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefAcousticTexture* cRef = (WwiseCRefAcousticTexture*)InAcousticTextureArrayRefPtr;
    
    if (cRef)
    {
        if ((unsigned int)index < GetAcousticTextureCount())
        {
            result = &cRef[index];
        }
    }
    return result;
}

const char* GetAcousticTextureName(void* InAcousticTextureRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefAcousticTexture* cRef = (WwiseCRefAcousticTexture*)InAcousticTextureRefPtr;
    if (cRef)
    {
        result = cRef->acousticTextureName;
    }
    return result;
}

const char* GetAcousticTexturePath(void* InAcousticTextureRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefAcousticTexture* cRef = (WwiseCRefAcousticTexture*)InAcousticTextureRefPtr;
    if (cRef)
    {
        result = cRef->acousticTexturePath;
    }
    return result;
}

const WwiseDBGuid* GetAcousticTextureGuid(void* InAcousticTextureRefPtr)
{
    const WwiseDBGuid* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefAcousticTexture* cRef = (WwiseCRefAcousticTexture*)InAcousticTextureRefPtr;
    if (cRef)
    {
        result = cRef->acousticTextureGuid;
    }
    return result;
}

const WwiseDBShortId GetAcousticTextureShortId(void* InAcousticTextureRefPtr)
{
    WwiseDBShortId result = -1;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefAcousticTexture* cRef = (WwiseCRefAcousticTexture*)InAcousticTextureRefPtr;
    if (cRef)
    {
        result = cRef->acousticTextureShortId;
    }
    return result;
}

const WwiseCRefAcousticTexture* GetAcousticTextureRefString(const char* InString)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    return GetAcousticTextureRef(WwiseDBObjectInfo(WwiseDBGuid(), 0, InString, ""));
}

const WwiseCRefGameParameter* GetAllGameParameterRef()
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseGameParameterGlobalIdsMap GameParameters;
    GetCurrentPlatformGameParameters(GameParameters);
    WwiseCRefGameParameter* allGameParameteres = new WwiseCRefGameParameter[GameParameters.Size()];
    int index = 0;
    for (auto const& GameParameter : GameParameters)
    {
        FillGameParameter(GameParameter.second, allGameParameteres[index]);
        ++index;
    }
    return allGameParameteres;
}

const unsigned int GetGameParameterCount()
{
    if (!IsInitialized)
    {
        return 0;
    }
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        return DataStructure.Platforms[CurrentPlatform].GameParameters.Size();
    }
    return 0;
}

void DeleteGameParameterRef(void* InGameParameterRefPtr)
{
    WwiseCRefGameParameter* arg1 = (WwiseCRefGameParameter*)InGameParameterRefPtr;
    {
        delete arg1;
    }
}

void DeleteGameParameterArrayRef(void* InGameParameterArrayRefPtr)
{
    WwiseCRefGameParameter* arg1 = (WwiseCRefGameParameter*)InGameParameterArrayRefPtr;
    {
        delete[] arg1;
    }
}

const WwiseCRefGameParameter* GetGameParameter(void* InGameParameterArrayRefPtr, int index)
{
    WwiseCRefGameParameter* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefGameParameter* cRef = (WwiseCRefGameParameter*)InGameParameterArrayRefPtr;
    
    if (cRef)
    {
        if ((unsigned int)index < GetGameParameterCount())
        {
            result = &cRef[index];
        }
    }
    return result;
}

const char* GetGameParameterName(void* InGameParameterRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefGameParameter* cRef = (WwiseCRefGameParameter*)InGameParameterRefPtr;
    if (cRef)
    {
        result = cRef->gameParameterName;
    }
    return result;
}

const char* GetGameParameterPath(void* InGameParameterRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefGameParameter* cRef = (WwiseCRefGameParameter*)InGameParameterRefPtr;
    if (cRef)
    {
        result = cRef->gameParameterPath;
    }
    return result;
}

const WwiseDBGuid* GetGameParameterGuid(void* InGameParameterRefPtr)
{
    const WwiseDBGuid* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefGameParameter* cRef = (WwiseCRefGameParameter*)InGameParameterRefPtr;
    if (cRef)
    {
        result = cRef->gameParameterGuid;
    }
    return result;
}

const WwiseDBShortId GetGameParameterShortId(void* InGameParameterRefPtr)
{
    WwiseDBShortId result = -1;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefGameParameter* cRef = (WwiseCRefGameParameter*)InGameParameterRefPtr;
    if (cRef)
    {
        result = cRef->gameParameterShortId;
    }
    return result;
}

const WwiseCRefGameParameter* GetGameParameterRefString(const char* InString)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    return GetGameParameterRef(WwiseDBObjectInfo(WwiseDBGuid(), 0, InString, ""));
}


const WwiseCRefTrigger* GetAllTriggerRef()
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseTriggerGlobalIdsMap Triggers;
    GetCurrentPlatformTriggers(Triggers);
    WwiseCRefTrigger* allTriggers = new WwiseCRefTrigger[Triggers.Size()];
    int index = 0;
    for (auto const& Trigger : Triggers)
    {
        FillTrigger(Trigger.second, allTriggers[index]);
        ++index;
    }
    return allTriggers;
}

const unsigned int GetTriggerCount()
{
    if (!IsInitialized)
    {
        return 0;
    }
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        return DataStructure.Platforms[CurrentPlatform].Triggers.Size();
    }
    return 0;
}

void DeleteTriggerRef(void* InTriggerRefPtr)
{
    WwiseCRefTrigger* arg1 = (WwiseCRefTrigger*)InTriggerRefPtr;
    {
        delete arg1;
    }
}

void DeleteTriggerArrayRef(void* InTriggerArrayRefPtr)
{
    WwiseCRefTrigger* arg1 = (WwiseCRefTrigger*)InTriggerArrayRefPtr;
    {
        delete[] arg1;
    }
}

const WwiseCRefTrigger* GetTrigger(void* InTriggerArrayRefPtr, int index)
{
    WwiseCRefTrigger* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefTrigger* cRef = (WwiseCRefTrigger*)InTriggerArrayRefPtr;
    
    if (cRef)
    {
        if ((unsigned int)index < GetTriggerCount())
        {
            result = &cRef[index];
        }
    }
    return result;
}

const char* GetTriggerName(void* InTriggerRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefTrigger* cRef = (WwiseCRefTrigger*)InTriggerRefPtr;
    if (cRef)
    {
        result = cRef->triggerName;
    }
    return result;
}

const char* GetTriggerPath(void* InTriggerRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefTrigger* cRef = (WwiseCRefTrigger*)InTriggerRefPtr;
    if (cRef)
    {
        result = cRef->triggerPath;
    }
    return result;
}

const WwiseDBGuid* GetTriggerGuid(void* InTriggerRefPtr)
{
    const WwiseDBGuid* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefTrigger* cRef = (WwiseCRefTrigger*)InTriggerRefPtr;
    if (cRef)
    {
        result = cRef->triggerGuid;
    }
    return result;
}

const WwiseDBShortId GetTriggerShortId(void* InTriggerRefPtr)
{
    WwiseDBShortId result = -1;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefTrigger* cRef = (WwiseCRefTrigger*)InTriggerRefPtr;
    if (cRef)
    {
        result = cRef->triggerShortId;
    }
    return result;
}

const WwiseCRefTrigger* GetTriggerRefString(const char* InString)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    return GetTriggerRef(WwiseDBObjectInfo(WwiseDBGuid(), 0, InString, ""));
}


const WwiseCRefBus* GetAllBusRef()
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseBusGlobalIdsMap Buses;
    GetCurrentPlatformBuses(Buses);
    WwiseCRefBus* allBuses = new WwiseCRefBus[Buses.Size()];
    int index = 0;
    for (auto const& Bus : Buses)
    {
        FillBus(Bus.second, allBuses[index]);
        ++index;
    }
    return allBuses;
}

const unsigned int GetBusCount()
{
    if (!IsInitialized)
    {
        return 0;
    }
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        return DataStructure.Platforms[CurrentPlatform].Busses.Size();
    }
    return 0;
}

void DeleteBusRef(void* InBusRefPtr)
{
    WwiseCRefBus* arg1 = (WwiseCRefBus*)InBusRefPtr;
    {
        delete arg1;
    }
}

void DeleteBusArrayRef(void* InBusArrayRefPtr)
{
    WwiseCRefBus* arg1 = (WwiseCRefBus*)InBusArrayRefPtr;
    {
        delete[] arg1;
    }
}

const WwiseCRefBus* GetBus(void* InBusArrayRefPtr, int index)
{
    WwiseCRefBus* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefBus* cRef = (WwiseCRefBus*)InBusArrayRefPtr;
    
    if (cRef)
    {
        if ((unsigned int)index < GetBusCount())
        {
            result = &cRef[index];
        }
    }
    return result;
}

const char* GetBusName(void* InBusRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefBus* cRef = (WwiseCRefBus*)InBusRefPtr;
    if (cRef)
    {
        result = cRef->busName;
    }
    return result;
}

const char* GetBusPath(void* InBusRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefBus* cRef = (WwiseCRefBus*)InBusRefPtr;
    if (cRef)
    {
        result = cRef->busPath;
    }
    return result;
}

const WwiseDBGuid* GetBusGuid(void* InBusRefPtr)
{
    const WwiseDBGuid* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefBus* cRef = (WwiseCRefBus*)InBusRefPtr;
    if (cRef)
    {
        result = cRef->busGuid;
    }
    return result;
}

const WwiseDBShortId GetBusShortId(void* InBusRefPtr)
{
    WwiseDBShortId result = -1;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefBus* cRef = (WwiseCRefBus*)InBusRefPtr;
    if (cRef)
    {
        result = cRef->busShortId;
    }
    return result;
}

const WwiseCRefBus* GetBusRefString(const char* InString)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    return GetBusRef(WwiseDBObjectInfo(WwiseDBGuid(), 0, InString, ""));
}


const WwiseCRefAuxBus* GetAllAuxBusRef()
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    WwiseAuxBusGlobalIdsMap AuxBuses;
    GetCurrentPlatformAuxBuses(AuxBuses);
    WwiseCRefAuxBus* allAuxBuses = new WwiseCRefAuxBus[AuxBuses.Size()];
    int index = 0;
    for (auto const& AuxBus : AuxBuses)
    {
        FillAuxBus(AuxBus.second, allAuxBuses[index]);
        ++index;
    }
    return allAuxBuses;
}

const unsigned int GetAuxBusCount()
{
    if (!IsInitialized)
    {
        return 0;
    }
    if (DataStructure.Platforms.Contains(CurrentPlatform))
    {
        return DataStructure.Platforms[CurrentPlatform].AuxBusses.Size();
    }
    return 0;
}

void DeleteAuxBusRef(void* InAuxBusRefPtr)
{
    WwiseCRefAuxBus* arg1 = (WwiseCRefAuxBus*)InAuxBusRefPtr;
    {
        delete arg1;
    }
}

void DeleteAuxBusArrayRef(void* InAuxBusArrayRefPtr)
{
    WwiseCRefAuxBus* arg1 = (WwiseCRefAuxBus*)InAuxBusArrayRefPtr;
    {
        delete[] arg1;
    }
}

const WwiseCRefAuxBus* GetAuxBus(void* InAuxBusArrayRefPtr, int index)
{
    WwiseCRefAuxBus* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefAuxBus* cRef = (WwiseCRefAuxBus*)InAuxBusArrayRefPtr;
    
    if (cRef)
    {
        if ((unsigned int)index < GetAuxBusCount())
        {
            result = &cRef[index];
        }
    }
    return result;
}

const char* GetAuxBusName(void* InAuxBusRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefAuxBus* cRef = (WwiseCRefAuxBus*)InAuxBusRefPtr;
    if (cRef)
    {
        result = cRef->auxBusName;
    }
    return result;
}

const char* GetAuxBusPath(void* InAuxBusRefPtr)
{
    const char* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefAuxBus* cRef = (WwiseCRefAuxBus*)InAuxBusRefPtr;
    if (cRef)
    {
        result = cRef->auxBusPath;
    }
    return result;
}

const WwiseDBGuid* GetAuxBusGuid(void* InAuxBusRefPtr)
{
    const WwiseDBGuid* result = nullptr;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefAuxBus* cRef = (WwiseCRefAuxBus*)InAuxBusRefPtr;
    if (cRef)
    {
        result = cRef->auxBusGuid;
    }
    return result;
}

const WwiseDBShortId GetAuxBusShortId(void* InAuxBusRefPtr)
{
    WwiseDBShortId result = -1;
    if (!IsInitialized)
    {
        return result;
    }
    WwiseCRefAuxBus* cRef = (WwiseCRefAuxBus*)InAuxBusRefPtr;
    if (cRef)
    {
        result = cRef->auxBusShortId;
    }
    return result;
}

const WwiseCRefAuxBus* GetAuxBusRefString(const char* InString)
{
    if (!IsInitialized)
    {
        return nullptr;
    }
    return GetAuxBusRef(WwiseDBObjectInfo(WwiseDBGuid(), 0, InString, ""));
}
