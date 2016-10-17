
#include <iostream>

#include "network.h"

CNetwork::CNetwork()
{

    m_NetworkMode = NETWORKMODE_LOCAL;

    m_Socket = NULL;

}

CNetwork::~CNetwork(void)
{


}

ENetworkMode CNetwork::NetworkMode()
{

    return m_NetworkMode;

}

void CNetwork::SetNetworkMode(ENetworkMode NetworkMode)
{

    m_NetworkMode = NetworkMode;

}

bool CNetwork::Connect(const char* IpAddressString, int port)
{

    if (SDLNet_Init() == SDL_ERROR)
    {
        std::cout << "Init failed: " << SDLNet_GetError();
        return false;
    }

    if (m_NetworkMode == NETWORKMODE_SERVER)
    {

        IPaddress ip;

        if (SDLNet_ResolveHost(&ip, NULL, port) == SDL_ERROR)
        {
            std::cout << "Listen failed: " << SDLNet_GetError();
            return false;
        }

        m_Socket = SDLNet_TCP_Open(&ip);

        if (!m_Socket)
        {
            std::cout << "Open failed: " << SDLNet_GetError();
            return false;
        }

        m_socketSet = SDLNet_AllocSocketSet(2);

        SDLNet_TCP_AddSocket(m_socketSet, m_Socket);

        // Wait for the client
        while (1)
        {

            m_ClientSocket = SDLNet_TCP_Accept(m_Socket);

#ifdef LINUX
			usleep(20 * 1000);   // usleep takes sleep time in us (1 millionth of a second)
#endif
#ifdef WINDOWS
			Sleep(20);
#endif

            if (m_ClientSocket)
                break;

        }

        SDLNet_TCP_AddSocket(m_socketSet, m_ClientSocket);

    }
    else if (m_NetworkMode == NETWORKMODE_CLIENT)
    {

        IPaddress ip;

        if (SDLNet_ResolveHost(&ip, IpAddressString, port) == SDL_ERROR)
        {
            std::cout << "Connection failed: " << SDLNet_GetError();
            return false;
        }

        m_Socket = SDLNet_TCP_Open(&ip);

        if (!m_Socket)
        {
            std::cout << "Open failed: " << SDLNet_GetError();
            return false;
        }

        m_socketSet = SDLNet_AllocSocketSet(1);

        SDLNet_TCP_AddSocket(m_socketSet, m_Socket);

    }

    return true;

}

bool CNetwork::Disconnect()
{

    if (m_NetworkMode != NETWORKMODE_LOCAL)
    {

        SDLNet_TCP_Close(m_Socket);

        if (m_NetworkMode == NETWORKMODE_SERVER)
            SDLNet_TCP_Close(m_ClientSocket);

    }

    return true;

}

bool CNetwork::Send(ESocketType SocketType, const char* buf, int len)
{

    int Sent = 0;

    if (SocketType == SOCKET_SERVER)
        Sent = SDLNet_TCP_Send(m_Socket, buf, len);
    else if (SocketType == SOCKET_CLIENT)
        Sent = SDLNet_TCP_Send(m_ClientSocket, buf, len);

    if (Sent == SDL_ERROR)
    {
        std::cout << "Send error: " << SDLNet_GetError();
        return false;
    }

    return true;

}

int CNetwork::Receive(ESocketType SocketType, char* buf, int len)
{

    if (SocketType == SOCKET_SERVER)
    {
        return SDLNet_TCP_Recv(m_Socket, buf, len);
    }
    else if (SocketType == SOCKET_CLIENT)
    {
        return SDLNet_TCP_Recv(m_ClientSocket, buf, len);
    }
    else
        return 0;

}

int CNetwork::ReceiveNonBlocking(ESocketType SocketType, char* buf, int len)
{

    int active = SDLNet_CheckSockets(m_socketSet, 1);

    if (active > 0)
    {
        if (SocketType == SOCKET_SERVER)
        {
            if (SDLNet_SocketReady(m_Socket))
                return SDLNet_TCP_Recv(m_Socket, buf, len);
            else
                return 0;
        }
        else if (SocketType == SOCKET_CLIENT)
        {
            if (SDLNet_SocketReady(m_ClientSocket))
                return SDLNet_TCP_Recv(m_ClientSocket, buf, len);
            else
                return 0;
        }
        else
            return 0;

    }
    else
        return 0;

}

unsigned long CNetwork::CheckSum(const char *buf)
{
    unsigned long hash = 5381;
    int c;

    while (c = *buf++) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

