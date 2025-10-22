#ifndef TME_SERVER_SERVER_HPP
#define TME_SERVER_SERVER_HPP

namespace tme::server
{
    class ServerHandler;

    class Server
    {
    private:
        friend  ServerHandler;

    public:
        void Start();
        void Stop();

    private:
        Server();
        ~Server();


    };
}

#endif