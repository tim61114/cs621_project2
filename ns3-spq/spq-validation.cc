// Network Topology
//
//        10.1.1.0  255.255.255.0
// n0 -------------- r1 -------------- n1
//    point-to-point    point-to-point
//

#include <cstdint>
#include <utility>

#include "ns3/log.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/pcap-file-wrapper.h"
#include "ns3/pcap-file.h"
#include "ns3/vector.h"

#include "./model/strict-priority-queue.cc"
#include "ns3/drop-tail-queue.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SPQSimulation");

Ptr<PointToPointNetDevice> NetDeviceDynamicCast (Ptr<NetDevice> const&p)
{
  return Ptr<PointToPointNetDevice> (dynamic_cast<PointToPointNetDevice *> (PeekPointer (p)));
}

int main () 
{   
  uint32_t queueNumber = 2;
  u_int32_t PriorityA = 0;
  u_int32_t PriorityB = 1;

  // uint16_t node0PortA = 4001;     // high priority source port
  // uint16_t node0PortB = 4002;     // low priority source port
  uint16_t node1PortA = 5001;     // high priority estination port
  uint16_t node1PortB = 5002;     // low priority Destination port

  uint32_t queueMaxPackets = 5000;   // max packet number allowed queueing in mid node

  // uint32_t DEFAULT_DATA_BYTES = 1073741824;  // 0.1
  double DEFAULT_START_TIME = 0.0;
  double DEFAULT_END_TIME = 40.0;

  double appAStartTime = DEFAULT_START_TIME + 12.0;      // start later than B A: 12-30, B: 0-40
  double appAEndTime = DEFAULT_END_TIME -10.0;
  double appBStartTime = DEFAULT_START_TIME;
  double appBEndTime = DEFAULT_END_TIME ;


  std::vector<std::pair<uint32_t, uint16_t>> priority_port(queueNumber);
  priority_port[0].first = PriorityA; // TODO: read from config
  priority_port[0].second = node1PortA;
  priority_port[1].first = PriorityB;
  priority_port[1].second = node1PortB;

  // Create vector of TrafficClass*
  std::vector<TrafficClass*> tc_vector;
  for (u_int32_t i = 0; i < queueNumber; i++) {
    DestPortNumberFilter* destPortFilter1 = new DestPortNumberFilter(priority_port[i].second);
    // Create Filter
    Filter* filter1 = new Filter();
    filter1->elements.push_back(destPortFilter1);  
    // Create TrafficClass
    TrafficClass* tc = new TrafficClass();
    tc->setPriorityLevel(priority_port[i].first);
    if (priority_port[i].first == 1) {
      tc->setDefault(true);   // set priority 1 queue as default
    }
    tc->setMaxPackets(queueMaxPackets);
    tc->filters.push_back(filter1);
    tc_vector.push_back(tc);
  }


  NodeContainer nodes;
  nodes.Create(3);

  PointToPointHelper p2p;
  p2p.SetDeviceAttribute("DataRate", StringValue("40Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer devices1 = p2p.Install(nodes.Get(0), nodes.Get(1));

  p2p.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer devices2 = p2p.Install(nodes.Get(1), nodes.Get(2));

  // create spq
  ObjectFactory m_spqFactory;
  m_spqFactory.SetTypeId("StrictPriorityQueue");
  m_spqFactory.Set("QueueNumber", UintegerValue(queueNumber));

  // Install SPQ on router1
  Ptr<StrictPriorityQueue> spq = m_spqFactory.Create<StrictPriorityQueue>();
  spq->setQ_Class(tc_vector);
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

  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  // Create applciation A on node 0
  UdpClientHelper clientA (interface2.GetAddress(1), node1PortA);
  clientA.SetAttribute("Interval", TimeValue(Seconds(0.0003)));
  clientA.SetAttribute("PacketSize", UintegerValue(512));
  clientA.SetAttribute("MaxPackets", UintegerValue(0));
  ApplicationContainer appContA = clientA.Install(nodes.Get(0));
  appContA.Start(Seconds(appAStartTime));  
  appContA.Stop(Seconds(appAEndTime)); 


  // Create applciation B on node 0
  UdpClientHelper clientB (interface2.GetAddress(1), node1PortB);
  clientB.SetAttribute("Interval", TimeValue(Seconds(0.0003)));
  clientB.SetAttribute("PacketSize", UintegerValue(512));
  clientB.SetAttribute("MaxPackets", UintegerValue(0));
  ApplicationContainer appContB = clientB.Install(nodes.Get(0));
  appContB.Start(Seconds(appBStartTime));  
  appContB.Stop(Seconds(appBEndTime));  

  // Create the first UdpServerHelper instance
  UdpServerHelper serverA (node1PortA);
  ApplicationContainer serverAppsA = serverA.Install (nodes.Get(2));
  serverAppsA.Start (Seconds(DEFAULT_START_TIME));
  serverAppsA.Stop (Seconds(DEFAULT_END_TIME));

  // Create the second UdpServerHelper instance
  UdpServerHelper serverB (node1PortB);
  ApplicationContainer serverAppsB = serverB.Install (nodes.Get(2));
  serverAppsB.Start (Seconds(DEFAULT_START_TIME));
  serverAppsB.Stop (Seconds(DEFAULT_END_TIME));
 




  // // Create applciation A on node 0
  // InetSocketAddress destIpPort(interface2.GetAddress(1), node1PortA);
  // BulkSendHelper appA("ns3::TcpSocketFactory", destIpPort);
  // appA.SetAttribute("MaxBytes", UintegerValue(0));  // Send unlimited packets
  // appA.SetAttribute("Local", AddressValue(InetSocketAddress(Ipv4Address::GetAny(), node0PortA)));
  // ApplicationContainer appContA = appA.Install(nodes.Get(0));
  // appContA.Start(Seconds(appAStartTime));  
  // appContA.Stop(Seconds(appAEndTime)); 

  // 
  // InetSocketAddress destIpPort2(interface2.GetAddress(1), node1PortB);   
  // BulkSendHelper appB("ns3::TcpSocketFactory", destIpPort2);
  // appB.SetAttribute("MaxBytes", UintegerValue(0));  // Send unlimited packets
  // appB.SetAttribute("Local", AddressValue(InetSocketAddress(Ipv4Address::GetAny(), node0PortB)));
  // ApplicationContainer appContB = appB.Install(nodes.Get(0));
  // appContB.Start(Seconds(appBStartTime));  
  // appContB.Stop(Seconds(appBEndTime));  

  // Create a packet sinkc application A and install it on node1
  // PacketSinkHelper sink("ns3::UdpSocketFactory",
  //                      InetSocketAddress (Ipv4Address::GetAny(), node1PortA));
  // ApplicationContainer sinkAppContA = sink.Install(nodes.Get(2));
  // sinkAppContA.Start(Seconds(DEFAULT_START_TIME));
  // sinkAppContA.Stop(Seconds(DEFAULT_END_TIME));

  // // Create a packet sinkc application A and install it on node1
  // PacketSinkHelper sink2("ns3::UdpSocketFactory",
  //                      InetSocketAddress (Ipv4Address::GetAny(), node1PortB));
  // ApplicationContainer sinkAppContB = sink2.Install(nodes.Get(2));
  // sinkAppContB.Start(Seconds(DEFAULT_START_TIME));
  // sinkAppContB.Stop(Seconds(DEFAULT_END_TIME));

  p2p.EnablePcapAll("dvc", true);

  Simulator::Run();
  Simulator::Destroy();
  return 0;

}
