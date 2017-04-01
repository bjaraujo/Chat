
#include <conio.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <string>

#include "anyoption.h"
#include "aes256.hpp"

#include "network.h"

int main(int argc, char **argv)
{

	AnyOption opt;

	opt.addUsage("");
	opt.addUsage("Usage: ");
	opt.addUsage("");
	opt.addUsage(" -h  --help              Prints this help ");
	opt.addUsage(" -s  --server            Starts as server");
	opt.addUsage(" -c  --client            Starts as client");
	opt.addUsage(" -n  --nick [nick]       Chat nick name");
	opt.addUsage("");

	opt.setFlag("help", 'h');
	opt.setFlag("server", 's');
	opt.setFlag("client", 'c');
	opt.setOption("nick", 'n');

	opt.processCommandArgs(argc, argv);

	if (!opt.hasOptions()) {
		opt.printUsage();
		return 0;
	}

	if (opt.getFlag("help") || opt.getFlag('h'))
		opt.printUsage();

	const int passwordLength = 8;
	char password[passwordLength + 1];
	std::string nickName;

	srand((unsigned int)time(NULL));

	CNetwork Network;

	if (opt.getValue("nick") != NULL || opt.getValue('n') != NULL)
	{
		nickName = std::string(opt.getValue("nick") != NULL ? opt.getValue("nick") : opt.getValue("n"));
		std::cout << "nick = " << nickName << endl;
	}

	Network.Initialize();

	// Pair client and server
	const std::string proxyIpAddress = "80.79.23.114";
	const int proxyPortNum = 1234;

	std::string pairIpAddress;
	int pairPortNum;

	if (opt.getFlag("server") || opt.getFlag('s'))
	{
		std::cout << "*** STARTING AS HOST" << std::endl;
		Network.SetNetworkMode(NETWORKMODE_HOST);
	}
	else if (opt.getValue("client") || opt.getValue('c'))
	{
		std::cout << "*** STARTING AS CLIENT" << std::endl;
		Network.SetNetworkMode(NETWORKMODE_CLIENT);
	}

	Network.Pair(proxyIpAddress, proxyPortNum, pairIpAddress, pairPortNum);

	Network.Connect(pairIpAddress, pairPortNum);

	if (Network.NetworkMode() == NETWORKMODE_HOST)
	{
		std::cout << "Password: ";
		for (int i = 0; i < passwordLength; i++)
		{
			if (i % 2 == 0)
				password[i] = 0x41 + rand() % 26;
			else
				password[i] = 0x30 + rand() % 10;

			std::cout << password[i];
		}

		std::cout << std::endl;

		std::cout << "Waiting for client to connect..." << std::endl;
	}

	std::cout << "Successfully connected to " << pairIpAddress << ":" << pairPortNum << std::endl;

	if (nickName.empty())
		std::cout << "Error no nickname!" << std::endl;

	// Sync
	std::string pairNickName;

	while (true)
	{
		Network.Send("NICK:" + nickName);

		std::string data;
		Network.Receive(data);

		if (!data.empty())
		{
			int pos = data.find("NICK:");

			if (pos != std::string::npos)
			{
				pairNickName = data.substr(5, data.length() - 5);
				break;
			}
		}

		Network.Sleep(1000);

	}

	// Flush receive buffer
	for (int i = 0; i < 6; i++)
	{

		std::string data;
		Network.Receive(data);

		Network.Sleep(500);

	}

	std::cout << "Talking to: " << pairNickName << std::endl;

	if (Network.NetworkMode() == NETWORKMODE_CLIENT)
	{

		char line[256];

		cout << "Enter password: ";
		cin.get(line, 256);

		for (int i = 0; i < passwordLength; i++)
			password[i] = line[i];

	}

	ByteArray key;

	for (int i = 0; i < passwordLength; i++)
		key.push_back(password[i]);

	Aes256 aes(key);

	int len = 0;
	char sendBuffer[512];

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

				Network.Send(std::string((char*)sendBufferEnc.data()));

				len = 0;

				std::cout << "> ";
			}

		}

		Network.Sleep(20);

		// Recieve messages
		std::string recieveBuffer;

		if (Network.Receive(recieveBuffer))
		{
			std::cout << std::endl;

			ByteArray recvBufferEnc;
			
			if (aes.decrypt(key, (unsigned char*)recieveBuffer.c_str(), recieveBuffer.length(), recvBufferEnc) > 0)
			{
				std::cout << pairNickName << ": " << recvBufferEnc.data() << std::endl;
			}

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