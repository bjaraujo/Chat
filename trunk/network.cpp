
#include <iostream>

#include "network.h"

CNetwork::CNetwork()
{

    m_NetworkMode = NETWORKMODE_LOCAL;

    m_udpSocket = NULL;

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

bool CNetwork::UDPSendString(IPaddress& ip, std::string data)
{

	UDPpacket packet;
	packet.address = ip;
	packet.channel = -1;
	packet.data = (Uint8*) data.c_str();
	packet.len = data.length();

	std::cout << "Sending: " << data << std::endl;

	int numSent = SDLNet_UDP_Send(m_udpSocket, packet.channel, &packet);

	if (!numSent) {
		std::cout << "SDLNet_UDP_Send: " << SDLNet_GetError() << std::endl;
		return false;
	}

	return true;

}

bool CNetwork::UDPRecieveString(IPaddress& ip, std::string& data)
{

	bool res = false;

	UDPpacket* packet;
	
	if (!(packet = SDLNet_AllocPacket(1024)))
	{
		std::cout << "SDLNet_AllocPacket: " << SDLNet_GetError() << std::endl;
		return false;
	}

	int numRecv = SDLNet_UDP_Recv(m_udpSocket, packet);

	if (numRecv) 
	{
		data = std::string((char*)packet->data, packet->len);
		res = true;
	}

	SDLNet_FreePacket(packet);
	
	return res;

}

bool CNetwork::Pair(std::string proxyIpAddressString, int proxyPortNum, std::string& pairIpAddress, int& pairPortNum)
{

	// create a UDPsocket
	m_udpSocket = SDLNet_UDP_Open(proxyPortNum);
	
	if (!m_udpSocket) {
		std::cout << "UDP open failed:" << SDLNet_GetError() << std::endl;
		return false;
	}

	IPaddress ipProxy;

	if (SDLNet_ResolveHost(&ipProxy, proxyIpAddressString.c_str(), proxyPortNum) == SDL_ERROR)
	{
		std::cout << "Connection failed: " << SDLNet_GetError() << std::endl;
		return false;
	}

	UDPSendString(ipProxy, "START");

	while (true)
	{

		if (m_NetworkMode == NETWORKMODE_SERVER)
			UDPSendString(ipProxy, "A");
		else if (m_NetworkMode == NETWORKMODE_CLIENT)
			UDPSendString(ipProxy, "B");

		std::string data;

		if (UDPRecieveString(ipProxy, data))
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

	UDPSendString(ipProxy, "END");

	if (SDLNet_ResolveHost(&m_ip, pairIpAddress.c_str(), pairPortNum) == SDL_ERROR)
	{
		std::cout << "Connection failed: " << SDLNet_GetError() << std::endl;
		return false;
	}

	for (int i = 0; i < 5; i++)
	{

		if (m_NetworkMode == NETWORKMODE_SERVER)
			UDPSendString(m_ip, "Hello CLIENT");
		else if (m_NetworkMode == NETWORKMODE_CLIENT)
			UDPSendString(m_ip, "Hello SERVER");

		std::string data;
		UDPRecieveString(m_ip, data);

		std::cout << "Recieved message: " << data << std::endl;

#ifdef __linux__
		usleep(1000 * 1000);   // usleep takes sleep time in us (1 millionth of a second)
#else
		Sleep(1000);
#endif

	}

	for (int i = 0; i < 20; i++)
	{

		std::string data;
		UDPRecieveString(m_ip, data);

		std::cout << "Recieved message: " << data << std::endl;

#ifdef __linux__
		usleep(250 * 1000);   // usleep takes sleep time in us (1 millionth of a second)
#else
		Sleep(250);
#endif

	}

	return true;

}


bool CNetwork::Send(const std::string data)
{

	return UDPSendString(m_ip, data);

}

bool CNetwork::Receive(std::string& data)
{

	return UDPRecieveString(m_ip, data);

}

