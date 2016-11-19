
#include <conio.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <string>

#include "anyoption.h"
#include "aes256.hpp"

#include "network.h"

vector<std::string> split(const std::string& str, const std::string& delimiters) {
	
	std::vector<std::string> v;
	std::string::size_type start = 0;
	auto pos = str.find_first_of(delimiters, start);
	while (pos != std::string::npos) {
		if (pos != start) // ignore empty tokens
			v.emplace_back(str, start, pos - start);
		start = pos + 1;
		pos = str.find_first_of(delimiters, start);
	}
	if (start < str.length()) // ignore trailing delimiter
		v.emplace_back(str, start, str.length() - start); // add what's left of the string
	return v;

}

int main(int argc, char **argv)
{

	AnyOption opt;

	opt.addUsage("");
	opt.addUsage("Usage: ");
	opt.addUsage("");
	opt.addUsage(" -h  --help              Prints this help ");
	opt.addUsage(" -s  --server            Starts as server");
	opt.addUsage(" -c  --client [ip:port]  Starts as client");
	opt.addUsage(" -n  --nick [nick]       Chat nick name");
	opt.addUsage("");

	opt.setFlag("help", 'h');
	opt.setFlag("server", 's');
	opt.setOption("client", 'c');
	opt.setOption("nick", 'n');

	opt.processCommandArgs(argc, argv);

	if (!opt.hasOptions()) {
		opt.printUsage();
		return 0;
	}

	if (opt.getFlag("help") || opt.getFlag('h'))
		opt.printUsage();

	const int aPortNum = 1234;

	const int aPasswordLength = 8;
	char aPassword[aPasswordLength + 1];
	char *aNickName = NULL;

	srand((unsigned int)time(NULL));

	CNetwork Network;

	if (opt.getValue("nick") != NULL || opt.getValue('n') != NULL)
	{
		aNickName = opt.getValue("nick") != NULL ? opt.getValue("nick") : opt.getValue("n");
		std::cout << "nick = " << aNickName << endl;
	}

	if (opt.getFlag("server") || opt.getFlag('s'))
	{
		std::cout << "*** STARTING AS SERVER" << std::endl;
		Network.SetNetworkMode(NETWORKMODE_SERVER);

		std::cout << "Password: ";
		for (int i = 0; i < aPasswordLength; i++)
		{
			if (i % 2 == 0)
				aPassword[i] = 0x41 + rand() % 26;
			else
				aPassword[i] = 0x30 + rand() % 10;

			std::cout << aPassword[i];
		}

		std::cout << std::endl;

		std::cout << "Waiting for client to connect..." << std::endl;

		if (!Network.Connect("", aPortNum))
		{
			std::cout << "Unable to connect!" << std::endl;
			return -1;
		}

	}
	else if (opt.getValue("client") || opt.getValue('c'))
	{
		std::cout << "*** STARTING AS CLIENT" << std::endl;
		Network.SetNetworkMode(NETWORKMODE_CLIENT);

		char* anIpAddressPort = opt.getValue("client") != NULL ? opt.getValue("client") : opt.getValue("c");

		std::cout << "Connecting to: " << anIpAddressPort << std::endl;

		std::vector<std::string> vecIpAddressPort = split(anIpAddressPort, ":");

		if (!Network.Connect(vecIpAddressPort[0].c_str(), stoi(vecIpAddressPort[1])))
		{
			std::cout << "Unable to connect!" << std::endl;
			return -1;
		}
	}

	if (aNickName == NULL)
		std::cout << "Error no nickname!" << std::endl;

	int len = 0;

	char anotherNickName[80];

	if (Network.NetworkMode() == NETWORKMODE_SERVER)
	{
		std::cout << "Successfully connected to client." << std::endl;

		Network.Send(SOCKET_CLIENT, aNickName, 80);
		Network.Receive(SOCKET_CLIENT, &anotherNickName[0], 80);
		std::cout << "Talking to: " << anotherNickName << std::endl;

	}
	else if (Network.NetworkMode() == NETWORKMODE_CLIENT)
	{
		std::cout << "Successfully connected to server." << std::endl;

		Network.Send(SOCKET_SERVER, aNickName, 80);
		Network.Receive(SOCKET_SERVER, &anotherNickName[0], 80);
		std::cout << "Talking to: " << anotherNickName << std::endl;

		char line[256];

		cout << "Enter password: ";
		cin.get(line, 256);

		for (int i = 0; i < aPasswordLength; i++)
			aPassword[i] = line[i];

	}

	ByteArray key;

	for (int i = 0; i < aPasswordLength; i++)
		key.push_back(aPassword[i]);

	Aes256 aes(key);

	char sendBuffer[512];
	char recieveBuffer[512];

	std::cout << "> ";

	while (true) {

		// Send message
		if (_kbhit())
		{
			char cur = _getch();

			if (cur == 8)
			{
				if (len > 0)
				{
					std::cout << cur;
					cout << " ";
					std::cout << cur;
					len--;
				}
			}
			else
			{
				std::cout << cur;


			}

			if (cur != 13 && len < 511)
			{

				if (cur != 8)
					sendBuffer[len++] = cur;

			}
			else
			{

				sendBuffer[len++] = '\0';

				std::cout << std::endl;

				ByteArray sendBufferEnc;
				aes.encrypt(key, (unsigned char*)sendBuffer, len, sendBufferEnc);

				if (Network.NetworkMode() == NETWORKMODE_SERVER)
					Network.Send(SOCKET_CLIENT, (char *)sendBufferEnc.data(), sendBufferEnc.size());
				else if (Network.NetworkMode() == NETWORKMODE_CLIENT)
					Network.Send(SOCKET_SERVER, (char *)sendBufferEnc.data(), sendBufferEnc.size());

				len = 0;

				std::cout << "> ";

			}

		}

		#ifdef LINUX
				usleep(20 * 1000);   // usleep takes sleep time in us (1 millionth of a second)
		#endif
		#ifdef WINDOWS
				Sleep(20);
		#endif

		// Recieve messages
		int Received = 0;
		int Bufsize = 512;

		do {

			if (Network.NetworkMode() == NETWORKMODE_SERVER)
				Received += Network.ReceiveNonBlocking(SOCKET_CLIENT, &recieveBuffer[Received], Bufsize);
			else if (Network.NetworkMode() == NETWORKMODE_CLIENT)
				Received += Network.ReceiveNonBlocking(SOCKET_SERVER, &recieveBuffer[Received], Bufsize);

			if (Received == SDL_ERROR)
			{
				std::cout << "Recieve error: " << SDLNet_GetError();
				break;
			}

			if (Received > 0)
				Bufsize -= Received;
			else
				break;

		} while (Bufsize > 0);

		if (Received > 0)
		{
			std::cout << std::endl;

			ByteArray recvBufferEnc;
			aes.decrypt(key, (unsigned char*)recieveBuffer, Received, recvBufferEnc);

			if (Network.NetworkMode() == NETWORKMODE_SERVER)
				std::cout << anotherNickName << ": " << recvBufferEnc.data() << std::endl;
			else if (Network.NetworkMode() == NETWORKMODE_CLIENT)
				std::cout << anotherNickName << ": " << recvBufferEnc.data() << std::endl;

			// Beep
			std::cout << '\a';

			std::cout << "> ";

			// Add current buffer
			for (int i = 0; i < len; i++)
				std::cout << sendBuffer[i];

		}

	};

	return 0;

}