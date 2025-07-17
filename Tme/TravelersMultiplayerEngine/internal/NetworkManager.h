#pragma once

#include "TME/ErrorCodes.h"

namespace tme
{
	class NetworkManager
	{
	public:
		NetworkManager() {}
		~NetworkManager() {}

		ErrorCodes StartServer();
		ErrorCodes StartClient();

	private:

	};
}
