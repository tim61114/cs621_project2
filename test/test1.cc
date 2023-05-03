#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "../src/lib/traffic-class.h"
#include "../src/lib/filter-element/filter-element.h"
#include "../src/lib/filter.h"

using namespace ns3;

int main(int argc, char *argv[]) {
  // Set up the network topology and components
  NodeContainer nodes;
  nodes.Create(2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
  pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install(nodes);

  InternetStackHelper internet;
  internet.Install(nodes);

  Ipv4AddressHelper address;
  address.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign(devices);

  // Create a TrafficClass with specific parameters
  uint32_t packets = 0;
  uint32_t maxPackets = 100;
  double_t weight = 0.5;
  uint32_t priority_level = 1;
  bool isDefault = false;
  TrafficClass trafficClass(packets, maxPackets, weight, priority_level, isDefault);

  // Add filters to the TrafficClass
  Ipv4Address srcIP("10.1.1.1");
  FilterElement* srcIPFilter = new SrcIPAddr(srcIP);
  Filter* filter = new Filter();
  filter->AddFilterElement(srcIPFilter);
  trafficClass.AddFilter(filter);

  // Create and configure an OnOff application to generate traffic
  OnOffHelper onOffHelper("ns3::UdpSocketFactory", InetSocketAddress(interfaces.GetAddress(1), 9));
  onOffHelper.SetConstantRate(DataRate("2Mbps"), 1024);
  onOffHelper.SetAttribute("StartTime", TimeValue(Seconds(1.0)));
  onOffHelper.SetAttribute("StopTime", TimeValue(Seconds(10.0)));

  ApplicationContainer sourceApp = onOffHelper.Install(nodes.Get(0));

  // Create a PacketSink application to receive traffic
  PacketSinkHelper sinkHelper("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), 9));
  sinkHelper.SetAttribute("StartTime", TimeValue(Seconds(1.0)));
  sinkHelper.SetAttribute("StopTime", TimeValue(Seconds(10.0)));

  ApplicationContainer sinkApp = sinkHelper.Install(nodes.Get(1));

  // Run the simulation
  Simulator::Stop(Seconds(11.0));
  Simulator::Run();

  // Calculate the total received packets
  Ptr<PacketSink> sink = DynamicCast<PacketSink>(sinkApp.Get(0));
  uint64_t totalReceived = sink->GetTotalRx();

  // Print simulation results
  std::cout << "Total received packets: " << totalReceived << std::endl;

  // Clean up
  Simulator::Destroy();
  return 0;
}