// Network Topology
//
//        10.1.1.0  255.255.255.0
// n0 -------------- r1 -------------- n1
//    point-to-point    point-to-point
//
#include "ns3/log.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "../src/strict-priority-queue.cc"
#include <cstdint>


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SPQSimulation");

Ptr<PointToPointNetDevice> NetDeviceDynamicCast (Ptr<NetDevice> const&p)
{
  return Ptr<PointToPointNetDevice> (dynamic_cast<PointToPointNetDevice *> (PeekPointer (p)));
}

int 
main () 
{
    uint16_t node1PortA = 9000;     // high priority 
    uint16_t node1PortB = 9500;     // low priority
    uint16_t routerPort = 8000;
    uint32_t tcp_adu_size = 946;

    uint32_t DEFAULT_DATA_BYTES = 1073741824;  // 0.1
    double DEFAULT_START_TIME = 0.0;
    double DEFAULT_END_TIME = 100000000.0;

    double appAStartTime = DEFAULT_START_TIME + 10000000.0;      // start later than B
    double appAEndTime = DEFAULT_END_TIME;
    double appBStartTime = DEFAULT_START_TIME;
    double appBEndTime = DEFAULT_END_TIME;

    uint32_t DEFAULT_QUEUE_SIZE = 655350000;

    uint32_t queueNumber = 2;
    std::vector<std::pair<uint32_t, uint16_t>> priorityPortList;
    priorityPortList.push_back(std::make_pair(0, node1PortA));
    priorityPortList.push_back(std::make_pair(1, node1PortB));

    // TODO: allow setting by config file

    Time::SetResolution(Time::NS);
    // LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    // LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer nodes;
    nodes.Create(3);

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("40Mbps"));
    NetDeviceContainer devices1 = p2p.Install(nodes.Get(0), nodes.Get(1));

    p2p.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    NetDeviceContainer devices2 = p2p.Install(nodes.Get(1), nodes.Get(2));

    // set spq to R1
    //Ptr<StrictPriorityQueue> spqR1 = &StrictPriorityQueue(queueNumber, priorityPortList);
    StrictPriorityQueue* spqR1obj = new StrictPriorityQueue(queueNumber, priorityPortList);
    Ptr<StrictPriorityQueue> spqR1 = CreateObject<StrictPriorityQueue>(queueNumber, priorityPortList);
    // TODO: set up the queue, traffic class ? filter?
    Ptr<PointToPointNetDevice> devR1 = NetDeviceDynamicCast(devices2.Get(0));
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
    BulkSendHelper ftpA ("ns3::TcpSocketFactory", Address());
    ftpA.SetAttribute("Remote", node1Address);
    ftpA.SetAttribute("SendSize", UintegerValue(tcp_adu_size));
    ftpA.SetAttribute("MaxBytes", UintegerValue(DEFAULT_DATA_BYTES));
    ApplicationContainer ftpAppContA = ftpA.Install(nodes.Get(0));
    ftpAppContA.Start(Seconds(appAStartTime));
    ftpAppContA.Stop(Seconds(appAEndTime));

    // Create FTP applciation B and install it on node0
    AddressValue node1Bddress(InetSocketAddress(interface2.GetAddress(1), node1PortB));  //receiver node1's address
    BulkSendHelper ftpB("ns3::TcpSocketFactory", Address());
    ftpB.SetAttribute("Remote", node1Address);
    ftpB.SetAttribute("SendSize", UintegerValue(tcp_adu_size));
    ftpB.SetAttribute("MaxBytes", UintegerValue(DEFAULT_DATA_BYTES));
    ApplicationContainer ftpAppContB = ftpB.Install(nodes.Get(0));
    ftpAppContB.Start(Seconds(appBStartTime));
    ftpAppContB.Stop(Seconds(appBEndTime));

    // Create a packet sinkc application A and install it on node1
    PacketSinkHelper sinkA("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny(), node1PortA));
    ApplicationContainer sinkAppContA = sinkA.Install(nodes.Get(2));
    sinkAppContA.Start(Seconds(DEFAULT_START_TIME));
    sinkAppContA.Stop(Seconds(DEFAULT_END_TIME));

    // Create a packet sinkc application B and install it on node1
    PacketSinkHelper sinkB("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny(), node1PortB));
    ApplicationContainer sinkAppContB = sinkB.Install(nodes.Get(2));
    sinkAppContB.Start(Seconds(DEFAULT_START_TIME));
    sinkAppContB.Stop(Seconds(DEFAULT_END_TIME));



    Simulator::Run();
    Simulator::Destroy();
    return 0;

}