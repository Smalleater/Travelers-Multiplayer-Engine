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

		bool isLocalAddress(const std::string& _address)
		{
			TME_ASSERT_REF_PTR_OR_COPIABLE(_address);

			if (_address == "127.0.0.1" || _address == "localhost")
			{
				return true;
			}

			char hostname[256];
			if (gethostname(hostname, sizeof(hostname)))
			{
				return false;
			}

			addrinfo hints = {};
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_flags = AI_PASSIVE;

			addrinfo* result = nullptr;
			if (getaddrinfo(hostname, nullptr, &hints, &result))
			{
				return false;
			}

			bool found = false;
			for (addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next)
			{
				char ipstr[INET_ADDRSTRLEN] = {};
				sockaddr_in* sockaddr_ipv4 = reinterpret_cast<sockaddr_in*>(ptr->ai_addr);
				if (inet_ntop(AF_INET, &(sockaddr_ipv4->sin_addr), ipstr, sizeof(ipstr)))
				{
					std::string localIp(ipstr);
					if (localIp == _address)
					{
						found = true;
						break;
					}
				}
			}

			freeaddrinfo(result);
			return found;
		}
	}
}