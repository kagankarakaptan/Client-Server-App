#include <iostream>
#include <WS2tcpip.h>
#include <fstream>


// Include the Winsock library (lib) file
#pragma comment (lib, "ws2_32.lib")

// Saves us from typing std::cout << etc. etc. etc.
using namespace std;

// Main entry point into the server
void main()
{
	////////////////////////////////////////////////////////////
	// INITIALIZE WINSOCK
	////////////////////////////////////////////////////////////

	// Structure to store the WinSock version. This is filled in
	// on the call to WSAStartup()
	WSADATA data;

	// To start WinSock, the required version must be passed to
	// WSAStartup(). This server is going to use WinSock version
	// 2 so I create a word that will store 2 and 2 in hex i.e.
	// 0x0202
	WORD version = MAKEWORD(2, 2);

	// Start WinSock
	int wsOk = WSAStartup(version, &data);
	if (wsOk != 0)
	{
		// Not ok! Get out quickly
		cout << "Can't start Winsock! " << wsOk;
		return;
	}

	////////////////////////////////////////////////////////////
	// SOCKET CREATION AND BINDING
	////////////////////////////////////////////////////////////

	// Create a socket, notice that it is a user datagram socket (UDP)
	SOCKET socketReyiz = socket(AF_INET, SOCK_DGRAM, 0);

	// Create a server hint structure for the server
	sockaddr_in server;
	server.sin_addr.S_un.S_addr = ADDR_ANY; // Us any IP address available on the machine
	server.sin_family = AF_INET; // Address format is IPv4
	server.sin_port = htons(54000); // Convert from little to big endian
	ofstream myWriteFile;

	// Try and bind the socket to the IP and port
	if (bind(socketReyiz, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		int WSAerror = WSAGetLastError();
		myWriteFile.open("BindLogs.txt");
		myWriteFile << "Can't bind socket! " << WSAerror << endl;
		myWriteFile.close();

	}

	////////////////////////////////////////////////////////////
	// MAIN LOOP SETUP AND ENTRY
	////////////////////////////////////////////////////////////

	sockaddr_in client; // Use to hold the client information (port / ip address)
	//sockaddr_in client2;
	int clientLength = sizeof(client); // The size of the client information
	//int clientLength2 = sizeof(client2);

	char buf[1024];




	// Enter a loop
	while (true)
	{
		ZeroMemory(&client, clientLength); // Clear the client structure
		ZeroMemory(buf, 1024); // Clear the receive buffer

		// Wait for message
		int bytesIn = recvfrom(socketReyiz, buf, 1024, 0, (sockaddr*)&client, &clientLength);
		if (bytesIn == SOCKET_ERROR)
		{
			cout << "Error receiving from client " << WSAGetLastError() << endl;
			continue;
		}

		// Display message and client info
		char clientIp[256]; // Create enough space to convert the address byte array //GET IP!
		ZeroMemory(clientIp, 256); // to string of characters
		//// Convert from byte array to chars
		inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);

		string s = buf;

		int sendOk = sendto(socketReyiz, s.c_str(), s.size() + 1, 0, (sockaddr*)&client, sizeof(client));
		if (sendOk == SOCKET_ERROR)
		{
			cout << "That didn't work! " << WSAGetLastError() << endl;
		}

		// Display the message / who sent it
		cout << "Message received from " << clientIp << " : " << buf << endl << " and sended back to " << (sockaddr*)&client << endl << endl;
	}

	// Close socket
	closesocket(socketReyiz);

	// Shutdown winsock
	WSACleanup();
}
