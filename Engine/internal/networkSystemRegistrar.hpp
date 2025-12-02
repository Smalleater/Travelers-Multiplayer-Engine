#ifndef TRA_ENGINE_NETWORK_SYSTEM_REGISTRAR_HPP
#define TRA_ENGINE_NETWORK_SYSTEM_REGISTRAR_HPP

namespace tra::engine
{
	class NetworkEcs;
	namespace NetworkSystemRegistrar
	{
		void registerNetworkSystems(NetworkEcs* _networkEcs);
	}
}

#endif
