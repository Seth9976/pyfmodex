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
#include <cstdarg>
#include <cstdio>
#include <string>

#include "AdapterTypes/WwiseProjectDatabaseLoggingSeverity.h"
#include "AdapterTypes/Standard/WwiseProjectDatabaseLoggingUtils.h"

struct WwiseProjectDatabaseLoggingParameters
{
	WwiseProjectDatabaseLoggingSeverity CurrentLogVerbosity = Log;
	static WwiseProjectDatabaseLoggingParameters* Get();
private:
	WwiseProjectDatabaseLoggingParameters(){}
};

inline bool IsAnError(WwiseProjectDatabaseLoggingSeverity Severity)
{
	return Severity <= Error;
}

inline void SetVerbosity(WwiseProjectDatabaseLoggingSeverity NewSeverity)
{
	WwiseProjectDatabaseLoggingParameters::Get()->CurrentLogVerbosity = NewSeverity;
}

inline void StandardLogging(bool bCondition, WwiseProjectDatabaseLoggingSeverity Severity, const char* Message, ...)
{
	if(!bCondition)
	{
		return;
	}
	

	if((int)Severity > (int)WwiseProjectDatabaseLoggingParameters::Get()->CurrentLogVerbosity)
	{
		return;
	}
	char FormattedString[256];
	va_list Args;
	va_start(Args, Message);
	vsnprintf(FormattedString, 256, Message, Args);
	va_end(Args);
	std::string SeverityString;
	switch(Severity)
	{
	case Fatal:
		SeverityString = "Fatal";
		break;
	case Error:
		SeverityString = "Error";
		break;
	case Warning:
		SeverityString = "Warning";
		break;
	case Log:
		SeverityString = "Log";
		break;
	case Display:
		SeverityString = "Display";
		break;
	case Verbose:
		SeverityString = "Verbose";
		break;
	case VeryVerbose:
		SeverityString = "VeryVerbose";
		break;
	default:
		break;
	}
	
	if(IsAnError(Severity))
	{
		std::string LogToSend = "LogWwiseProjectDatabase " + SeverityString + ": " + FormattedString + "\n";
		WwiseProjectDatabaseLoggingUtils::LogError(LogToSend.c_str());
	}
	else
	{
		printf("LogWwiseProjectDatabase %s: %s\n", SeverityString.c_str(), FormattedString);
	}
}