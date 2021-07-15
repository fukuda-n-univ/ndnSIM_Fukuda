#ifndef NDNSIM_EXAMPLES_NDN_CXX_SIMPLE_CUSTOM_CONSUMER_STARTER_HPP
#define NDNSIM_EXAMPLES_NDN_CXX_SIMPLE_CUSTOM_CONSUMER_STARTER_HPP

#include "consumer.hpp"
#include "ns3/simulator.h"

#include "ns3/ndnSIM/helper/ndn-stack-helper.hpp"
#include "ns3/application.h"

namespace ns3 {

// Class inheriting from ns3::Application
class CustomConsumerStarter : public Application
{
public:
  static TypeId
  GetTypeId();

protected:
  // inherited from Application base class.
  virtual void
  StartApplication();

  virtual void
  StopApplication();

private:
  std::unique_ptr<app::CustomConsumer> m_instance;
};

} // namespace ns3
#endif // NDNSIM_EXAMPLES_NDN_CXX_SIMPLE_REAL_APP_STARTER_HPP
