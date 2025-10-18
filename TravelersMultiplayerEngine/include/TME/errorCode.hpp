#ifndef TME_API_ERRORCODE_HPP
#define TME_API_ERRORCODE_HPP

namespace tme
{
    enum class ErrorCode
    {
        Success = 0,

        // Socket Error
        SocketShutdownFailed
    };
}

#endif