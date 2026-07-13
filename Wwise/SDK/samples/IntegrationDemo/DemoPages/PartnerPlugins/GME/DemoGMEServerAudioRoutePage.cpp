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

#include"stdafx.h"
#include "DemoGMEServerAudioRoutePage.h"
#include "Menu.h"

#ifdef INTDEMO_GME

AudioRoutePage::AudioRoutePage( Menu* in_ParentMenu ):Page( in_ParentMenu, GMEWWisePlugin_GetVersion()) {
    m_szHelp  = "Use this page to set voice routing rules.\n";
    send_type_control = NULL;
    recv_type_control = NULL;
    recv_list_btn = NULL;
    send_list_btn = NULL;
    set_audioroute_btn = NULL;
    get_audioroute_btn = NULL;
    openid_numric_btn = NULL;
	output_string = "";
    
    send_type = GMESDK_AUDIO_ROUTE_RULE_ALL;
    recv_type = GMESDK_AUDIO_ROUTE_RULE_ALL;
}

AudioRoutePage::~AudioRoutePage()
{

}

void AudioRoutePage::InitControls() {
    send_type_control = new ToggleControl(*this);
    send_type_control->SetLabel("Send routing type:");
    send_type_control->AddOption("All");
    send_type_control->AddOption("None");
    send_type_control->AddOption("BlockList");
    send_type_control->AddOption("AllowList");
    send_type_control->SetDelegate((PageMFP)&AudioRoutePage::AudioRouteTypeValueChanged);
    send_type_control->CallDelegate(NULL); // Force the RTPC to the correct initial value
    m_Controls.push_back(send_type_control);
    
    recv_type_control = new ToggleControl(*this);
    recv_type_control->SetLabel("Receive routing type:");
    recv_type_control->AddOption("All");
    recv_type_control->AddOption("None");
    recv_type_control->AddOption("BlockList");
    recv_type_control->AddOption("AllowList");
    recv_type_control->SetDelegate((PageMFP)&AudioRoutePage::AudioRouteTypeValueChanged);
    recv_type_control->CallDelegate(NULL); // Force the RTPC to the correct initial value
    m_Controls.push_back(recv_type_control);
    
    openid_numric_btn = new NumericControl(*this);
    openid_numric_btn->SetLabel("Far-end User ID:");
    openid_numric_btn->SetMinValue(10000);
    openid_numric_btn->SetMaxValue(10000000);
    openid_numric_btn->SetIncrement(100);
    openid_numric_btn->SetValue(50000);
    openid_numric_btn->SetInitialIncrement(100);
    openid_numric_btn->SetDelegate(NULL);
    openid_numric_btn->SetDelegate((PageMFP)&AudioRoutePage::ButtonPressed);
    openid_numric_btn->CallDelegate(NULL); // Force the RTPC to the correct initial value
    m_Controls.push_back(openid_numric_btn);

    send_list_btn = new ButtonControl(*this);
    send_list_btn->SetLabel("Add above UserID to send_list");
    send_list_btn->SetDelegate((PageMFP)&AudioRoutePage::ButtonPressed);
    m_Controls.push_back(send_list_btn);
    
    recv_list_btn = new ButtonControl(*this);
    recv_list_btn->SetLabel("Add above UserID to recv_list");
    recv_list_btn->SetDelegate((PageMFP)&AudioRoutePage::ButtonPressed);
    m_Controls.push_back(recv_list_btn);
    
    set_audioroute_btn = new ButtonControl(*this);
    set_audioroute_btn->SetLabel("Set voice routing rules to server");
    set_audioroute_btn->SetDelegate((PageMFP)&AudioRoutePage::ButtonPressed);
    m_Controls.push_back(set_audioroute_btn);

    get_audioroute_btn = new ButtonControl(*this);
    get_audioroute_btn->SetLabel("Get voice routing rules from server");
    get_audioroute_btn->SetDelegate((PageMFP)&AudioRoutePage::ButtonPressed);
    m_Controls.push_back(get_audioroute_btn);

}

