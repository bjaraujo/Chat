
#pragma once

#ifdef __linux__
#include <unistd.h>
#else
#include <windows.h>
#endif

#include <vector>
#include <string>
#include <sstream>
#include <iterator>

#include "SDL_net.h"

enum ENetworkMode
{
    NETWORKMODE_LOCAL,
    NETWORKMODE_SERVER,
    NETWORKMODE_CLIENT
};

enum ESocketType
{
    SOCKET_SERVER,
    SOCKET_CLIENT
};

#define SDL_ERROR -1

//! Manages the network communication
class CNetwork
{
private:

    ENetworkMode m_NetworkMode;

    TCPsocket m_Socket;
    TCPsocket m_ClientSocket;
    SDLNet_SocketSet m_socketSet;

	bool           UDPSendString(UDPsocket& udpSocket, IPaddress& ip, std::string data);
	bool		   UDPRecieveString(UDPsocket& udpSocket, IPaddress& ip, std::string& data);

public:

    CNetwork();
    ~CNetwork(void);

    ENetworkMode   NetworkMode();
    void           SetNetworkMode(ENetworkMode NetworkMode);

	bool		   Initialize();
	bool		   Pair(const std::string proxyIpAddressString, int proxyPortNum, std::string& pairIpAddress, int& pairPortNum);
    bool           Connect(const std::string IpAddressString, int port);
    bool           Disconnect();

    bool           Send(ESocketType SocketType, const char* buf, int len);
    int            Receive(ESocketType SocketType, char* buf, int len);
    int            ReceiveNonBlocking(ESocketType SocketType, char* buf, int len);

    unsigned long  CheckSum(const char *buf);

};

