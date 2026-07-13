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

#include "AdapterTypes/WwiseGuid.h"
#include "AdapterTypes/Standard/WwiseStandardString.h"
#include "AdapterTypes/WwiseDataTypesAdapter.h"

#include <string>

class GuidType
{
	static int HexDigit(char c)
	{
		int Result = 0;

		if (c >= '0' && c <= '9')
		{
			Result = c - '0';
		}
		else if (c >= 'A' && c <= 'F')
		{
			Result = c + 10 - 'A';
		}
		else
		{
			Result = -1;
		}

		return Result;
	}

	static void AdjustGuid(std::string& GuidString)
	{
		if(GuidString.length() != 8)
		{
			int diff = 8 - (int)GuidString.length();
			GuidString.insert(0, diff, '0');
		}
	}

	unsigned int HexNumber(const std::string& HexString) const
	{
		unsigned int Ret = 0;

		for(unsigned int i = 0; i < HexString.size(); i++)
		{
			Ret *= 16;
			int Digit = HexDigit(HexString[i]);
			if(Digit == -1)
			{
				return -1;
			}
			Ret += Digit;
		}

		return Ret;
	}
public:
	unsigned int Values[4] = {0, 0, 0, 0};

	GuidType()
	{
	}
	
	GuidType(unsigned int InA, unsigned int InB, unsigned int InC, unsigned int InD)
	{
		Values[0] = InA;
		Values[1] = InB;
		Values[2] = InC;
		Values[3] = InD;
	}

	GuidType(const std::string& InGuidStr)
	{
		if (!Parse(InGuidStr))
		{
			for(int i = 0; i < 4; i++)
			{
				Values[i] = 0;
			}
		}
	}

	bool Parse(const std::string& GuidString)
	{
		std::string NormalizedGuidString;
		if ((GuidString[9] != '-') ||
			(GuidString[14] != '-') ||
			(GuidString[19] != '-') ||
			(GuidString[24] != '-'))
		{
			return false;
		}

		NormalizedGuidString += GuidString.substr(1, 8);
		NormalizedGuidString += GuidString.substr(10, 4);
		NormalizedGuidString += GuidString.substr(15, 4);
		NormalizedGuidString += GuidString.substr(20, 4);
		NormalizedGuidString += GuidString.substr(25, 12);

		for(int i = 0; i < 4; i++)
		{
			auto Value = HexNumber(NormalizedGuidString.substr(i * 8, 8));
			if(Value == (unsigned int)-1)
			{
				for(int j = 0; j < 4; j++)
				{
					Values[j] = 0;
				}
				return false;
			}
			Values[i] = Value;
		}
		
		return true;
	}

	std::string ToString() const
	{
		std::string Guids[4];

		for (auto i = 0; i < 4; ++i)
		{
			std::vector<char> Buffer(8 + 1);
#ifdef _WIN32
			int ResultSize = sprintf_s(Buffer.data(), Buffer.size(), "%X", Values[i]);
#else
			int ResultSize = snprintf(Buffer.data(), Buffer.size(), "%X", Values[i]);
#endif

			if (ResultSize < 0)
			{
				return "{00000000-0000-0000-0000-00000000000}";
			}

			Guids[i] = std::string(Buffer.data());
			AdjustGuid(Guids[i]);
		}

		std::string GuidString = Guids[0] + "-" + Guids[1].substr(0, 4) + "-" +
			Guids[1].substr(4) + "-" + Guids[2].substr(0, 4) + "-" + Guids[2].substr(4) + Guids[3];
		return "{" + GuidString + "}";
	}

	bool IsValid() const
	{
		return ((Values[0] | Values[1] | Values[2] | Values[3]) != 0);
	}
	/**
	 * Compares two GUIDs for equality.
	 *
	 * @param X The first GUID to compare.
	 * @param Y The second GUID to compare.
	 * @return true if the GUIDs are equal, false otherwise.
	 */
	friend bool operator==(const GuidType& X, const GuidType& Y)
	{
		return ((X.Values[0] ^ Y.Values[0]) | (X.Values[1] ^ Y.Values[1]) |
			(X.Values[2] ^ Y.Values[2]) | (X.Values[3] ^ Y.Values[3])) == 0;
	}

	/**
	 * Compares two GUIDs for inequality.
	 *
	 * @param X The first GUID to compare.
	 * @param Y The second GUID to compare.
	 * @return true if the GUIDs are not equal, false otherwise.
	 */
	friend bool operator!=(const GuidType& X, const GuidType& Y)
	{
		return ((X.Values[0] ^ Y.Values[0]) | (X.Values[1] ^ Y.Values[1])
			| (X.Values[2] ^ Y.Values[2]) | (X.Values[3] ^ Y.Values[3])) != 0;
	}

	/**
	 * Compares two GUIDs.
	 *
	 * @param X The first GUID to compare.
	 * @param Y The second GUID to compare.
	 * @return true if the first GUID is less than the second one.
	 */
	friend bool operator<(const GuidType& X, const GuidType& Y)
	{
		return	X.Values[0] < Y.Values[0] ? true : X.Values[0] > Y.Values[0] ? false :
				X.Values[1] < Y.Values[1] ? true : X.Values[1] > Y.Values[1] ? false :
				X.Values[2] < Y.Values[2] ? true : X.Values[2] > Y.Values[2] ? false :
				X.Values[3] < Y.Values[3] ? true : X.Values[3] > Y.Values[3] ? false : false;
	}

};

class WwiseStandardGuid : public IWwiseGuid<GuidType>
{
public:
	WwiseStandardGuid(){}

	WwiseStandardGuid(const WwiseStandardString& InString)
	{
		Parse(InString);
	}

	WwiseStandardGuid(int A, int B, int C, int D)
	{
		this->Guid = GuidType(A, B, C, D);
	}
	
	bool Parse(const WwiseStandardString& String)
	{
		return Guid.Parse(String.String);
	}

	static bool Parse(WwiseStandardString& String, WwiseStandardGuid& OutGuid)
	{
		return OutGuid.Parse(String);
	}

	bool IsValid() const override
	{
		return this->Guid.IsValid();	
	}

	WwiseStandardString ToString() const
	{
		return WwiseStandardString(this->Guid.ToString());
	}

	void GetGuidValues(unsigned int& A, unsigned int& B, unsigned int& C, unsigned int& D) const
	{
		A = this->Guid.Values[0];
		B = this->Guid.Values[1];
		C = this->Guid.Values[2];
		D = this->Guid.Values[3];
	}
};

inline size_t GetTypeHash(const WwiseStandardGuid& Guid)
{
	size_t value0 = std::hash<unsigned int>()(Guid.Guid.Values[0]);
	size_t value1 = std::hash<unsigned int>()(Guid.Guid.Values[1]);
	size_t value2 = std::hash<unsigned int>()(Guid.Guid.Values[2]);
	size_t value3 = std::hash<unsigned int>()(Guid.Guid.Values[3]);
	return value0 ^ value1 ^ value2 ^ value3;
}
template <>
struct std::hash<WwiseStandardGuid>
{
	std::size_t operator()(const WwiseStandardGuid& InGuid) const noexcept
	{
		return GetTypeHash(InGuid);
	}
};
