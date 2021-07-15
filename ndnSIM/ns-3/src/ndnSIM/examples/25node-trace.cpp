/*
 * 25node-trace.cpp
 *
 *  Created on: 2020/01/10
 *      Author: fukudanatsuko
 */



#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {

int
main(int argc, char* argv[])
{
  CommandLine cmd;
  cmd.Parse(argc, argv);

  AnnotatedTopologyReader topologyReader("", 1);
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/topo-tree-25-node.txt");
  topologyReader.Read();

  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.InstallAll();

  // Choosing forwarding strategy
  ndn::StrategyChoiceHelper::InstallAll("/prefix", "/localhost/nfd/strategy/best-route");

  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  // Getting containers for the consumer/producer
  Ptr<Node> consumers[9] = {Names::Find<Node>("Src1"), Names::Find<Node>("Src2"),
                            Names::Find<Node>("Src3"), Names::Find<Node>("Src4"),
  	  	  	  	  	  	  	Names::Find<Node>("Src5"),Names::Find<Node>("Src6"),
							Names::Find<Node>("Src7"),Names::Find<Node>("Src8"),
							Names::Find<Node>("Src9")};
  Ptr<Node> producers[9] ={ Names::Find<Node>("Dst1"),Names::Find<Node>("Dst2"),
		  	  	  	  	   Names::Find<Node>("Dst3"),Names::Find<Node>("Dst4"),
						   Names::Find<Node>("Dst5"),Names::Find<Node>("Dst6"),
						   Names::Find<Node>("Dst7"),Names::Find<Node>("Dst8"),
						   Names::Find<Node>("Dst9")};

    ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
    consumerHelper.SetAttribute("Frequency", StringValue("100")); // 100 interests a second

    consumerHelper.SetPrefix("/dst1");
    consumerHelper.Install(consumers[1]);

    consumerHelper.SetPrefix("/dst2");
    consumerHelper.Install(consumers[2]);


    consumerHelper.SetPrefix("/dst3");
    consumerHelper.Install(consumers[3]);


    consumerHelper.SetPrefix("/dst4");
    consumerHelper.Install(consumers[4]);


    consumerHelper.SetPrefix("/dst5");
    consumerHelper.Install(consumers[5]);


    consumerHelper.SetPrefix("/dst6");
    consumerHelper.Install(consumers[6]);


    consumerHelper.SetPrefix("/dst7");
    consumerHelper.Install(consumers[7]);


    consumerHelper.SetPrefix("/dst8");
    consumerHelper.Install(consumers[8]);


    consumerHelper.SetPrefix("/dst9");
    consumerHelper.Install(consumers[9]);


  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));

  ndnGlobalRoutingHelper.AddOrigins("/dst1", producers[1]);
    producerHelper.SetPrefix("/dst1");
    producerHelper.Install(producers[1]);

  ndnGlobalRoutingHelper.AddOrigins("/dst2", producers[2]);
    producerHelper.SetPrefix("/dst2");
    producerHelper.Install(producers[2]);

   ndnGlobalRoutingHelper.AddOrigins("/dst3", producers[3]);
   producerHelper.SetPrefix("/dst3");
   producerHelper.Install(producers[3]);

   ndnGlobalRoutingHelper.AddOrigins("/dst4", producers[4]);
   producerHelper.SetPrefix("/dst4");
   producerHelper.Install(producers[4]);

   ndnGlobalRoutingHelper.AddOrigins("/dst5", producers[5]);
     producerHelper.SetPrefix("/dst5");
     producerHelper.Install(producers[5]);

   ndnGlobalRoutingHelper.AddOrigins("/dst6", producers[6]);
     producerHelper.SetPrefix("/dst6");
     producerHelper.Install(producers[6]);

   ndnGlobalRoutingHelper.AddOrigins("/dst7", producers[7]);
     producerHelper.SetPrefix("/dst7");
     producerHelper.Install(producers[7]);

   ndnGlobalRoutingHelper.AddOrigins("/dst8", producers[8]);
     producerHelper.SetPrefix("/dst8");
     producerHelper.Install(producers[8]);

     ndnGlobalRoutingHelper.AddOrigins("/dst9", producers[9]);
       producerHelper.SetPrefix("/dst9");
       producerHelper.Install(producers[9]);


  // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes();

  Simulator::Stop(Seconds(20.0));

  ndn::L3RateTracer::InstallAll("rate-trace.txt", Seconds(0.5));
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