void AudioRoutePage::ButtonPressed(void* in_pSender, ControlEvent* in_pEvent) {
    
    if (openid_numric_btn == in_pSender) {
        NumericControl* sender = (NumericControl*)in_pSender;
        sprintf(openID, "%d", (AkInt32)sender->GetValue());

    } else if (recv_list_btn == in_pSender) {
        AkUInt64 openid = (AkUInt64)openid_numric_btn->GetValue();
        recv_list.insert(std::to_string(openid));

        output_string = openID;
        output_string += " is added to recv_list";
        return;
        
    } else if(send_list_btn == in_pSender) {
        AkUInt64 openid = (AkUInt64)openid_numric_btn->GetValue();
        send_list.insert(std::to_string(openid));

        output_string = openID;
        output_string += " is added to send_list";
        return;

    } else if(set_audioroute_btn == in_pSender) {
        char* openId_for_send = NULL;
        char* openId_for_recv = NULL;
        const int total_buff_len = 1024;
        if (send_list.size() > 0) {
            openId_for_send = new char[total_buff_len];
            int current_len = 0;
            for(std::set<std::string>::iterator it = send_list.begin(); it != send_list.end() && current_len < total_buff_len; it++) {
                snprintf(openId_for_send + current_len, total_buff_len - current_len, "%s,", it->c_str());
                current_len += (int)it->length() + 1;
            }
            openId_for_send[current_len - 1] = 0;
            
        }
        
        if (recv_list.size() > 0) {
            openId_for_recv = new char[total_buff_len];
            int current_len = 0;
            for(std::set<std::string>::iterator it = recv_list.begin(); it != recv_list.end() && current_len < total_buff_len; it++) {
                snprintf(openId_for_recv + current_len, total_buff_len - current_len, "%s,", it->c_str());
                current_len += (int)it->length() + 1;
            }
            openId_for_recv[current_len - 1] = 0;
            
        }

        int ret =  GMEWWisePlugin_SetServerAudioRoute(send_type, openId_for_send, recv_type, openId_for_recv);
        
        if (ret != 0) {
            output_string = "Error_InvalidArgument";
        }
        else {
            OutputStringtoScreen(send_type, openId_for_send ? openId_for_send : "" , recv_type, openId_for_recv ? openId_for_recv : "");
        }
        
        if (openId_for_send) {
            delete []openId_for_send;
        }
        if (openId_for_recv) {
            delete []openId_for_recv;
        }
        send_list.clear();
        recv_list.clear();
        
    } else if(get_audioroute_btn == in_pSender) {
        char *openId_for_send = new char[1024];
        char *openId_for_recv = new char[1024];
        openId_for_send[0] = 0;
        openId_for_recv[0] = 0;
        int send_list_buf_len = 1024;
        int recv_list_buf_len = 1024;
        
        GMEWWisePlugin_AudioRouteMode send_type = GMESDK_AUDIO_ROUTE_RULE_ALL;
        GMEWWisePlugin_AudioRouteMode recv_type = GMESDK_AUDIO_ROUTE_RULE_ALL;

        GMEWWisePlugin_GetServerAudioRoute(&send_type, openId_for_send, send_list_buf_len, &recv_type, openId_for_recv,recv_list_buf_len);
        OutputStringtoScreen(send_type, openId_for_send, recv_type, openId_for_recv);

        if (openId_for_send) {
            delete []openId_for_send;
        }
        if (openId_for_recv) {
            delete []openId_for_recv;
        }
        
    }
}

void AudioRoutePage::AudioRouteTypeValueChanged(void* in_pSender, ControlEvent* in_pEvent) {
    
    GMEWWisePlugin_AudioRouteMode *audioroute_type = NULL;
    ToggleControl* sender = NULL;
    if (send_type_control == in_pSender) {
        sender = send_type_control;
        audioroute_type = &send_type;
        
    } else if(recv_type_control == in_pSender) {
        sender = recv_type_control;
        audioroute_type = &recv_type;
    }
    string type_string = sender->SelectedText();
    if (type_string == "None") {
        *audioroute_type = GMESDK_AUDIO_ROUTE_RULE_NONE;
    } else if(type_string == "All") {
        *audioroute_type = GMESDK_AUDIO_ROUTE_RULE_ALL;
    } else if(type_string == "BlockList") {
        *audioroute_type = GMESDK_AUDIO_ROUTE_RULE_BLOCKLIST;
    } else if(type_string == "AllowList") {
        *audioroute_type = GMESDK_AUDIO_ROUTE_RULE_ALLOWLIST;
    }
}

bool AudioRoutePage::Update()
{
    int localUTCTime = 0;
    int messageType = 0;
    int code = 0;
    char message[256] = {0};
    char message2[256] = {0};
    char message3[256] = {0};
    char result[512] = {0};
    GMEWWisePlugin_GetMessage(&localUTCTime, &messageType, &code, message, 256, message2, 256);

    switch (messageType)
    {
        case GMESDK_MESSAGETYPE_SERVER_AUDIO_ROUTE_COMPLETE:
            snprintf(result, 512, "Voice routing rules has been set to the server :%d errorinfo :%s",code, message);
            output_string = result;
            break;
        default:
            break;
    }

    return Page::Update();
}

void AudioRoutePage::OutputStringtoScreen(int send_type, const char *send_list, int recv_type, const char *recv_list)
{
    char message[512] = { 0 };
    output_string = "";
    output_string += "send_type:";
    static const char* audio_route[] = { "None", "All", "Blocklist", "Allowlist" };
    output_string += audio_route[send_type - 1];
    output_string += "\nsend_list:";
    output_string += send_list;
    
    output_string += "\nrecv_type:";
    output_string += audio_route[recv_type - 1];
    output_string += "\nrecv_type_list:";
    output_string += recv_list;
}

void AudioRoutePage::Draw()
{
    Page::Draw();
    int iHeight = m_pParentMenu->GetHeight();
    int iWidth  = m_pParentMenu->GetWidth();
    DrawTextOnScreen(output_string.c_str(), 0, iHeight*5 / 6, DrawStyle_Text);
}

#endif // INTDEMO_GME
