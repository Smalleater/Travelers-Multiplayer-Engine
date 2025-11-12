#ifndef TME_ENGINE_NETWORK_SYSTEM_REGISTRAR_HPP
#define TME_ENGINE_NETWORK_SYSTEM_REGISTRAR_HPP

namespace tme::engine
{
	class NetworkEcs;
	namespace NetworkSystemRegistrar
	{
		void registerNetworkSystems(NetworkEcs* _networkEcs);
	}
}

#endif
