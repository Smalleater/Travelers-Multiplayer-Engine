#include "TME/core/netUtils.hpp"

#include "TME/debugUtils.hpp"
#include "TME/core/networkInclude.hpp"

namespace tme::core
{
	namespace NetUtils
	{
		bool isValidIpV4Address(const std::string& _address)
		{
			TME_ASSERT_REF_PTR_OR_COPIABLE(_address);

			sockaddr_in sa;
			int result = inet_pton(AF_INET, _address.c_str(), &(sa.sin_addr));
			return result == 1;
		}

		bool isValidPort(uint16_t _port)
		{
			return _port <= 65535;
		}
	}
}