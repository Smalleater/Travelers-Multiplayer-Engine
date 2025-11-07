#ifndef TME_ENGINE_NETWORK_SYSTEM_REGISTRAR_HPP
#define TME_ENGINE_NETWORK_SYSTEM_REGISTRAR_HPP

namespace tme::engine
{
	class NetworkEcs;
	namespace NetworkSystemRegistrar
	{
		void registerNetworkSystems(const NetworkEcs* _networkEcs);
	}
}

#endif
