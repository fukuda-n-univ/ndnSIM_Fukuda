
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {

int
main(int argc, char* argv[])
{


  // Setting default parameters for PointToPoint links and channels
  Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
  Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
  Config::SetDefault("ns3::DropTailQueue::MaxPackets", StringValue("10"));

  CommandLine cmd;
  cmd.Parse(argc, argv);

  AnnotatedTopologyReader topologyReader("", 25);
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/geant.txt");
  topologyReader.Read();

  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;

  ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize",
                                "100"); // default ContentStore parameters
  ndnHelper.InstallAll();

  // Set BestRoute strategy
  ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/best-route");

  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  // Getting containers for the consumer/producer
  Ptr<Node> producer1 = Names::Find<Node>("rtr-22");
  Ptr<Node> producer2 =Names::Find<Node>("rtr-30");
  Ptr<Node> producer3 =Names::Find<Node>("rtr-31");
  Ptr<Node> producer4 =Names::Find<Node>("rtr-33");

  NodeContainer consumerNodes;

  consumerNodes.Add(Names::Find<Node>("rtr-16"));

  consumerNodes.Add(Names::Find<Node>("rtr-1"));
  consumerNodes.Add(Names::Find<Node>("rtr-2"));
  consumerNodes.Add(Names::Find<Node>("rtr-3"));
  consumerNodes.Add(Names::Find<Node>("rtr-4"));
  consumerNodes.Add(Names::Find<Node>("rtr-5"));
  consumerNodes.Add(Names::Find<Node>("rtr-6"));
  consumerNodes.Add(Names::Find<Node>("rtr-7"));
  consumerNodes.Add(Names::Find<Node>("rtr-8"));
  consumerNodes.Add(Names::Find<Node>("rtr-9"));
  consumerNodes.Add(Names::Find<Node>("rtr-10"));
  consumerNodes.Add(Names::Find<Node>("rtr-11"));
  consumerNodes.Add(Names::Find<Node>("rtr-12"));
  consumerNodes.Add(Names::Find<Node>("rtr-13"));
  consumerNodes.Add(Names::Find<Node>("rtr-14"));
  consumerNodes.Add(Names::Find<Node>("rtr-15"));

  //myConsumerNodes.Add(Names::Find<Node>("rtr-16"));
  consumerNodes.Add(Names::Find<Node>("rtr-17"));
  consumerNodes.Add(Names::Find<Node>("rtr-18"));
  consumerNodes.Add(Names::Find<Node>("rtr-19"));
  consumerNodes.Add(Names::Find<Node>("rtr-20"));
  consumerNodes.Add(Names::Find<Node>("rtr-21"));
  //myConsumerNodes.Add(Names::Find<Node>("rtr-22"));

  consumerNodes.Add(Names::Find<Node>("rtr-23"));
  consumerNodes.Add(Names::Find<Node>("rtr-24"));
  consumerNodes.Add(Names::Find<Node>("rtr-25"));
  consumerNodes.Add(Names::Find<Node>("rtr-26"));
  consumerNodes.Add(Names::Find<Node>("rtr-27"));
  consumerNodes.Add(Names::Find<Node>("rtr-28"));
  consumerNodes.Add(Names::Find<Node>("rtr-29"));
  //myConsumerNodes.Add(Names::Find<Node>("rtr-30"));
  //myConsumerNodes.Add(Names::Find<Node>("rtr-31"));
  consumerNodes.Add(Names::Find<Node>("rtr-32"));
  //myConsumerNodes.Add(Names::Find<Node>("rtr-33"));
  consumerNodes.Add(Names::Find<Node>("rtr-34"));
  consumerNodes.Add(Names::Find<Node>("rtr-35"));
  consumerNodes.Add(Names::Find<Node>("rtr-36"));

  // Install NDN applications
  std::string prefix = "/prefix";

  ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
  consumerHelper.SetPrefix(prefix);
  consumerHelper.SetAttribute("Frequency", StringValue("100")); // 100 interests a second
  consumerHelper.Install("rtr-16");
/*
  ndn::AppHelper myConsumerHelper("ns3::ndn::ConsumerHash");
  myConsumerHelper.SetPrefix(prefix);
  myConsumerHelper.SetAttribute("Frequency", StringValue("50"));
  myConsumerHelper.Install("rtr-1");
  myConsumerHelper.Install("rtr-2");
  myConsumerHelper.Install("rtr-3");
  myConsumerHelper.Install("rtr-4");
  myConsumerHelper.Install("rtr-5");
  myConsumerHelper.Install("rtr-6");
  myConsumerHelper.Install("rtr-7");
  myConsumerHelper.Install("rtr-8");
  myConsumerHelper.Install("rtr-9");
  myConsumerHelper.Install("rtr-10");
  myConsumerHelper.Install("rtr-11");
  myConsumerHelper.Install("rtr-12");
  myConsumerHelper.Install("rtr-13");
  myConsumerHelper.Install("rtr-14");
  myConsumerHelper.Install("rtr-15");

  myConsumerHelper.Install("rtr-17");
  myConsumerHelper.Install("rtr-18");
  myConsumerHelper.Install("rtr-19");
  myConsumerHelper.Install("rtr-20");
  myConsumerHelper.Install("rtr-21");

  myConsumerHelper.Install("rtr-23");
  myConsumerHelper.Install("rtr-24");
  myConsumerHelper.Install("rtr-25");
  myConsumerHelper.Install("rtr-26");
  myConsumerHelper.Install("rtr-27");
  myConsumerHelper.Install("rtr-28");
  myConsumerHelper.Install("rtr-29");

  myConsumerHelper.Install("rtr-32");

  myConsumerHelper.Install("rtr-34");
  myConsumerHelper.Install("rtr-35");
  myConsumerHelper.Install("rtr-36");*/

  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  producerHelper.SetPrefix(prefix);
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));

  producerHelper.Install(producer1);
  producerHelper.Install(producer2);
  producerHelper.Install(producer3);
  producerHelper.Install(producer4);


  // Add /prefix origins to ndn::GlobalRouter
  ndnGlobalRoutingHelper.AddOrigins(prefix, producer1);
  ndnGlobalRoutingHelper.AddOrigins(prefix,producer2);
  ndnGlobalRoutingHelper.AddOrigins(prefix, producer3);
  ndnGlobalRoutingHelper.AddOrigins(prefix,producer4);

  // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes();

  Simulator::Stop(Seconds(10.0));

   ndn::AppDelayTracer::InstallAll("app-delays-trace.txt");

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
