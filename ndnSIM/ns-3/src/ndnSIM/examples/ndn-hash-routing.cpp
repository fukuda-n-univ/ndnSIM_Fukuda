
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
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/geant.txt");
  topologyReader.Read();


  // Creating nodes
  Ptr<Node> node1 = Names::Find<Node>("rtr-1");
  Ptr<Node> node2 = Names::Find<Node>("rtr-2");
  Ptr<Node> node3 = Names::Find<Node>("rtr-3");
  Ptr<Node> node4 = Names::Find<Node>("rtr-4");
  Ptr<Node> node5 = Names::Find<Node>("rtr-5");
  Ptr<Node> node6 = Names::Find<Node>("rtr-6");
  Ptr<Node> node7 = Names::Find<Node>("rtr-7");
  Ptr<Node> node8 = Names::Find<Node>("rtr-8");
  Ptr<Node> node9 = Names::Find<Node>("rtr-9");
  Ptr<Node> node10 = Names::Find<Node>("rtr-10");
  Ptr<Node> node11 = Names::Find<Node>("rtr-11");
  Ptr<Node> node12 = Names::Find<Node>("rtr-12");
  Ptr<Node> node13 = Names::Find<Node>("rtr-13");
  Ptr<Node> node14 = Names::Find<Node>("rtr-14");
  Ptr<Node> node15 = Names::Find<Node>("rtr-15");
  Ptr<Node> node16 = Names::Find<Node>("rtr-16");
  Ptr<Node> node17 = Names::Find<Node>("rtr-17");
  Ptr<Node> node18 = Names::Find<Node>("rtr-18");
  Ptr<Node> node19 = Names::Find<Node>("rtr-19");
  Ptr<Node> node20 = Names::Find<Node>("rtr-20");
  Ptr<Node> node21 = Names::Find<Node>("rtr-21");
  Ptr<Node> node22 = Names::Find<Node>("rtr-22");
  Ptr<Node> node23 = Names::Find<Node>("rtr-23");
  Ptr<Node> node24 = Names::Find<Node>("rtr-24");
  Ptr<Node> node25 = Names::Find<Node>("rtr-25");
  Ptr<Node> node26 = Names::Find<Node>("rtr-26");
  Ptr<Node> node27 = Names::Find<Node>("rtr-27");
  Ptr<Node> node28 = Names::Find<Node>("rtr-28");
  Ptr<Node> node29 = Names::Find<Node>("rtr-29");
  Ptr<Node> node30 = Names::Find<Node>("rtr-30");
  Ptr<Node> node31 = Names::Find<Node>("rtr-31");
  Ptr<Node> node32 = Names::Find<Node>("rtr-32");
  Ptr<Node> node33 = Names::Find<Node>("rtr-33");
  Ptr<Node> node34 = Names::Find<Node>("rtr-34");
  Ptr<Node> node35 = Names::Find<Node>("rtr-35");
  Ptr<Node> node36 = Names::Find<Node>("rtr-36");

  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.SetDefaultRoutes(true);
  ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize",
                                "100"); // default ContentStore
  ndnHelper.InstallAll();

  // App1
  ndn::AppHelper app1("ns3::ndn::Producer");
  app1.Install(node1);

  // App2
  ndn::AppHelper app2("ns3::ndn::SubConsumerCbr");
  app2.Install(node2);
  app2.Install(node3);
  app2.Install(node4);
  app2.Install(node5);
  app2.Install(node6);
  app2.Install(node7);
  app2.Install(node8);
  app2.Install(node9);
  app2.Install(node10);
  app2.Install(node11);
  app2.Install(node12);
  app2.Install(node13);

  app2.Install(node15);
  app2.Install(node16);
  app2.Install(node17);
  app2.Install(node18);
  app2.Install(node19);
  app2.Install(node20);
  app2.Install(node21);
  app2.Install(node22);
  app2.Install(node23);
  app2.Install(node24);
  app2.Install(node25);
  app2.Install(node26);
  app2.Install(node27);
  app2.Install(node28);
  app2.Install(node29);
  app2.Install(node30);
  app2.Install(node31);
  app2.Install(node32);
  app2.Install(node33);
  app2.Install(node34);
  app2.Install(node35);
  app2.Install(node36);

  ndn::AppHelper app3("CustomConsumerStarter");
  app3.Install(node14);



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
