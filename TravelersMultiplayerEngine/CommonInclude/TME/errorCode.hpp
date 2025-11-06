#ifndef TME_API_ERRORCODE_HPP
#define TME_API_ERRORCODE_HPP

namespace tme
{
	enum class ErrorCode
	{
		Success = 0,

		// Client/Server Error
		DisconnectWithErrors,

		// Server Error
		ServerAlreadyStarted,

		// Client Error
		ClientAlreadyConnected,

		// Engine Error
		NetworkEngineNotInitialized,
		InvalidIpAddress,
		InvalidPortNumber,

		// Network ECS Error
		EntityDoesNotHaveComponent,
		EntityAlreadyHasComponent,
		ComponentStoreUnavailable,
		InvalidEntity,
		EntityDoesNotExist,

		// Socket Error
		SocketAlreadyOpen,
		SocketNotOpen,
		SocketConnectionClosed,
		SocketWouldBlock,
		SocketShutdownFailed,
		SocketGetaddrinfoFailed,
		SocketConnectFailed,
		SocketBindFailed,
		SocketListenFailed,
		SocketAcceptFailed,
		SocketSendSizeTooLarge,
		SocketSendFailed,
		SocketReceiveSizeTooLarge,
		SocketReceiveFailed,
		SocketSetBlockingFailed,
		SocketGetPortFailed,

		// WSA Error
		WSAStartupFailed
	};
}

#endif