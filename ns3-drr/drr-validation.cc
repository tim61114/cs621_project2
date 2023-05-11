#include <cstdint>
#include <utility>
#include <cmath>
#include <vector>

#include "ns3/log.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/pcap-file-wrapper.h"
#include "ns3/pcap-file.h"
#include "ns3/vector.h"

#include "./model/deficit-round-robin.h"
#include "ns3/drop-tail-queue.h"

using namespace ns3;

Ptr<PointToPointNetDevice> NetDeviceDynamicCast (Ptr<NetDevice> const&p)
{
  return Ptr<PointToPointNetDevice> (dynamic_cast<PointToPointNetDevice *> (PeekPointer (p)));
}

int main () 
{   
    uint32_t queueNumber = 3;
    double_t deficitA = 600;
    double_t deficitB = 400;
    double_t deficitC = 200;

    uint16_t node1PortA = 5001;     // first Destination port
    uint16_t node1PortB = 5002;     // second priority Destination port
    uint16_t node1PortC = 5003;     // third priority Destination port

    double DEFAULT_START_TIME = 0.0;
    double DEFAULT_END_TIME = 40.0;


    std::vector<uint16_t> ports = { node1PortA, node1PortB, node1PortC };
    std::vector<double_t> deficit = { deficitA, deficitB, deficitC };

    // Create vector of TrafficClass*
    std::vector<TrafficClass*> tc_vector;
    for (u_int32_t i = 0; i < queueNumber; i++) {
        DestPortNumberFilter* destPortFilter = new DestPortNumberFilter(ports[i]);
        // Create Filter
        Filter* filter1 = new Filter();
        filter1->elements.push_back(destPortFilter);  
        // Create TrafficClass
        TrafficClass* tc = new TrafficClass();
        tc->setWeight(deficit[i]);
        tc->filters.push_back(filter1);
        tc_vector.push_back(tc);
    }


    NodeContainer nodes;
    nodes.Create(3);

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("4Mbps"));
    p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
    NetDeviceContainer devices1 = p2p.Install(nodes.Get(0), nodes.Get(1));

    p2p.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
    p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
    NetDeviceContainer devices2 = p2p.Install(nodes.Get(1), nodes.Get(2));

    // create spq
    ObjectFactory m_drrFactory;
    m_drrFactory.SetTypeId("DeficitRoundRobin");
    m_drrFactory.Set("QueueNumber", UintegerValue(queueNumber));


    // Install SPQ on router1
    Ptr<DeficitRoundRobin> drr = m_drrFactory.Create<DeficitRoundRobin>();
    drr->setQ_Class(tc_vector);
    drr->setDeficit(deficit);
    Ptr<PointToPointNetDevice> devR1 = NetDeviceDynamicCast(devices2.Get(0));
    devR1->SetQueue(drr);

    // Create the Internet stacks
    InternetStackHelper stack;
    stack.Install(nodes);

    // Create addresses
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interface1 = address.Assign(devices1);
    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer interface2 = address.Assign(devices2);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Create applciation A on node 0
    UdpClientHelper clientA (interface2.GetAddress(1), ports[0]);
    clientA.SetAttribute("Interval", TimeValue(Seconds(0.0003)));
    clientA.SetAttribute("PacketSize", UintegerValue(512));
    clientA.SetAttribute("MaxPackets", UintegerValue(0));
    ApplicationContainer appContA = clientA.Install(nodes.Get(0));
    appContA.Start(Seconds(DEFAULT_START_TIME));  
    appContA.Stop(Seconds(DEFAULT_END_TIME)); 


    // Create applciation B on node 0
    UdpClientHelper clientB (interface2.GetAddress(1), ports[1]);
    clientB.SetAttribute("Interval", TimeValue(Seconds(0.0003)));
    clientB.SetAttribute("PacketSize", UintegerValue(512));
    clientB.SetAttribute("MaxPackets", UintegerValue(0));
    ApplicationContainer appContB = clientB.Install(nodes.Get(0));
    appContB.Start(Seconds(DEFAULT_START_TIME));  
    appContB.Stop(Seconds(DEFAULT_END_TIME));  

    // Create applciation C on node 0
    UdpClientHelper clientC (interface2.GetAddress(1), ports[2]);
    clientC.SetAttribute("Interval", TimeValue(Seconds(0.0003)));
    clientC.SetAttribute("PacketSize", UintegerValue(512));
    clientC.SetAttribute("MaxPackets", UintegerValue(0));
    ApplicationContainer appContC = clientC.Install(nodes.Get(0));
    appContC.Start(Seconds(DEFAULT_START_TIME));  
    appContC.Stop(Seconds(DEFAULT_END_TIME));  

    // Create the first UdpServerHelper instance
    UdpServerHelper serverA (ports[0]);
    ApplicationContainer serverAppsA = serverA.Install (nodes.Get(2));
    serverAppsA.Start (Seconds(DEFAULT_START_TIME));
    serverAppsA.Stop (Seconds(DEFAULT_END_TIME));

    // Create the second UdpServerHelper instance
    UdpServerHelper serverB (ports[1]);
    ApplicationContainer serverAppsB = serverB.Install (nodes.Get(2));
    serverAppsB.Start (Seconds(DEFAULT_START_TIME));
    serverAppsB.Stop (Seconds(DEFAULT_END_TIME));

    // Create the third UdpServerHelper instance
    UdpServerHelper serverC (ports[2]);
    ApplicationContainer serverAppsC = serverC.Install (nodes.Get(2));
    serverAppsC.Start (Seconds(DEFAULT_START_TIME));
    serverAppsC.Stop (Seconds(DEFAULT_END_TIME));


    p2p.EnablePcapAll("dvc", true);

    Simulator::Run();
    Simulator::Destroy();
    return 0;

}

