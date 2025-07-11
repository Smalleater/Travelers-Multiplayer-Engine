#include "TME/Old/Utils.h"

#include <cstddef>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

namespace tme
{
	bool Utils::isOnlyDigits(const char* str)
	{
		if (str == NULL || *str == '\0') {
			return 0;
		}

		while (*str) {
			if (!isdigit((unsigned char)*str)) {
				return 0;
			}

			str++;
		}

		return true;
	}

	bool Utils::isValidIPv4(const char* ip)
	{
		if (ip == NULL) return 0;

		char ipCopy[32];
		strncpy_s(ipCopy, sizeof(ipCopy), ip, _TRUNCATE);
		ipCopy[sizeof(ipCopy) - 1] = '\0';

		int count = 0;
		char* context = NULL;
		char* token = strtok_s(ipCopy, ".", &context);

		while (token != NULL) {
			if (!isOnlyDigits(token)) return 0;

			int num = atoi(token);
			if (num < 0 || num > 255) return 0;

			count++;
			token = strtok_s(NULL, ".", &context);
		}

		return count == 4;
	}

	bool Utils::isValidPort(const char* portStr)
	{
		if (!isOnlyDigits(portStr)) return 0;

		long port = strtol(portStr, NULL, 10);
		return port >= 1 && port <= 65535;
	}
}