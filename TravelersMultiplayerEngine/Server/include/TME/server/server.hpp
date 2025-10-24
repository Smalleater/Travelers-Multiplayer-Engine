#ifndef TME_SERVER_SERVER_HPP
#define TME_SERVER_SERVER_HPP

#include "TME/export.hpp"

#include <cstdint>

namespace tme::Server
{
	TME_API static void Start(uint16_t _port);
	TME_API static void Stop();
}

#endif