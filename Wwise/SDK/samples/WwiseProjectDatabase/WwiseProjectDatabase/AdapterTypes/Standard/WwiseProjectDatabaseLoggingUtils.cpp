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

#include <vector>
#include <string>
#include "AdapterTypes/Standard/WwiseProjectDatabaseLoggingUtils.h"

std::vector<std::string> WwiseProjectDatabaseLoggingUtils::Errors;
std::vector<std::string> WwiseProjectDatabaseLoggingUtils::ExpectedErrors;

void WwiseProjectDatabaseLoggingUtils::ParseErrors()
{
    for(unsigned int i = 0; i < Errors.size();)
    {
        bool removed = false;
        for(unsigned int j = 0; j < ExpectedErrors.size();)
        {
            if(Errors[i].find(ExpectedErrors[j]))
            {
                Errors.erase(Errors.begin() + i);
                ExpectedErrors.erase(ExpectedErrors.begin() + j);
                removed = true;
                break;
            }
            j++;
        }
        if(!removed)
        {
            i++;
        }
    }

    for(auto& Error : Errors)
    {
        fprintf(stderr, "Unhandled Error \"%ss\"\n", Error.c_str());
    }
    
    for(auto& Error : ExpectedErrors)
    {
        fprintf(stderr, "Expected Error \"%s\" did not appear\n", Error.c_str());
    }
    
    if(!Errors.empty() || !ExpectedErrors.empty())
    {
        throw std::exception();
    }
}
