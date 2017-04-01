
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

	UDPsocket m_udpSocket;
	IPaddress m_ip;

	bool           UDPSendString(IPaddress& ip, std::string data);
	bool		   UDPRecieveString(IPaddress& ip, std::string& data);

public:

    CNetwork();
    ~CNetwork(void);

    ENetworkMode   NetworkMode();
    void           SetNetworkMode(ENetworkMode NetworkMode);

	bool		   Initialize();
	bool		   Pair(const std::string proxyIpAddressString, int proxyPortNum, std::string& pairIpAddress, int& pairPortNum);

    bool           Send(const std::string data);
    bool           Receive(std::string& data);

};

