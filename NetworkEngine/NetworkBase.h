#pragma once
#include <winsock2.h>
#include <enet/enet.h>
#include <functional>

class NetworkBase
{
public:
	NetworkBase();
	~NetworkBase();

	// Initialze network.
	bool Initialise(uint16_t portNum= 0, size_t maxPeers = 1);

	// Destroy host.
	void Destroy();

	// Build connection of peer for client.
	ENetPeer* ConnectPeer(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint16_t port);

	// Process incoming packets and send outgoing packet.
	void PacketProcesser(std::function<void(const ENetEvent&)> callback, float dt);
	
	// An ENet host for communicating with peers.
	ENetHost* netHost;

	//Profilling Information
	//	- Incoming and Outgoing Kilobytes transmitted over the last second
	float m_OutgoingKb;
	float m_IncomingKb;

	//Used to keep track of profilling transmit rate
	float m_SecondTimer;
};