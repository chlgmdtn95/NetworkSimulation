#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

#include <fstream>

using namespace ns3;

class MyServer : public Application
{
    public :
        MyServer(uint16_t port);
        virtual ~MyServer();

        void    Setup(Ptr<Socket> socket, uint16_t port);

    private :
        virtual void    StartApplication(void);
        virtual void    StopApplication(void);

        void forCallback(Ptr<Socket> socket);

        uint16_t    m_port;
        Ptr<Socket> m_socket;
        Address     m_local;
        uint16_t    m_receivedPackets;
};

void    MyServer::Setup(Ptr<Socket> socket, uint16_t port)
{
    m_socket = socket;
    m_port = port;
}

MyServer::MyServer(uint16_t port)
{
    this->m_port = port;
    this->m_socket = nullptr;
    this->m_receivedPackets = 0;
    this->m_local = InetSocketAddress(Ipv4Address::GetAny(), port);
}

MyServer::~MyServer()
{
    m_socket = nullptr;
}

void    MyServer::forCallback(Ptr<Socket> socket)
{
    Ptr<Packet> packet;
    Address from;

    while((packet = socket->RecvFrom(from)))
    {
        if(InetSocketAddress::IsMatchingType(from))
        {
            NS_LOG_UNCOND(Simulator::Now().GetSeconds() << "\t" << ++m_receivedPackets);
        }
    }
}

void    MyServer::StartApplication()
{
    m_socket->Bind(m_local);
    this->m_socket->SetRecvCallback(MakeCallback(&MyServer::forCallback, this));
}

void    MyServer::StopApplication(void)
{}