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

#ifndef ServerAudioRoutePage_hpp
#define ServerAudioRoutePage_hpp

#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include "GMEDefines.h"

#ifdef INTDEMO_GME

#include <stdio.h>
#include "Page.h"
#include <set>

class AudioRoutePage : public Page {
public:
    AudioRoutePage(Menu* in_ParentMenu);
    ~AudioRoutePage();
    
    virtual void Draw();
    bool Update();
private:
    
    ToggleControl* send_type_control;
    ToggleControl* recv_type_control;
    ButtonControl* recv_list_btn;
    ButtonControl* send_list_btn;
    ButtonControl* set_audioroute_btn;
    ButtonControl* get_audioroute_btn;
    NumericControl* openid_numric_btn;
    GMEWWisePlugin_AudioRouteMode send_type;
    GMEWWisePlugin_AudioRouteMode recv_type;
    /// Initializes the various components on the page.
    virtual void InitControls();
    void AudioRouteTypeValueChanged(void* in_pSender, ControlEvent* in_pEvent);
    void ButtonPressed(void* in_pSender, ControlEvent* in_pEvent);
    void OutputStringtoScreen(int send_type, const char *send_list, int recv_type, const char *recv_list);
    std::set<std::string> send_list;
    std::set<std::string> recv_list;
    
	char openID[128];
    std::string output_string;
};
#endif // INTDEMO_GME
#endif /* ServerAudioRoutePage_hpp */
