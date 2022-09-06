#include "NetworkBase.h"
#include <enet/enet.h>
#include <iostream>

NetworkBase::NetworkBase() 
{
	netHost	= nullptr;
}

NetworkBase::~NetworkBase() 
{
	Destroy();
}

bool NetworkBase::Initialise(uint16_t port, size_t maxPeers)
{
	//enet_initialize();

	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = port;

	netHost = enet_host_create(
		(port == 0) ? NULL : &address,	// address
		maxPeers,						// peerCount
		1,								// ChannelLimit
		0,								// Downstream bandwidth of the host in bytes/second; if 0, ENet will assume unlimited bandwidth.
		0);								// Upstream bandwidth of the host in bytes/second; if 0, ENet will assume unlimited bandwidth.

	if (!netHost)
	{
		std::cout << "Unable to initialise Network Host!" << std::endl;
		return false;
	}

	return true;
}

void NetworkBase::Destroy()
{
	if (netHost)
	{
		enet_host_destroy(netHost);
		netHost = nullptr;
	}
}

ENetPeer* NetworkBase::ConnectPeer(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint16_t port)
{
	if (netHost)
	{
		ENetAddress address;
		address.port = port;

		address.host = (ip4 << 24) | (ip3 << 16) | (ip2 << 8) | (ip1);

		ENetPeer* peer = enet_host_connect(netHost, &address, 2, 0);

		if (!peer)
		{
			std::cout << "Unable to connect to peer!" << std::endl;
		}

		return peer;
	}
	else
	{
		std::cout << "Unable to connect to peer!" << std::endl;
		return NULL;
	}
}

// Process incoming packets and send outgoing packets
void NetworkBase::PacketProcesser(std::function<void(const ENetEvent&)> callback, float dt)
{
	if (netHost)
	{
		//Handle all incoming packets & send any packets awaiting dispatch
		ENetEvent event;
		while (enet_host_service(netHost, &event, 0) > 0)
		{
			callback(event);
		}

		//Update Transmit / Recieve bytes per second
		m_SecondTimer += dt;
		if (m_SecondTimer >= 1.0f)
		{
			m_SecondTimer = 0.0f;

			m_IncomingKb = float(netHost->totalReceivedData / 128.0); // - 8 bits in a byte and 1024 bits in a KiloBit
			m_OutgoingKb = float(netHost->totalSentData / 128.0);
			//std::cout << "Transmit rate: " << m_IncomingKb << " / " << "Recieve rate: " << m_OutgoingKb << std::endl;
			netHost->totalReceivedData = 0;
			netHost->totalSentData = 0;
		}

	}
	else
	{
		std::cout << "Unable to service network: Network not initialized!" << std::endl;
	}

}