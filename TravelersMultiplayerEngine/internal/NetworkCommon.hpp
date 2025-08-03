#ifndef TME_NETWORK_COMMON_HPP
#define TME_NETWORK_COMMON_HPP

namespace tme
{
    // Windows Sockets
    #ifdef _WIN32

    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif

    #include <Windows.h>
    #include <WinSock2.h>
    #include <WS2tcpip.h>
    #include <iphlpapi.h>

    #pragma comment(lib, "Ws2_32.lib")

    using socket_t = SOCKET;
    #define CLOSE_SOCKET closesocket
    #define INVALID_SOCKET_FD INVALID_SOCKET
    #define WOULD_BLOCK_ERROR WSAEWOULDBLOCK

    // POSIX Sockets
    #else

    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <errno.h>

    using socket_t = int;
    #define CLOSE_SOCKET close
    #define INVALID_SOCKET_FD -1
    #define WOULD_BLOCK_ERROR EWOULDBLOCK

    #endif
}

#endif