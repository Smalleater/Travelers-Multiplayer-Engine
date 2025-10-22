#ifndef TME_API_ERRORCODE_HPP
#define TME_API_ERRORCODE_HPP

namespace tme
{
    enum class ErrorCode
    {
        Success = 0,

        //Server Error
        ServerAlreadyExists,
        ServerNotFound,

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
    };
}

#endif