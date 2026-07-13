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

#include <vector>
#include <string>

struct WwiseProjectDatabaseLoggingUtils
{
    static std::vector<std::string> Errors;
    static std::vector<std::string> ExpectedErrors;

    static void ParseErrors();

    static void ResetErrors()
    {
        Errors.clear();
        ExpectedErrors.clear();
    }

    static void LogError(const char* Message)
    {
        fputs(Message, stderr);
        Errors.push_back(Message);
    }

    static void AddExpectedError(const std::string& error, int occurence = 1)
    {
        for(int i = 0; i < occurence; i++)
        {
            ExpectedErrors.push_back(error);
        }
    }
};
