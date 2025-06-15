#pragma once

namespace tme
{
	class Utils
	{
	public:
		static bool isOnlyDigits(const char* str);

		static bool isValidIPv4(const char* ip);
		static bool isValidPort(const char* portStr);

	private:

	};
}
