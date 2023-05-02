// Network Topology
//
//        10.1.1.0  255.255.255.0
// n0 -------------- r1 -------------- n1
//    point-to-point    point-to-point
//

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "../src/strict-priority-queue.cc"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SPQSimulation");

int 
main () 
{
    uint16_t node1PortA = 9000;     // high priority 
    uint16_t node1PortB = 9500;     // low priority
    uint16_t routerPort = 8000;
    uint32_t tcp_adu_size = 946;

    uint32_t DEFAULT_DATA_BYTES = 1073741824;  // 0.1 GB

    double DEFAULT_START_TIME = 0.0;
    double DEFAULT_END_TIME = 100000000.0;

    double appAStartTime = DEFAULT_START_TIME + 10000000.0;      // start later than B
    double appAEndTime = DEFAULT_END_TIME;
    double appBStartTime = DEFAULT_START_TIME;
    double appBEndTime = DEFAULT_END_TIME;

    uint32_t DEFAULT_QUEUE_SIZE = 655350000;

    uint32_t queueNumber = 2;
    std::vector<std::vector<uint32_t, uint16_t>> priorityPortList;
    priorityPortList.push_back({0, node1PortA});
    priorityPortList.push_back({1, node1PortB});

    // TODO: allow setting by config file

    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer nodes;
    nodes.Create(3);

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("40Mbps"));
    NetDeviceContainer devices1 = p2p.Install(nodes.Get(0), nodes.Get(1));

    p2p.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    NetDeviceContainer devices2 = p2p.Install(nodes.Get(1), nodes.Get(2));

    // set spq to R1
    StrictPriorityQueue spqR1(queueNumber, priorityPortList);
    // TODO: set up the queue, traffic class ? filter?
    Ptr<PointToPointNetDevice> devR1 = devices2.Get(0);
    devR1->SetQueue(spqR1);

    // Create the Internet stacks
    InternetStackHelper stack;
    stack.Install(nodes);

    // Create addresses
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interface1 = address.Assign(devices1);
    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer interface2 = address.Assign(devices2);
    
    // // TODO: delete.  Print IP address
    std::cout << "Node 1 IP address: " << interface1.GetAddress(0) << std::endl;
    std::cout << "Node 2 IP address: " << interface1.GetAddress(1) << std::endl;
    std::cout << "Node 2 IP address: " << interface2.GetAddress(0) << std::endl;
    std::cout << "Node 3 IP address: " << interface2.GetAddress(1) << std::endl;

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    
    // Create applications

    // Create a FTP applicationA and install it on node0
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(tcp_adu_size));
    AddressValue node1Address(InetSocketAddress(interface2.GetAddress(1), node1PortA));  //receiver node1's address
    BulkSendApplication ftpA("ns3::TcpSocketFactory", Address());
    ftpA.SetAttribute("Remote", node1Address);
    ftpA.SetAttribute("SendSize", UintegerValue(tcp_adu_size));
    ftpA.SetAttribute("MaxBytes", UintegerValue(DEFAULT_DATA_BYTES));
    ApplicationContainer ftpAppContA = ftpA.Install(nodes.Get(0));
    ftpAppContA.Start(Seconds(appAStartTime));
    ftpAppContA.End(Seconds(appAEndTime));

    // Create FTP applciation B and install it on node0
    AddressValue node1Address(InetSocketAddress(interface2.GetAddress(1), node1PortB));  //receiver node1's address
    BulkSendApplication ftpB("ns3::TcpSocketFactory", Address());
    ftpB.SetAttribute("Remote", node1Address);
    ftpB.SetAttribute("SendSize", UintegerValue(tcp_adu_size));
    ftpB.SetAttribute("MaxBytes", UintegerValue(DEFAULT_DATA_BYTES));
    ApplicationContainer ftpAppContB = ftpB.Install(nodes.Get(0));
    ftpAppContB.Start(Seconds(appBStartTime));
    ftpAppContB.End(Seconds(appBEndTime));

    // Create a packet sinkc application A and install it on node1
    PacketSinkHelper sinkA("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny(), node1PortA));
    ApplicationContainer sinkAppContA = sinkA.Install(nodes.Get(2));
    sinkAppContA.Start(Seconds(DEFAULT_START_TIME));
    sinkAppContA.End(Seconds(DEFAULT_END_TIME));

    // Create a packet sinkc application B and install it on node1
    PacketSinkHelper sinkB("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny(), node1PortB));
    ApplicationContainer sinkAppContB = sinkB.Install(nodes.Get(2));
    sinkAppContB.Start(Seconds(DEFAULT_START_TIME));
    sinkAppContB.End(Seconds(DEFAULT_END_TIME));


    // // TODO: set client server port
    UdpEchoServerHelper echoServer(serverPort);

    ApplicationContainer serverApps = echoServer.Install(nodes.Get(2));
    serverApps.Start(Seconds(DEFAULT_START_TIME));
    serverApps.Stop(Seconds(DEFAULT_END_TIME));

    UdpEchoClientHelper echoClient(interface2.GetAddress(1), serverPort);
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientAppsA = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));
    
    


    Simulator::Run();
    Simulator::Destroy();
    return 0;

}