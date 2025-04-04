#pragma once
#include <string>
#include <Windows.h>
#pragma warning(push)
#pragma warning(disable : 4244)
#include <algorithm>
#pragma warning(pop)

namespace CommonUtilities
{
	inline std::wstring StringToWstring(const std::string& aString)
	{
		const int sLength = static_cast<int>(aString.length()) + 1;
		const int len = MultiByteToWideChar(CP_ACP, 0, aString.c_str(), sLength, nullptr, 0);
		std::wstring r(len, L'\0');
		MultiByteToWideChar(CP_ACP, 0, aString.c_str(), sLength, r.data(), len);
		return r;
	}

	// Helper function to convert a string to lowercase
	inline std::string ToLower(const std::string& str)
	{
		std::string result;
		result.reserve(str.size());
		std::transform(str.begin(), str.end(), std::back_inserter(result),
		               [](unsigned char c) { return std::tolower(c); });
		return result;
	}

	// Helper function to check file extension (case-insensitive)
	inline bool HasExtension(const std::string& filePath, const std::string& extension)
	{
		std::string lowerFilePath = ToLower(filePath);
		std::string lowerExtension = ToLower(extension);
		if (lowerFilePath.length() >= lowerExtension.length())
		{
			return (0 == lowerFilePath.compare(lowerFilePath.length() - lowerExtension.length(),
			                                   lowerExtension.length(), lowerExtension));
		}
		return false;
	}
}
