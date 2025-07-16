#include "TME/Utils.h"

#include <Windows.h>
#include <initializer_list>

namespace tme
{
	bool Utils::HasAnyConsoleOutput()
	{
		for (DWORD stdHandle : {STD_OUTPUT_HANDLE, STD_ERROR_HANDLE})
		{
			HANDLE h = GetStdHandle(stdHandle);
			if (h != INVALID_HANDLE_VALUE && h != nullptr)
			{
				if (GetFileType(h) == FILE_TYPE_CHAR)
				{
					return true;
				}
			}
		}

		return false;
	}
}
