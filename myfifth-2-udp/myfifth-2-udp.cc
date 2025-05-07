/*
 * SPDX-License-Identifier: GPL-2.0-only
 */

// #include "tutorial-app.h"

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

#include "myserver.h"

#include <fstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("FifthScriptExample");

class Myapp: public Application
{
    public:
        Myapp();
        virtual ~Myapp();

        void Setup(Ptr<Socket> socket,
                    Address address,
                    uint32_t packetSize,
                    uint32_t nPackets,
                    DataRate dataRate
        );
    
    private:
        virtual void StartApplication(void);
        virtual void StopApplication(void);

        Ptr<ExponentialRandomVariable> rng_value;

        void    ScheduleTx(void);
        void    SendPacket(void);

        Ptr<Socket> m_socket;
        Address     m_peer;
        uint32_t    m_packetSize;
        uint32_t    m_nPackets;
        DataRate    m_dataRate;
        EventId     m_sendEvent;
        bool        m_running;
        uint32_t    m_packetsSent;
};

void    Myapp::StartApplication(void)
{
    m_running = true;
    m_packetsSent = 0;
    m_socket->Bind();
    m_socket->Connect(m_peer);
    SendPacket();
}

void    Myapp::SendPacket(void)
{
    Ptr<Packet> packet = Create<Packet> (m_packetSize);
    m_socket->Send(packet);
    if (++m_packetsSent < m_nPackets)
        ScheduleTx();
}

Myapp::Myapp() : m_socket(nullptr),
                    m_peer(),
                    m_packetSize(0),
                    m_nPackets(0),
                    m_dataRate(0),
                    m_sendEvent(),
                    m_running(false),
                    m_packetsSent(0)
{
    rng_value = CreateObject<ExponentialRandomVariable> ();
}

void    Myapp::ScheduleTx(void)
{
    if (m_running)
    {
        Time    tNext (
            // Seconds(m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate()))
            rng_value->GetValue(0.001778, 0.0)
        );
        // NS_LOG_UNCOND("send packet");
        m_sendEvent = Simulator::Schedule(tNext, &Myapp::SendPacket, this);
    }
}

Myapp::~Myapp()
{
    m_socket = nullptr;
}

void    Myapp::StopApplication(void)
{
    m_running = false;

    if (m_sendEvent.IsPending())
    {
        Simulator::Cancel(m_sendEvent);
    }

    if (m_socket)
    {
        m_socket->Close();
    }
}

void    Myapp::Setup(Ptr<Socket> socket,
    Address address,
    uint32_t packetSize,
    uint32_t nPackets,
    DataRate dataRate)
{
    m_socket = socket;
    m_peer = address;
    m_packetSize = packetSize;
    m_nPackets = nPackets;
    m_dataRate = dataRate;    
}

// ===========================================================================
//
//         node 0                 node 1
//   +----------------+    +----------------+
//   |    ns-3 TCP    |    |    ns-3 TCP    |
//   +----------------+    +----------------+
//   |    10.1.1.1    |    |    10.1.1.2    |
//   +----------------+    +----------------+
//   | point-to-point |    | point-to-point |
//   +----------------+    +----------------+
//           |                     |
//           +---------------------+
//                5 Mbps, 2 ms
//
//
// We want to look at changes in the ns-3 TCP congestion window.  We need
// to crank up a flow and hook the CongestionWindow attribute on the socket
// of the sender.  Normally one would use an on-off application to generate a
// flow, but this has a couple of problems.  First, the socket of the on-off
// application is not created until Application Start time, so we wouldn't be
// able to hook the socket (now) at configuration time.  Second, even if we
// could arrange a call after start time, the socket is not public so we
// couldn't get at it.
//
// So, we can cook up a simple version of the on-off application that does what
// we want.  On the plus side we don't need all of the complexity of the on-off
// application.  On the minus side, we don't have a helper, so we have to get
// a little more involved in the details, but this is trivial.
//
// So first, we create a socket and do the trace connect on it; then we pass
// this socket into the constructor of our simple application which we then
// install in the source node.
// ===========================================================================
//

// /**
//  * Congestion window change callback
//  *
//  * @param oldCwnd Old congestion window.
//  * @param newCwnd New congestion window.
//  */
// static void
// CwndChange(uint32_t oldCwnd, uint32_t newCwnd)
// {
//     NS_LOG_UNCOND(Simulator::Now().GetSeconds() << "\t" << newCwnd);
// }

/**
 * Rx drop callback
 *
 * @param p The dropped packet.
 */
static void
RxDrop(Ptr<const Packet> p)
{
    NS_LOG_UNCOND("RxDrop at " << Simulator::Now().GetSeconds());
}

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    // In the following three lines, TCP NewReno is used as the congestion
    // control algorithm, the initial congestion window of a TCP connection is
    // set to 1 packet, and the classic fast recovery algorithm is used. Note
    // that this configuration is used only to demonstrate how TCP parameters
    // can be configured in ns-3. Otherwise, it is recommended to use the default
    // settings of TCP in ns-3.
    // Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::TcpNewReno"));
    // Config::SetDefault("ns3::TcpSocket::InitialCwnd", UintegerValue(1));
    // Config::SetDefault("ns3::TcpL4Protocol::RecoveryType",
    //                    TypeIdValue(TypeId::LookupByName("ns3::TcpClassicRecovery")));

    NodeContainer nodes;
    nodes.Create(2);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer devices;
    devices = pointToPoint.Install(nodes);

    Ptr<RateErrorModel> em = CreateObject<RateErrorModel>();
    em->SetAttribute("ErrorRate", DoubleValue(0.00001));
    devices.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(em));

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.252");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    uint16_t serverPort = 8080;
    Address serverAddress(InetSocketAddress(interfaces.GetAddress(1), serverPort));

    Ptr<MyServer> myServer = CreateObject<MyServer> (serverPort);
    Ptr<Socket> udpServerSocket = Socket::CreateSocket(nodes.Get(1), UdpSocketFactory::GetTypeId());
    myServer->Setup(udpServerSocket, serverPort);
    nodes.Get(1)->AddApplication(myServer);
    myServer->SetStartTime(Seconds(1.));
    myServer->SetStopTime(Seconds(20.));
    // PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory",
    //                                   InetSocketAddress(Ipv4Address::GetAny(), sinkPort));
    // ApplicationContainer sinkApps = packetSinkHelper.Install(nodes.Get(1));
    // sinkApps.Start(Seconds(0.));
    // sinkApps.Stop(Seconds(20.));
    
    Ptr<Socket> ns3UdpSocket = Socket::CreateSocket(nodes.Get(0), UdpSocketFactory::GetTypeId());

    Ptr<Myapp> app = CreateObject<Myapp>();
    app->Setup(ns3UdpSocket, serverAddress, 1000, 1000, DataRate("4.5Mbps"));
    nodes.Get(0)->AddApplication(app);
    app->SetStartTime(Seconds(1.));
    app->SetStopTime(Seconds(20.));

    devices.Get(1)->TraceConnectWithoutContext("PhyRxDrop", MakeCallback(&RxDrop));

    Simulator::Stop(Seconds(20));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
