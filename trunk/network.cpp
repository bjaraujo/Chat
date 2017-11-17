
#include <stdlib.h>
#include <iostream>

#include "network.h"

CNetwork::CNetwork()
{

    m_NetworkMode = NETWORKMODE_HOST;
    m_udpSocket = NULL;

}

CNetwork::~CNetwork(void)
{


}

void CNetwork::Sleep(const int ms)
{

#ifdef __linux__
    usleep(ms * 1000);   // usleep takes sleep time in us (1 millionth of a second)
#else
    ::Sleep(ms);
#endif

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
    packet.data = (Uint8*)data.c_str();
    packet.len = data.length();

    //std::cout << "Sending: " << data << std::endl;

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

        if (m_NetworkMode == NETWORKMODE_HOST)
            UDPSendString(ipProxy, "A");
        else if (m_NetworkMode == NETWORKMODE_CLIENT)
            UDPSendString(ipProxy, "B");

        std::string data;

        if (UDPRecieveString(ipProxy, data))
        {

            int pos = data.find(":");

            if (pos != std::string::npos) {

                pairIpAddress = data.substr(0, pos);
                pairPortNum = atoi(data.substr(pos + 1, data.length() - pos - 1).c_str());

                std::cout << "Pair ip address: " << data << std::endl;
                break;
            }

        }

        this->Sleep(1000);

    }

    UDPSendString(ipProxy, "END");

    return true;

}

bool CNetwork::Connect(const std::string pairIpAddress, const int pairPortNum)
{

    if (SDLNet_ResolveHost(&m_ip, pairIpAddress.c_str(), pairPortNum) == SDL_ERROR)
    {
        std::cout << "Connection failed: " << SDLNet_GetError() << std::endl;
        return false;
    }

    for (int i = 0; i < 5; i++)
    {

        UDPSendString(m_ip, "ACK");

        std::string data;
        UDPRecieveString(m_ip, data);

        this->Sleep(500);

    }

    this->Sleep(1000);

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

