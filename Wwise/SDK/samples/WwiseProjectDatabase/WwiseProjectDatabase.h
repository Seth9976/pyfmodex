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

#include "WwiseProjectDatabase/AdapterTypes/WwiseWrapperTypes.h"
#include "stdafx.h"
#include "WwiseProjectDatabase/C-Ref/WwiseCRefSoundBank.h"
#include "WwiseProjectDatabase/C-Ref/WwiseCRefPlugin.h"
#include "WwiseProjectDatabase/Ref/WwiseRefCollections.h"

extern "C"
{
    /*
    * Exposed Utilities  
    */

    /// Initialize the WwiseProjectDatabase of a given Wwise Project for a given platform.
    /// This parses and populates the structures.
    /// @param InDirectoryPath The Path to the Root Output Path of the Wwise Project
    /// @param InDirectoryPlatformName The Platform the WwiseProjectDatabase will parse
    WWISE_PROJECT_DATABASE_API void Init(const char* InDirectoryPath, const char* InDirectoryPlatformName);

    /// Set the Platform to parse.
    /// @param InDirectoryPlatformName Platform to parse
    WWISE_PROJECT_DATABASE_API void SetCurrentPlatform(const char* InDirectoryPlatformName);

    /// Set the Language to parse
    /// @param InLanguageName Current Language to parse
    WWISE_PROJECT_DATABASE_API void SetCurrentLanguage(const char* InLanguageName);

    /// Get 4 values forming the Guid
    /// @param InGuid Guid
    /// @param A First value of the Guid
    /// @param B Second value of the Guid
    /// @param C Third value of the Guid
    /// @param D Fourth value of the Guid
    WWISE_PROJECT_DATABASE_API void GetGuidInt(void* InGuid, unsigned int& A, unsigned int& B, unsigned int& C, unsigned int& D);

    /*
        SoundBank
    */

    /// Get a reference to the start of a list containing all the SoundBanks.
    /// @return The reference to the start of a list containing all the SoundBanks
    WWISE_PROJECT_DATABASE_API const WwiseCRefSoundBank* GetAllSoundBanksRef();

    /// Get the number of SoundBanks in the Project Database.
    /// @return The number of SoundBanks in the Project Database
    WWISE_PROJECT_DATABASE_API const unsigned int GetSoundBankCount();

    /// Get a reference to a SoundBank given its index in the list of all the SoundBanks.
    /// @param InSoundBanksArrayRefPtr The reference to the start of a list containing all the SoundBanks
    /// @param index The index of the SoundBank to retrieve
    /// @return The reference of the SoundBank
    WWISE_PROJECT_DATABASE_API const WwiseCRefSoundBank* GetSoundBankRefIndex(void* InSoundBanksArrayRefPtr, int index);

    /// Get a reference to a SoundBank given its name
    /// @param InString The SoundBank Name
    /// @return The reference of the SoundBank
    WWISE_PROJECT_DATABASE_API const WwiseCRefSoundBank* GetSoundBankRefString(const char* InName, const char* InType);

    /// Get the name of a SoundBank
    /// @param InSoundBankRefPtr The SoundBank reference
    /// @return The Name of the SoundBank
    WWISE_PROJECT_DATABASE_API const char* GetSoundBankName(void* InSoundBankRefPtr);

    /// Get the path of a SoundBank
    /// @param InSoundBankRefPtr The SoundBank reference
    /// @return The Path of the SoundBank
    WWISE_PROJECT_DATABASE_API const char* GetSoundBankPath(void* InSoundBankRefPtr);

    /// Get the object path of a SoundBank
    /// @param InSoundBankRefPtr The SoundBank reference
    /// @return The Object Path of the SoundBank
    WWISE_PROJECT_DATABASE_API const char* GetSoundBankObjectPath(void* InSoundBankRefPtr);

    /// Get the Language of a SoundBank as a string
    /// @param InSoundBankRefPtr The SoundBank reference
    /// @return The Language of the SoundBank
    WWISE_PROJECT_DATABASE_API const char* GetSoundBankLanguage(void* InSoundBankRefPtr);

    /// Get the Language ID of a SoundBank
    /// @param InSoundBankRefPtr The SoundBank reference
    /// @return The Language ID of the SoundBank
    WWISE_PROJECT_DATABASE_API const WwiseDBShortId GetSoundBankLanguageId(void* InSoundBankRefPtr);

    /// Get the GUID of a SoundBank
    /// @param InSoundBankRefPtr The SoundBank reference
    /// @return A reference to the GUID of the SoundBank
    WWISE_PROJECT_DATABASE_API const WwiseDBGuid* GetSoundBankGuid(void* InSoundBankRefPtr);

    /// Get the ShortID of a SoundBank
    /// @param InSoundBankRefPtr The SoundBank reference
    /// @return The ShortID of the SoundBank
    WWISE_PROJECT_DATABASE_API const WwiseDBShortId GetSoundBankShortId(void* InSoundBankRefPtr);

    /// Get if a SoundBank is a User Defined SoundBank
    /// @param InSoundBankRefPtr The SoundBank reference
    /// @return Whether the SoundBank is a User Defined SoundBank or not
    WWISE_PROJECT_DATABASE_API const bool IsUserBank(void* InSoundBankRefPtr);

    /// Get if a SoundBank is an Init Bank
    /// @param InSoundBankRefPtr The SoundBank reference
    /// @return Whether the SoundBank is an Init SoundBank or not
    WWISE_PROJECT_DATABASE_API const bool IsInitBank(void* InSoundBankRefPtr);

    /// Get if a SoundBank Reference is a valid reference
    /// A valid reference is a reference with information properly filled with the metadata.
    /// @param InSoundBankRefPtr The SoundBank reference
    /// @return Whether the SoundBank reference is valid not
    WWISE_PROJECT_DATABASE_API const bool IsSoundBankValid(void* InSoundBankRefPtr);

    /// Get a Media in the SoundBank at a given index
    /// For the index to be valid, it must be greater or equal to 0 and less than GetSoundBankMediasCount.
    /// @param InSoundBankRefPtr The SoundBank reference
    /// @param index The Index of the Media
    /// @return The Reference of the Media or nullptr if the index is invalid
    WWISE_PROJECT_DATABASE_API const WwiseCRefMedia* GetSoundBankMedia(void* InSoundBankRefPtr, int index);

    /// Get the number of Medias in a SoundBank
    /// @param InSoundBankRefPtr The SoundBank reference
    /// @return The Number of Medias in the SoundBank
    WWISE_PROJECT_DATABASE_API const unsigned int GetSoundBankMediasCount(void* InSoundBankRefPtr);

    /// Get an Event in the SoundBank at a given index
    /// For the index to be valid, it must be greater or equal to 0 and less than GetSoundBankEventsCount.
    /// @param InSoundBankRefPtr The SoundBank reference
    /// @param index The Index of the Event
    /// @return The Reference of the Event or nullptr if the index is invalid
    WWISE_PROJECT_DATABASE_API const WwiseCRefEvent* GetSoundBankEvent(void* InSoundBankRefPtr, int index);

    /// Get the number of Events in a SoundBank
    /// @param InSoundBankRefPtr The SoundBank reference
    /// @return The Number of Events in the SoundBank
    WWISE_PROJECT_DATABASE_API const unsigned int GetSoundBankEventsCount(void* InSoundBankRefPtr);

    /// Delete a SoundBank Reference. This should be called upon exiting the scope of the reference.
    /// @param InSoundBankRefPtr The SoundBank Reference
    WWISE_PROJECT_DATABASE_API void DeleteSoundBankRef(void* InSoundBankRefPtr);

    /// Delete an array containing all the SoundBanks. This should be called upon exiting the scope of the array.
    /// @param InSoundBankRefPtr The reference to an array containing all the SoundBanks
    WWISE_PROJECT_DATABASE_API void DeleteSoundBanksArrayRef(void* InSoundBanksArrayRefPtr);

    /*
        Media
    */

    /// Get the Name of a Media
    /// @param InMediaRefPtr The Media Reference
    /// @return The Name of the Media
    WWISE_PROJECT_DATABASE_API const char* GetMediaName(void* InMediaRefPtr);

    /// Get the Object Path of a Media
    /// @param InMediaRefPtr The Media Reference
    /// @return The Object Path of the Media
    WWISE_PROJECT_DATABASE_API const char* GetMediaPath(void* InMediaRefPtr);

    /// Get the ShortID of a Media
    /// @param InMediaRefPtr The Media Reference
    /// @return The ShortID of the Media
    WWISE_PROJECT_DATABASE_API const WwiseDBShortId GetMediaShortId(void* InMediaRefPtr);

    /// Get the Language of a Media
    /// @param InMediaRefPtr The Media Reference
    /// @return The Language of the Media
    WWISE_PROJECT_DATABASE_API const char* GetMediaLanguage(void* InMediaRefPtr);

    /// Get if a Media is Streaming
    /// @param InMediaRefPtr The Media Reference
    /// @return Whether the Media is Streaming or not
    WWISE_PROJECT_DATABASE_API const bool GetMediaIsStreaming(void* InMediaRefPtr);

    /// Get the Media Location
    /// @param InMediaRefPtr The Media Reference
    /// @return The Location ID of the Media (	Memory (0), Loose (1), OtherBank (2) or Unknown (-1) )
    WWISE_PROJECT_DATABASE_API const WwiseDBShortId GetMediaLocation(void* InMediaRefPtr);

    /// Get the Media Cache Path
    /// @param InMediaRefPtr The Media Reference
    /// @return The Cache Path of the SoundBank
    WWISE_PROJECT_DATABASE_API const char* GetMediaCachePath(void* InMediaRefPtr);

    /// Delete a Media Reference. This should be called upon exiting the scope of the reference.
    /// @param InMediaRefPtr The Media Reference
    WWISE_PROJECT_DATABASE_API void DeleteMediaRef(void* InMediaRefPtr);

    /*
        Event
    */

    /// @return The reference to the start of a list containing all the Events
    WWISE_PROJECT_DATABASE_API const WwiseCRefEvent* GetAllEventsRef();

    /// Get a reference to the start of a list containing all the Events.
    /// @return The reference to the start of a list containing all the Events
    WWISE_PROJECT_DATABASE_API  unsigned int GetEventCount();

    /// Get the reference of an Event given its name
    /// @param InString The Name of the Event
    /// @return The Event reference
    WWISE_PROJECT_DATABASE_API const WwiseCRefEvent* GetEventRefString(const char* InString);

    /// Get the name of an Event
    /// @param InEventRefPtr The Event reference
    /// @return The Name of the Event
    WWISE_PROJECT_DATABASE_API const char* GetEventName(void* InEventRefPtr);

    /// Get the Object Path of an Event
    /// @param InEventRefPtr The Event reference
    /// @return The Object Path of the Event
    WWISE_PROJECT_DATABASE_API const char* GetEventPath(void* InEventRefPtr);

    /// Get the GUID of an Event
    /// @param InEventRefPtr The Event reference
    /// @return A reference to the GUID of the Event
    WWISE_PROJECT_DATABASE_API const WwiseDBGuid* GetEventGuid(void* InEventRefPtr);

    /// Get the ShortID of an Event
    /// @param InEventRefPtr The Event reference
    /// @return The ShortID of the Event
    WWISE_PROJECT_DATABASE_API const WwiseDBShortId GetEventShortId(void* InEventRefPtr);

    /// Get the Max Attenuation of an Event
    /// @param InEventRefPtr The Event reference
    /// @return The Max Attenuation of the Event
    WWISE_PROJECT_DATABASE_API const float GetEventMaxAttenuation(void* InEventRefPtr);

    /// Get the Minimum Duration of an Event
    /// @param InEventRefPtr The Event reference
    /// @return The Minimum Duration of the Event
    WWISE_PROJECT_DATABASE_API const float GetEventMinDuration(void* InEventRefPtr);

    /// Get the Maximum Duration of an Event
    /// @param InEventRefPtr The Event reference
    /// @return The Maximum Duration of the Event
    WWISE_PROJECT_DATABASE_API const float GetEventMaxDuration(void* InEventRefPtr);

    /// Get the Media of an Event at a given index
    /// For the index to be valid, it must be greater or equal to 0 and less than GetEventMediasCount().
    /// @param InEventRefPtr The Event reference
    /// @param index The Index of the Media
    /// @return The Media reference of nullptr if the index is invalid
    WWISE_PROJECT_DATABASE_API const WwiseCRefMedia* GetEventMedia(void* InEventRefPtr, int index);

    /// Get the number of Medias in an Event
    /// @param InEventRefPtr The Event reference
    /// @return The Number of Medias in the Event
    WWISE_PROJECT_DATABASE_API const unsigned int GetEventMediasCount(void* InEventRefPtr);

    /// Delete an Event reference. This should be called upon exiting the scope of the reference.
    /// @param InEventRefPtr The Event reference
    WWISE_PROJECT_DATABASE_API void DeleteEventRef(void* InEventRefPtr);

    /// Delete an array containing all the Events. This should be called upon exiting the scope of the array.
    /// @param InEventArrayRefPtr The reference to an array containing all the Events
    WWISE_PROJECT_DATABASE_API void DeleteEventArrayRef(void* InEventArrayRefPtr);

    /// Get an Event in the Event Array at a given index
    /// For the index to be valid, it must be greater or equal to 0 and less than GetEventsCount.
    /// @param InEventArrayRefPtr The Event Array reference
    /// @param index The Index of the Event
    /// @return The Reference of the Event or nullptr if the index is invalid
    WWISE_PROJECT_DATABASE_API const WwiseCRefEvent* GetEvent(void* InEventArrayRefPtr, int index);

    /*
     * Platform
    */

    /// Get a reference to a platform given its name
    /// @param InPlatformName The name of the platform
    /// @return The reference of the platform
    WWISE_PROJECT_DATABASE_API const WwiseCRefPlatform* GetPlatformRef(const char* InPlatformName);

    /// Get the name of a Platform
    /// @param InPlatformRefPtr The Platform reference
    /// @return The Name of the Platform
    WWISE_PROJECT_DATABASE_API const char* GetPlatformName(void* InPlatformRefPtr);

    /// Get the GUID of a Platform
    /// @param InPlatformRefPtr The Platform reference
    /// @return A reference to the GUID of the Platform
    WWISE_PROJECT_DATABASE_API const WwiseDBGuid* GetPlatformGuid(void* InPlatformRefPtr);

    /// Delete a Platform reference. This should be called upon exiting the scope of the reference.
    /// @param InPlatformRefPtr The Platform reference
    WWISE_PROJECT_DATABASE_API void DeletePlatformRef(void* InPlatformRefPtr);

    /*
     * Plugin
    */

    /// Get a reference to the start of a list containing all the Plugins.
    /// @return The reference to the start of a list containing all the Plugins
    WWISE_PROJECT_DATABASE_API const WwiseCRefPlugin* GetAllPluginRef();

    /// Get the number of Plugins in the Project Database.
    /// @return The number of Plugins in the Project Database
    WWISE_PROJECT_DATABASE_API const unsigned int GetPluginCount();

    /// Get a reference to a Plugin given its index in the list of all the Plugins.
    /// @param InPluginRefPtr The reference to the start of a list containing all the Plugins
    /// @param index The index of the Plugin to retrieve
    /// @return The reference of the Plugin
    WWISE_PROJECT_DATABASE_API const WwiseCRefPlugin* GetPluginRefIndex(void* InPluginArrayRefPtr, int index);

    /// Get the id of a Plugin
    /// @param InPluginRefPtr The Plugin reference
    /// @return The Id of the Plugin
    WWISE_PROJECT_DATABASE_API const WwiseDBShortId GetPluginId(void* InPluginRefPtr);

    /// Get the name of a Plugin
    /// @param InPluginRefPtr The Plugin reference
    /// @return The Name of the Plugin
    WWISE_PROJECT_DATABASE_API const char* GetPluginName(void* InPluginRefPtr);

    /// Get the dll name of a Plugin
    /// @param InPluginRefPtr The Plugin reference
    /// @return The DLL name of the Plugin
    WWISE_PROJECT_DATABASE_API const char* GetPluginDLL(void* InPluginRefPtr);

    /// Get the static lib name of a Plugin
    /// @param InPluginRefPtr The Plugin reference
    /// @return The StaticLib of the Plugin
    WWISE_PROJECT_DATABASE_API const char* GetPluginStaticLib(void* InPluginRefPtr);

    /// Delete a Plugin Reference. This should be called upon exiting the scope of the reference.
    /// @param InPluginRefPtr The Plugin Reference
    WWISE_PROJECT_DATABASE_API void DeletePluginRef(void* InPluginRefPtr);

    /// Delete an array containing all the Plugins. This should be called upon exiting the scope of the array.
    /// @param InPluginRefPtr The reference to an array containing all the Plugins
    WWISE_PROJECT_DATABASE_API void DeletePluginArrayRef(void* InPluginArrayRefPtr);

    /*
     * Switch Group
    */

    /// Get a reference to the start of a list containing all the Switch Groups.
    /// @return The reference to the start of a list containing all the Switch Groups
    WWISE_PROJECT_DATABASE_API const WwiseCRefGroup* GetAllSwitchGroupRef();

    /// Get the number of Switch Groups in the Project Database.
    /// @return The number of Switch Groups in the Project Database
    WWISE_PROJECT_DATABASE_API const unsigned int GetSwitchGroupCount();

    /// Delete a Switch Group reference. This should be called upon exiting the scope of the reference.
    /// @param InSwitchGroupRefPtr The Switch reference
    WWISE_PROJECT_DATABASE_API void DeleteSwitchGroupRef(void* InSwitchGroupRefPtr);

    /// Delete an array containing all the Switch Groups. This should be called upon exiting the scope of the array.
    /// @param InSwitchGroupArrayRefPtr The reference to an array containing all the Switch Groups
    WWISE_PROJECT_DATABASE_API void DeleteSwitchGroupArrayRef(void* InSwitchGroupArrayRefPtr);

    /// Get a Switch Group in the Switch Group Array at a given index
    /// For the index to be valid, it must be greater or equal to 0 and less than GetSwitchesCount.
    /// @param InSwitchGroupArrayRefPtr The Switch Group Array reference
    /// @param index The Index of the Switch Group
    /// @return The Reference of the Switch Group or nullptr if the index is invalid
    WWISE_PROJECT_DATABASE_API const WwiseCRefGroup* GetSwitchGroup(void* InSwitchGroupArrayRefPtr, int index);

    /// Get the name of a Switch Group
    /// @param InSwitchGroupRefPtr The Switch Group reference
    /// @return The Name of the Switch Group
    WWISE_PROJECT_DATABASE_API const char* GetSwitchGroupName(void* InSwitchGroupRefPtr);

    /// Get the Object Path of a Switch Group
    /// @param InSwitchGroupRefPtr The Switch Group reference
    /// @return The Object Path of the Switch Group
    WWISE_PROJECT_DATABASE_API const char* GetSwitchGroupPath(void* InSwitchGroupRefPtr);

    /// Get the GUID of a Switch Group
    /// @param InSwitchGroupRefPtr The Switch Group reference
    /// @return A reference to the GUID of the Switch Group
    WWISE_PROJECT_DATABASE_API const WwiseDBGuid* GetSwitchGroupGuid(void* InSwitchGroupRefPtr);

    /// Get the ShortID of a Switch Group
    /// @param InSwitchGroupRefPtr The Switch Group reference
    /// @return The ShortID of the Switch Group
    WWISE_PROJECT_DATABASE_API const WwiseDBShortId GetSwitchGroupShortId(void* InSwitchGroupRefPtr);

    /// Get the reference of a Switch Group given its name
    /// @param InString The Name of the Event
    /// @return The Switch Group reference
    WWISE_PROJECT_DATABASE_API const WwiseCRefGroup* GetSwitchGroupRefString(const char* InString);

    /*
     * Switch
    */

    /// Get a reference to the start of a list containing all the Switches.
    /// @return The reference to the start of a list containing all the Switches
    WWISE_PROJECT_DATABASE_API const WwiseCRefSwitch* GetAllSwitchRef();

    /// Get the number of Switches in the Project Database.
    /// @return The number of Switches in the Project Database
    WWISE_PROJECT_DATABASE_API const unsigned int GetSwitchCount();

    /// Delete a Switch reference. This should be called upon exiting the scope of the reference.
    /// @param InSwitchRefPtr The Switch reference
    WWISE_PROJECT_DATABASE_API void DeleteSwitchRef(void* InSwitchRefPtr);

    /// Delete an array containing all the Switches. This should be called upon exiting the scope of the array.
    /// @param InSwitchArrayRefPtr The reference to an array containing all the Switches
    WWISE_PROJECT_DATABASE_API void DeleteSwitchArrayRef(void* InSwitchArrayRefPtr);

    /// Get a Switch in the Switch Array at a given index
    /// For the index to be valid, it must be greater or equal to 0 and less than GetSwitchesCount.
    /// @param InSwitchArrayRefPtr The Switch Array reference
    /// @param index The Index of the Switch
    /// @return The Reference of the Switch or nullptr if the index is invalid
    WWISE_PROJECT_DATABASE_API const WwiseCRefSwitch* GetSwitch(void* InSwitchArrayRefPtr, int index);

    /// Get the name of a Switch
    /// @param InSwitchRefPtr The Switch reference
    /// @return The Name of the Switch
    WWISE_PROJECT_DATABASE_API const char* GetSwitchName(void* InSwitchRefPtr);

    /// Get the Object Path of a Switch
    /// @param InSwitchRefPtr The Switch reference
    /// @return The Object Path of the Switch
    WWISE_PROJECT_DATABASE_API const char* GetSwitchPath(void* InSwitchRefPtr);

    /// Get the GUID of a Switch
    /// @param InSwitchRefPtr The Switch reference
    /// @return A reference to the GUID of the Switch
    WWISE_PROJECT_DATABASE_API const WwiseDBGuid* GetSwitchGuid(void* InSwitchRefPtr);

    /// Get the ShortID of a Switch
    /// @param InSwitchRefPtr The Switch reference
    /// @return The ShortID of the Switch
    WWISE_PROJECT_DATABASE_API const WwiseDBShortId GetSwitchShortId(void* InSwitchRefPtr);

    /// Get the ShortID of a Switch
    /// @param InSwitchRefPtr The Switch reference
    /// @return The ShortID of the Switch
    WWISE_PROJECT_DATABASE_API const WwiseDBShortId GetSwitchGroupId(void* InSwitchRefPtr);

    /// Get the reference of a Switch given its name
    /// @param InString The Name of the Event
    /// @return The Switch reference
    WWISE_PROJECT_DATABASE_API const WwiseCRefSwitch* GetSwitchRefString(const char* InString);

    /*
     * State Group
    */

    /// Get a reference to the start of a list containing all the State Groups.
    /// @return The reference to the start of a list containing all the State Groups
    WWISE_PROJECT_DATABASE_API const WwiseCRefGroup* GetAllStateGroupRef();

    /// Get the number of State Groups in the Project Database.
    /// @return The number of State Groups in the Project Database
    WWISE_PROJECT_DATABASE_API const unsigned int GetStateGroupCount();

    /// Delete a State reference. This should be called upon exiting the scope of the reference.
    /// @param InStateGroupRefPtr The State reference
    WWISE_PROJECT_DATABASE_API void DeleteStateGroupRef(void* InStateGroupRefPtr);

    /// Delete an array containing all the State Groups. This should be called upon exiting the scope of the array.
    /// @param InStateGroupArrayRefPtr The reference to an array containing all the States
    WWISE_PROJECT_DATABASE_API void DeleteStateGroupArrayRef(void* InStateGroupArrayRefPtr);

    /// Get a State in the State Array at a given index
    /// For the index to be valid, it must be greater or equal to 0 and less than GetStatesCount.
    /// @param InStateGroupArrayRefPtr The State Group Array reference
    /// @param index The Index of the State
    /// @return The Reference of the State or nullptr if the index is invalid
    WWISE_PROJECT_DATABASE_API const WwiseCRefGroup* GetStateGroup(void* InStateGroupArrayRefPtr, int index);

    /// Get the name of a State Group
    /// @param InStateGroupRefPtr The State reference
    /// @return The Name of the State
    WWISE_PROJECT_DATABASE_API const char* GetStateGroupName(void* InStateGroupRefPtr);

    /// Get the Object Path of a State Group
    /// @param InStateGroupRefPtr The State reference
    /// @return The Object Path of the State
    WWISE_PROJECT_DATABASE_API const char* GetStateGroupPath(void* InStateGroupRefPtr);

    /// Get the GUID of a State Group
    /// @param InStateGroupRefPtr The State reference
    /// @return A reference to the GUID of the State
    WWISE_PROJECT_DATABASE_API const WwiseDBGuid* GetStateGroupGuid(void* InStateGroupRefPtr);

    /// Get the ShortID of a State Group
    /// @param InStateGroupRefPtr The State reference
    /// @return The ShortID of the State
    WWISE_PROJECT_DATABASE_API const WwiseDBShortId GetStateGroupShortId(void* InStateGroupRefPtr);

    /// Get the reference of a State Group given its name
    /// @param InString The Name of the Event
    /// @return The State Group reference
    WWISE_PROJECT_DATABASE_API const WwiseCRefGroup* GetStateGroupRefString(const char* InString);

    /*
     * State
    */

    /// Get a reference to the start of a list containing all the States.
    /// @return The reference to the start of a list containing all the States
    WWISE_PROJECT_DATABASE_API const WwiseCRefState* GetAllStateRef();

    /// Get the number of States in the Project Database.
    /// @return The number of States in the Project Database
    WWISE_PROJECT_DATABASE_API const unsigned int GetStateCount();

    /// Delete a State reference. This should be called upon exiting the scope of the reference.
    /// @param InStateRefPtr The State reference
    WWISE_PROJECT_DATABASE_API void DeleteStateRef(void* InStateRefPtr);

    /// Delete an array containing all the Statees. This should be called upon exiting the scope of the array.
    /// @param InStateArrayRefPtr The reference to an array containing all the States
    WWISE_PROJECT_DATABASE_API void DeleteStateArrayRef(void* InStateArrayRefPtr);

    /// Get a State in the State Array at a given index
    /// For the index to be valid, it must be greater or equal to 0 and less than GetStatesCount.
    /// @param InStateArrayRefPtr The State Array reference
    /// @param index The Index of the State
    /// @return The Reference of the State or nullptr if the index is invalid
    WWISE_PROJECT_DATABASE_API const WwiseCRefState* GetState(void* InStateArrayRefPtr, int index);

    /// Get the name of a State
    /// @param InStateRefPtr The State reference
    /// @return The Name of the State
    WWISE_PROJECT_DATABASE_API const char* GetStateName(void* InStateRefPtr);

    /// Get the Object Path of a State
    /// @param InStateRefPtr The State reference
    /// @return The Object Path of the State
    WWISE_PROJECT_DATABASE_API const char* GetStatePath(void* InStateRefPtr);

    /// Get the GUID of a State
    /// @param InStateRefPtr The State reference
    /// @return A reference to the GUID of the State
    WWISE_PROJECT_DATABASE_API const WwiseDBGuid* GetStateGuid(void* InStateRefPtr);

    /// Get the ShortID of a State
    /// @param InStateRefPtr The State reference
    /// @return The ShortID of the State
    WWISE_PROJECT_DATABASE_API const WwiseDBShortId GetStateShortId(void* InStateRefPtr);

    /// Get the ShortID of a State
    /// @param InStateRefPtr The State reference
    /// @return The ShortID of the State
    WWISE_PROJECT_DATABASE_API const WwiseDBShortId GetStateGroupId(void* InStateRefPtr);

    /// Get the reference of a State given its name
    /// @param InString The Name of the Event
    /// @return The State reference
    WWISE_PROJECT_DATABASE_API const WwiseCRefState* GetStateRefString(const char* InString);

    /*
     * AcousticTexture
    */

    /// Get a reference to the start of a list containing all the AcousticTextures.
    /// @return The reference to the start of a list containing all the AcousticTextures
    WWISE_PROJECT_DATABASE_API const WwiseCRefAcousticTexture* GetAllAcousticTextureRef();

    /// Get the number of AcousticTextures in the Project Database.
    /// @return The number of AcousticTextures in the Project Database
    WWISE_PROJECT_DATABASE_API const unsigned int GetAcousticTextureCount();

    /// Delete a AcousticTexture reference. This should be called upon exiting the scope of the reference.
    /// @param InAcousticTextureRefPtr The AcousticTexture reference
    WWISE_PROJECT_DATABASE_API void DeleteAcousticTextureRef(void* InAcousticTextureRefPtr);

    /// Delete an array containing all the AcousticTextures. This should be called upon exiting the scope of the array.
    /// @param InAcousticTextureArrayRefPtr The reference to an array containing all the AcousticTextures
    WWISE_PROJECT_DATABASE_API void DeleteAcousticTextureArrayRef(void* InAcousticTextureArrayRefPtr);

    /// Get a AcousticTexture in the AcousticTexture Array at a given index
    /// For the index to be valid, it must be greater or equal to 0 and less than GetAcousticTexturesCount.
    /// @param InAcousticTextureArrayRefPtr The AcousticTexture Array reference
    /// @param index The Index of the AcousticTexture
    /// @return The Reference of the AcousticTexture or nullptr if the index is invalid
    WWISE_PROJECT_DATABASE_API const WwiseCRefAcousticTexture* GetAcousticTexture(void* InAcousticTextureArrayRefPtr, int index);

    /// Get the name of an AcousticTexture
    /// @param InAcousticTextureRefPtr The AcousticTexture reference
    /// @return The Name of the AcousticTexture
    WWISE_PROJECT_DATABASE_API const char* GetAcousticTextureName(void* InAcousticTextureRefPtr);

    /// Get the Object Path of an AcousticTexture
    /// @param InAcousticTextureRefPtr The AcousticTexture reference
    /// @return The Object Path of the AcousticTexture
    WWISE_PROJECT_DATABASE_API const char* GetAcousticTexturePath(void* InAcousticTextureRefPtr);

    /// Get the GUID of an AcousticTexture
    /// @param InAcousticTextureRefPtr The AcousticTexture reference
    /// @return A reference to the GUID of the AcousticTexture
    WWISE_PROJECT_DATABASE_API const WwiseDBGuid* GetAcousticTextureGuid(void* InAcousticTextureRefPtr);

    /// Get the ShortID of an AcousticTexture
    /// @param InAcousticTextureRefPtr The AcousticTexture reference
    /// @return The ShortID of the AcousticTexture
    WWISE_PROJECT_DATABASE_API const WwiseDBShortId GetAcousticTextureShortId(void* InAcousticTextureRefPtr);

    /// Get the reference of a AcousticTexture given its name
    /// @param InString The Name of the Event
    /// @return The AcousticTexture reference
    WWISE_PROJECT_DATABASE_API const WwiseCRefAcousticTexture* GetAcousticTextureRefString(const char* InString);

    /*
    * GameParameter
    */

    /// Get a reference to the start of a list containing all the GameParameters.
    /// @return The reference to the start of a list containing all the GameParameters
    WWISE_PROJECT_DATABASE_API const WwiseCRefGameParameter* GetAllGameParameterRef();

    /// Get the number of GameParameters in the Project Database.
    /// @return The number of GameParameters in the Project Database
    WWISE_PROJECT_DATABASE_API const unsigned int GetGameParameterCount();

    /// Delete a GameParameter reference. This should be called upon exiting the scope of the reference.
    /// @param InGameParameterRefPtr The GameParameter reference
    WWISE_PROJECT_DATABASE_API void DeleteGameParameterRef(void* InGameParameterRefPtr);

    /// Delete an array containing all the GameParameters. This should be called upon exiting the scope of the array.
    /// @param InGameParameterArrayRefPtr The reference to an array containing all the GameParameters
    WWISE_PROJECT_DATABASE_API void DeleteGameParameterArrayRef(void* InGameParameterArrayRefPtr);

    /// Get a GameParameter in the GameParameter Array at a given index
    /// For the index to be valid, it must be greater or equal to 0 and less than GetGameParametersCount.
    /// @param InGameParameterArrayRefPtr The GameParameter Array reference
    /// @param index The Index of the GameParameter
    /// @return The Reference of the GameParameter or nullptr if the index is invalid
    WWISE_PROJECT_DATABASE_API const WwiseCRefGameParameter* GetGameParameter(void* InGameParameterArrayRefPtr, int index);

    /// Get the name of an GameParameter
    /// @param InGameParameterRefPtr The GameParameter reference
    /// @return The Name of the GameParameter
    WWISE_PROJECT_DATABASE_API const char* GetGameParameterName(void* InGameParameterRefPtr);

    /// Get the Object Path of an GameParameter
    /// @param InGameParameterRefPtr The GameParameter reference
    /// @return The Object Path of the GameParameter
    WWISE_PROJECT_DATABASE_API const char* GetGameParameterPath(void* InGameParameterRefPtr);

    /// Get the GUID of an GameParameter
    /// @param InGameParameterRefPtr The GameParameter reference
    /// @return A reference to the GUID of the GameParameter
    WWISE_PROJECT_DATABASE_API const WwiseDBGuid* GetGameParameterGuid(void* InGameParameterRefPtr);

    /// Get the ShortID of an GameParameter
    /// @param InGameParameterRefPtr The GameParameter reference
    /// @return The ShortID of the GameParameter
    WWISE_PROJECT_DATABASE_API const WwiseDBShortId GetGameParameterShortId(void* InGameParameterRefPtr);
    
    /// Get the reference of a GameParameter given its name
    /// @param InString The Name of the Event
    /// @return The GameParameter reference
    WWISE_PROJECT_DATABASE_API const WwiseCRefGameParameter* GetGameParameterRefString(const char* InString);

    /*
     * Trigger
    */

    /// Get a reference to the start of a list containing all the Triggers.
    /// @return The reference to the start of a list containing all the Triggers
    WWISE_PROJECT_DATABASE_API const WwiseCRefTrigger* GetAllTriggerRef();

    /// Get the number of Triggers in the Project Database.
    /// @return The number of Triggers in the Project Database
    WWISE_PROJECT_DATABASE_API const unsigned int GetTriggerCount();

    /// Delete a Trigger reference. This should be called upon exiting the scope of the reference.
    /// @param InTriggerRefPtr The Trigger reference
    WWISE_PROJECT_DATABASE_API void DeleteTriggerRef(void* InTriggerRefPtr);

    /// Delete an array containing all the Triggers. This should be called upon exiting the scope of the array.
    /// @param InTriggerArrayRefPtr The reference to an array containing all the Triggers
    WWISE_PROJECT_DATABASE_API void DeleteTriggerArrayRef(void* InTriggerArrayRefPtr);

    /// Get a Trigger in the Trigger Array at a given index
    /// For the index to be valid, it must be greater or equal to 0 and less than GetTriggersCount.
    /// @param InTriggerArrayRefPtr The Trigger Array reference
    /// @param index The Index of the Trigger
    /// @return The Reference of the Trigger or nullptr if the index is invalid
    WWISE_PROJECT_DATABASE_API const WwiseCRefTrigger* GetTrigger(void* InTriggerArrayRefPtr, int index);

    /// Get the name of a Trigger
    /// @param InTriggerRefPtr The Trigger reference
    /// @return The Name of the Trigger
    WWISE_PROJECT_DATABASE_API const char* GetTriggerName(void* InTriggerRefPtr);

    /// Get the Object Path of a Trigger
    /// @param InTriggerRefPtr The Trigger reference
    /// @return The Object Path of the Trigger
    WWISE_PROJECT_DATABASE_API const char* GetTriggerPath(void* InTriggerRefPtr);

    /// Get the GUID of a Trigger
    /// @param InTriggerRefPtr The Trigger reference
    /// @return A reference to the GUID of the Trigger
    WWISE_PROJECT_DATABASE_API const WwiseDBGuid* GetTriggerGuid(void* InTriggerRefPtr);

    /// Get the ShortID of a Trigger
    /// @param InTriggerRefPtr The Trigger reference
    /// @return The ShortID of the Trigger
    WWISE_PROJECT_DATABASE_API const WwiseDBShortId GetTriggerShortId(void* InTriggerRefPtr);

    /// Get the reference of a Trigger given its name
    /// @param InString The Name of the Event
    /// @return The Trigger reference
    WWISE_PROJECT_DATABASE_API const WwiseCRefTrigger* GetTriggerRefString(const char* InString);

    /*
     * Bus
    */

    /// Get a reference to the start of a list containing all the Buses.
    /// @return The reference to the start of a list containing all the Buses
    WWISE_PROJECT_DATABASE_API const WwiseCRefBus* GetAllBusRef();

    /// Get the number of Buses in the Project Database.
    /// @return The number of Buses in the Project Database
    WWISE_PROJECT_DATABASE_API const unsigned int GetBusCount();

    /// Delete a Bus reference. This should be called upon exiting the scope of the reference.
    /// @param InBusRefPtr The Bus reference
    WWISE_PROJECT_DATABASE_API void DeleteBusRef(void* InBusRefPtr);

    /// Delete an array containing all the Buses. This should be called upon exiting the scope of the array.
    /// @param InBusArrayRefPtr The reference to an array containing all the Buses
    WWISE_PROJECT_DATABASE_API void DeleteBusArrayRef(void* InBusArrayRefPtr);

    /// Get a Bus in the Bus Array at a given index
    /// For the index to be valid, it must be greater or equal to 0 and less than GetBusesCount.
    /// @param InBusArrayRefPtr The Bus Array reference
    /// @param index The Index of the Bus
    /// @return The Reference of the Bus or nullptr if the index is invalid
    WWISE_PROJECT_DATABASE_API const WwiseCRefBus* GetBus(void* InBusArrayRefPtr, int index);

    /// Get the name of a Bus
    /// @param InBusRefPtr The Bus reference
    /// @return The Name of the Bus
    WWISE_PROJECT_DATABASE_API const char* GetBusName(void* InBusRefPtr);

    /// Get the Object Path of a Bus
    /// @param InBusRefPtr The Bus reference
    /// @return The Object Path of the Bus
    WWISE_PROJECT_DATABASE_API const char* GetBusPath(void* InBusRefPtr);

    /// Get the GUID of a Bus
    /// @param InBusRefPtr The Bus reference
    /// @return A reference to the GUID of the Bus
    WWISE_PROJECT_DATABASE_API const WwiseDBGuid* GetBusGuid(void* InBusRefPtr);

    /// Get the ShortID of a Bus
    /// @param InBusRefPtr The Bus reference
    /// @return The ShortID of the Bus
    WWISE_PROJECT_DATABASE_API const WwiseDBShortId GetBusShortId(void* InBusRefPtr);

    /// Get the reference of a Bus given its name
    /// @param InString The Name of the Event
    /// @return The Bus reference
    WWISE_PROJECT_DATABASE_API const WwiseCRefBus* GetBusRefString(const char* InString);

    /*
     * AuxBus
    */

    /// Get a reference to the start of a list containing all the AuxBuses.
    /// @return The reference to the start of a list containing all the AuxBuses
    WWISE_PROJECT_DATABASE_API const WwiseCRefAuxBus* GetAllAuxBusRef();

    /// Get the number of AuxBuses in the Project Database.
    /// @return The number of AuxBuses in the Project Database
    WWISE_PROJECT_DATABASE_API const unsigned int GetAuxBusCount();

    /// Delete a AuxBus reference. This should be called upon exiting the scope of the reference.
    /// @param InAuxBusRefPtr The AuxBus reference
    WWISE_PROJECT_DATABASE_API void DeleteAuxBusRef(void* InAuxBusRefPtr);

    /// Delete an array containing all the AuxBuses. This should be called upon exiting the scope of the array.
    /// @param InAuxBusArrayRefPtr The reference to an array containing all the AuxBuses
    WWISE_PROJECT_DATABASE_API void DeleteAuxBusArrayRef(void* InAuxBusArrayRefPtr);

    /// Get a AuxBus in the AuxBus Array at a given index
    /// For the index to be valid, it must be greater or equal to 0 and less than GetAuxBusesCount.
    /// @param InAuxBusArrayRefPtr The AuxBus Array reference
    /// @param index The Index of the AuxBus
    /// @return The Reference of the AuxBus or nullptr if the index is invalid
    WWISE_PROJECT_DATABASE_API const WwiseCRefAuxBus* GetAuxBus(void* InAuxBusArrayRefPtr, int index);

    /// Get the name of an AuxBus
    /// @param InAuxBusRefPtr The AuxBus reference
    /// @return The Name of the AuxBus
    WWISE_PROJECT_DATABASE_API const char* GetAuxBusName(void* InAuxBusRefPtr);

    /// Get the Object Path of an AuxBus
    /// @param InAuxBusRefPtr The AuxBus reference
    /// @return The Object Path of the AuxBus
    WWISE_PROJECT_DATABASE_API const char* GetAuxBusPath(void* InAuxBusRefPtr);

    /// Get the GUID of an AuxBus
    /// @param InAuxBusRefPtr The AuxBus reference
    /// @return A reference to the GUID of the AuxBus
    WWISE_PROJECT_DATABASE_API const WwiseDBGuid* GetAuxBusGuid(void* InAuxBusRefPtr);

    /// Get the ShortID of an AuxBus
    /// @param InAuxBusRefPtr The AuxBus reference
    /// @return The ShortID of the AuxBus
    WWISE_PROJECT_DATABASE_API const WwiseDBShortId GetAuxBusShortId(void* InAuxBusRefPtr);

    /// Get the reference of an AuxBus given its name
    /// @param InString The Name of the Event
    /// @return The AuxBus reference
    WWISE_PROJECT_DATABASE_API const WwiseCRefAuxBus* GetAuxBusRefString(const char* InString);
}