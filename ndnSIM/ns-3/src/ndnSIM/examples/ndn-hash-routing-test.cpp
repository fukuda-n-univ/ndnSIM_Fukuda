/*
 * ndn-hash-routing-test.cpp
 *
 *  Created on: 2021/05/25
 *      Author: fukudanatsuko
 */


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {

/**
 * This scenario simulates a one-node two-custom-app scenario:
 *
 *   +------+ <-----> (CustomApp)
 *   | Node |
 *   +------+ <-----> (Hijacker)
 *
 *     NS_LOG=CustomApp ./waf --run=ndn-custom-apps
 */
int
main(int argc, char* argv[])
{
  // Read optional command-line parameters (e.g., enable visualizer with ./waf --run=<> --visualize

// Setting default parameters for PointToPoint links and channels
  Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
  Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
  Config::SetDefault("ns3::DropTailQueue::MaxPackets", StringValue("10"));

  CommandLine cmd;
  cmd.Parse(argc, argv);

  AnnotatedTopologyReader topologyReader("", 25);
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/topo-grid-3x3.txt");
  topologyReader.Read();


  // Creating nodes
  Ptr<Node> node0 = Names::Find<Node>("Node0");
  Ptr<Node> node1 = Names::Find<Node>("Node1");
  Ptr<Node> node2 = Names::Find<Node>("Node2");
  Ptr<Node> node3 = Names::Find<Node>("Node3");
  Ptr<Node> node4 = Names::Find<Node>("Node4");
  Ptr<Node> node5 = Names::Find<Node>("Node5");
  Ptr<Node> node6 = Names::Find<Node>("Node6");
  Ptr<Node> node7 = Names::Find<Node>("Node7");
  Ptr<Node> node8 = Names::Find<Node>("Node8");

  // App1
  ndn::AppHelper app1("ns3::ndn::Producer");
  app1.Install(node8);

  // App2


  ndn::AppHelper app2("ns3::ndn::SubConsumerCbr");
  app2.Install(node1);
  app2.Install(node2);
  app2.Install(node3);
  app2.Install(node4);
  app2.Install(node5);
  app2.Install(node6);
  app2.Install(node7);

  app2.SetAttribute("Frequency", StringValue("10"));

  ndn::AppHelper app3("CustomConsumerStarter");
  app3.Install(node0);

  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.SetDefaultRoutes(true);
  ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize",
                                "100"); // default ContentStore
  ndnHelper.InstallAll();

  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();


  // Install NDN applications
  std::string prefix = "/ndn-fc/f1";

  ndnGlobalRoutingHelper.AddOrigins(prefix, node8);

  // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes();

  Simulator::Stop(Seconds(10.0));

  ndn::AppDelayTracer::InstallAll("app-delays-trace.txt");
  ndn::L3RateTracer::InstallAll("rate-trace.txt", Seconds(0.5));
  ndn::CsTracer::InstallAll("cs-trace.txt", Seconds(1));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}



