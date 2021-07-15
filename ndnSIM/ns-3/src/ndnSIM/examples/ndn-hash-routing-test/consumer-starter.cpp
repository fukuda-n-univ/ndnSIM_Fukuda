#include "consumer-starter.hpp"
#include <ndn-cxx/util/time.hpp>

#include "ns3/ndnSIM/helper/ndn-stack-helper.hpp"
#include "ns3/application.h"

NS_LOG_COMPONENT_DEFINE("CustomConsumerStarter");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(CustomConsumerStarter);

TypeId
CustomConsumerStarter::GetTypeId()
{
  static TypeId tid = TypeId("CustomConsumerStarter")
    .SetParent<Application>()
    .AddConstructor<CustomConsumerStarter>();

  return tid;
}

// inherited from Application base class.
void
CustomConsumerStarter::StartApplication()
{
  // Create an instance of the app, and passing the dummy version of KeyChain (no real signing)
  m_instance.reset(new app::CustomConsumer(ndn::StackHelper::getKeyChain()));
//  m_instance->run(); // can be omitted
  Simulator::Schedule(Seconds(2.0), &app::CustomConsumer::run, m_instance.get());
}

void
CustomConsumerStarter::StopApplication()
{
  // Stop and destroy the instance of the app
  m_instance.reset();
}

} // namespace ns3
