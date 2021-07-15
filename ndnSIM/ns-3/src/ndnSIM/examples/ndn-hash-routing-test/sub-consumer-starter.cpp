/*
 * sub-consumer-starter.cpp
 *
 *  Created on: 2021/05/11
 *      Author: fukudanatsuko
 */

#include "sub-consumer-starter.hpp"
#include <ndn-cxx/util/time.hpp>

#include "ns3/ndnSIM/helper/ndn-stack-helper.hpp"
#include "ns3/application.h"

NS_LOG_COMPONENT_DEFINE("SubConsumerStarter");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(SubConsumerStarter);

TypeId
SubConsumerStarter::GetTypeId()
{
  static TypeId tid = TypeId("SubConsumerStarter")
    .SetParent<Application>()
    .AddConstructor<SubConsumerStarter>();

  return tid;
}

// inherited from Application base class.
void
SubConsumerStarter::StartApplication()
{
  // Create an instance of the app, and passing the dummy version of KeyChain (no real signing)
  m_instance.reset(new app::SubConsumer(ndn::StackHelper::getKeyChain()));
//  m_instance->run(); // can be omitted
  Simulator::Schedule(Seconds(2.0), &app::SubConsumer::run, m_instance.get());
}

void
SubConsumerStarter::StopApplication()
{
  // Stop and destroy the instance of the app
  m_instance.reset();
}

} // namespace ns3



