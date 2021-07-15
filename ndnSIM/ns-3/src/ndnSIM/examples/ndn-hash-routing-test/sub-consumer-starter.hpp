/*
 * sub-consumer-starter.hpp
 *
 *  Created on: 2021/05/11
 *      Author: fukudanatsuko
 */

#ifndef SRC_NDNSIM_EXAMPLES_NDN_HASH_ROUTING_SUB_CONSUMER_STARTER_HPP_
#define SRC_NDNSIM_EXAMPLES_NDN_HASH_ROUTING_SUB_CONSUMER_STARTER_HPP_

#include "sub-consumer.hpp"
#include "ns3/simulator.h"

#include "ns3/ndnSIM/helper/ndn-stack-helper.hpp"
#include "ns3/application.h"

namespace ns3{

// Class inheriting from ns3::Application
class SubConsumerStarter : public Application
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
  std::unique_ptr<app::SubConsumer> m_instance;
};

}

#endif /* SRC_NDNSIM_EXAMPLES_NDN_HASH_ROUTING_SUB_CONSUMER_STARTER_HPP_ */
