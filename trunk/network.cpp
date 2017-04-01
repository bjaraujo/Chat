
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

bool CNetwork::Initialize()
{

	if (SDLNet_Init() == SDL_ERROR)
	{
		std::cout << "Init failed: " << SDLNet_GetError() << std::endl;
		return false;
	}

	return true;

}

bool CNetwork::UDPSendString(UDPsocket& udpSocket, IPaddress& ip, std::string data)
{

	UDPpacket packet;
	packet.address = ip;
	packet.channel = -1;
	packet.data = (Uint8*) data.c_str();
	packet.len = data.length();

	std::cout << "Sending: " << data << std::endl;

	int numSent = SDLNet_UDP_Send(udpSocket, packet.channel, &packet);

	if (!numSent) {
		std::cout << "SDLNet_UDP_Send: " << SDLNet_GetError() << std::endl;
		return false;
	}

	return true;

}

bool CNetwork::UDPRecieveString(UDPsocket& udpSocket, IPaddress& ip, std::string& data)
{

	// try to receive a waiting udp packet
	//UDPsocket udpsock;
	UDPpacket *packet;
	
	if (!(packet = SDLNet_AllocPacket(1024)))
	{
		std::cout << "SDLNet_AllocPacket: " << SDLNet_GetError() << std::endl;
		return false;
	}

	int numRecv = SDLNet_UDP_Recv(udpSocket, packet);

	if (numRecv) 
	{
		data = std::string((char*)packet->data, packet->len);
		return true;
	}

	return false;

}

bool CNetwork::Pair(std::string proxyIpAddressString, int proxyPortNum, std::string& pairIpAddress, int& pairPortNum)
{

	// create a UDPsocket
	UDPsocket udpSocket = SDLNet_UDP_Open(proxyPortNum);
	
	if (!udpSocket) {
		std::cout << "UDP open failed:" << SDLNet_GetError() << std::endl;
		return false;
	}

	IPaddress ip;

	if (SDLNet_ResolveHost(&ip, proxyIpAddressString.c_str(), proxyPortNum) == SDL_ERROR)
	{
		std::cout << "Connection failed: " << SDLNet_GetError() << std::endl;
		return false;
	}

	UDPSendString(udpSocket, ip, "START");

	while (true)
	{

		if (m_NetworkMode == NETWORKMODE_SERVER)
			UDPSendString(udpSocket, ip, "A");
		else if (m_NetworkMode == NETWORKMODE_CLIENT)
			UDPSendString(udpSocket, ip, "B");

		std::string data;

		if (UDPRecieveString(udpSocket, ip, data))
		{

			int pos = data.find(":");

			if (pos != std::string::npos) {

				pairIpAddress = data.substr(0, pos);
				pairPortNum = stoi(data.substr(pos + 1, data.length() - pos - 1));

				std::cout << "Pair ip address: " << data << std::endl;
				break;
			}

		}

#ifdef __linux__
		usleep(1000 * 1000);   // usleep takes sleep time in us (1 millionth of a second)
#else
		Sleep(1000);
#endif

	}

	UDPSendString(udpSocket, ip, "END");

	// Pair ip address
	IPaddress pairIp;

	if (SDLNet_ResolveHost(&pairIp, pairIpAddress.c_str(), pairPortNum) == SDL_ERROR)
	{
		std::cout << "Connection failed: " << SDLNet_GetError() << std::endl;
		return false;
	}

	for (int i = 0; i < 5; i++)
	{

		if (m_NetworkMode == NETWORKMODE_SERVER)
			UDPSendString(udpSocket, pairIp, "Hello CLIENT");
		else if (m_NetworkMode == NETWORKMODE_CLIENT)
			UDPSendString(udpSocket, pairIp, "Hello SERVER");

#ifdef __linux__
		usleep(1000 * 1000);   // usleep takes sleep time in us (1 millionth of a second)
#else
		Sleep(1000);
#endif


	}

	if (m_NetworkMode == NETWORKMODE_SERVER)
	{

		for (int i = 0; i < 2; i++)
		{
			std::string data;
			UDPRecieveString(udpSocket, pairIp, data);

			std::cout << "Recieved message: " << data << std::endl;
		}

	}
	else if (m_NetworkMode == NETWORKMODE_CLIENT)
	{

		for (int i = 0; i < 2; i++)
		{
			std::string data;
			UDPRecieveString(udpSocket, pairIp, data);

			std::cout << "Recieved message: " << data << std::endl;
		}

		for (int i = 0; i < 10; i++)
		{

			UDPSendString(udpSocket, pairIp, "Hello SERVER");

#ifdef __linux__
			usleep(1000 * 1000);   // usleep takes sleep time in us (1 millionth of a second)
#else
			Sleep(1000);
#endif

		}

	}

	return true;

}

bool CNetwork::Connect(const std::string ipAddressString, int port)
{

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
            std::cout << "TCP Open failed: " << SDLNet_GetError();
            return false;
        }

        m_socketSet = SDLNet_AllocSocketSet(2);

        SDLNet_TCP_AddSocket(m_socketSet, m_Socket);

        // Wait for the client
        while (1)
        {

            m_ClientSocket = SDLNet_TCP_Accept(m_Socket);

#ifdef __linux__
			usleep(1000 * 1000);   // usleep takes sleep time in us (1 millionth of a second)
#else
			Sleep(1000);
#endif

            if (m_ClientSocket)
                break;

        }

        SDLNet_TCP_AddSocket(m_socketSet, m_ClientSocket);

    }
    else if (m_NetworkMode == NETWORKMODE_CLIENT)
    {

        IPaddress ip;

        if (SDLNet_ResolveHost(&ip, ipAddressString.c_str(), port) == SDL_ERROR)
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

