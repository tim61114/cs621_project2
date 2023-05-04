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

#include "./model/strict-priority-queue.cc"


#include <cstdint>
#include <utility>


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SPQSimulation");

Ptr<PointToPointNetDevice> NetDeviceDynamicCast (Ptr<NetDevice> const&p)
{
  return Ptr<PointToPointNetDevice> (dynamic_cast<PointToPointNetDevice *> (PeekPointer (p)));
}

int 
main () 
{   
    uint32_t queueNumber = 2;
    u_int32_t PriorityA = 0;
    u_int32_t PriorityB = 1;

    uint16_t node0PortA = 9000;     // high priority source port
    uint16_t node0PortB = 9500;     // low priority source port
    uint16_t node1Port = 8000;     // Destination port
    uint32_t tcp_adu_size = 946;

    // uint32_t DEFAULT_DATA_BYTES = 1073741824;  // 0.1
    double DEFAULT_START_TIME = 0.0;
    double DEFAULT_END_TIME = 100000000.0;

    double appAStartTime = DEFAULT_START_TIME + 10000000.0;      // start later than B
    double appAEndTime = DEFAULT_END_TIME;
    double appBStartTime = DEFAULT_START_TIME;
    double appBEndTime = DEFAULT_END_TIME;

    //uint32_t DEFAULT_QUEUE_SIZE = 655350000;

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
    // Ptr<StrictPriorityQueue> spqR1 = CreateObject<StrictPriorityQueue>(queueNumber, priorityPortList);   
    // //TODO: set up the queue, traffic class ? filter?
    // Ptr<PointToPointNetDevice> devR1 = NetDeviceDynamicCast(devices2.Get(0));
    // devR1->SetQueue(spqR1);

    ObjectFactory m_spqFactory;
    m_spqFactory.SetTypeId("StrictPriorityQueue");
    m_spqFactory.Set("QueueNumber", UintegerValue(queueNumber));
    m_spqFactory.Set("FirstPriority", UintegerValue(PriorityA));
    m_spqFactory.Set("FirstPort", UintegerValue(node0PortA));
    m_spqFactory.Set("SecondPriority", UintegerValue(PriorityB));
    m_spqFactory.Set("SecondPort", UintegerValue(node0PortB));

    Ptr<StrictPriorityQueue> spq = m_spqFactory.Create<StrictPriorityQueue>();
    Ptr<PointToPointNetDevice> devR1 = NetDeviceDynamicCast(devices2.Get(0));
    devR1->SetQueue(spq);

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

    
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(tcp_adu_size));
    // set destination ip and port
    AddressValue node1Address(InetSocketAddress(interface2.GetAddress(1), node1Port));  //receiver node1's address
    BulkSendHelper BulkSendHelper ("ns3::TcpSocketFactory", Address());
    Ipv4Address node0IP =  interface1.GetAddress(0);

    // configure source ip and port for application A
    ApplicationContainer appContA = BulkSendHelper.Install(nodes.Get(0));
    Ptr<BulkSendApplication> bulkSendApplicationA = DynamicCast<BulkSendApplication>(appContA.Get(0));
    AddressValue node0AddressA(InetSocketAddress(node0IP, node0PortA));
    bulkSendApplicationA->SetAttribute("Local", node0AddressA);
    
    appContA.Start(Seconds(appAStartTime));
    appContA.Stop(Seconds(appAEndTime));
    // bulkSendApplicationA->SetStartTime(Seconds(appAStartTime));
    // bulkSendApplicationA->SetStopTime(Seconds(appAEndTime));


    // Create applciation B and install it on node0
    ApplicationContainer appContB = BulkSendHelper.Install(nodes.Get(0));
    Ptr<BulkSendApplication> bulkSendApplicationB = DynamicCast<BulkSendApplication>(appContB.Get(0));
    AddressValue node0AddressB(InetSocketAddress(node0IP, node0PortB));     // same address with A, diff port
    bulkSendApplicationB->SetAttribute("Local", node0AddressB);
    
    appContA.Start(Seconds(appBStartTime));
    appContA.Stop(Seconds(appBEndTime));
    // bulkSendApplicationB->SetStartTime(Seconds(appBStartTime));
    // bulkSendApplicationB->SetStopTime(Seconds(appBEndTime));

    // Create a packet sinkc application A and install it on node1
    PacketSinkHelper sink("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny(), node1Port));
    ApplicationContainer sinkAppContA = sink.Install(nodes.Get(2));
    sinkAppContA.Start(Seconds(DEFAULT_START_TIME));
    sinkAppContA.Stop(Seconds(DEFAULT_END_TIME));


    Simulator::Run();
    Simulator::Destroy();
    return 0;

}
