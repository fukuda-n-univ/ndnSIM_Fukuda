/*
 * ndn-sub-consumer-cbr.cpp
 *
 *  Created on: 2021/06/22
 *      Author: fukudanatsuko
 */


#include "ndn-sub-consumer-cbr.hpp"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/callback.h"
#include "ns3/string.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include "ns3/integer.h"
#include "ns3/double.h"


#include <stdio.h>

#include <boost/random/uniform_int_distribution.hpp>
#include<random>

#include <functional>
#include <string>

NS_LOG_COMPONENT_DEFINE("ndn.SubConsumerCbr");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(SubConsumerCbr);

TypeId
SubConsumerCbr::GetTypeId(void)
{
  static TypeId tid =
    TypeId("ns3::ndn::SubConsumerCbr")
      .SetGroupName("Ndn")
      .SetParent<Consumer>()
      .AddConstructor<SubConsumerCbr>()

      .AddAttribute("Frequency", "Frequency of interest packets", StringValue("1.0"),
                    MakeDoubleAccessor(&SubConsumerCbr::m_frequency), MakeDoubleChecker<double>())

      .AddAttribute("Randomize",
                    "Type of send time randomization: none (default), uniform, exponential",
                    StringValue("none"),
                    MakeStringAccessor(&SubConsumerCbr::SetRandomize, &SubConsumerCbr::GetRandomize),
                    MakeStringChecker())

      .AddAttribute("MaxSeq", "Maximum sequence number to request",
                    IntegerValue(std::numeric_limits<uint32_t>::max()),
                    MakeIntegerAccessor(&SubConsumerCbr::m_seqMax), MakeIntegerChecker<uint32_t>())

    ;

  return tid;
}

SubConsumerCbr::SubConsumerCbr()
  : m_frequency(1.0)
  , m_firstTime(true)
{
  NS_LOG_FUNCTION_NOARGS();
  m_seqMax = std::numeric_limits<uint32_t>::max();

  std::random_device rnd;
  std::mt19937 mt;
  mt.seed(rnd());
  boost::random::uniform_int_distribution<> randomNumGenerator(1,100000);

  int randomNum=randomNumGenerator(mt);

  std::string randomNumStr=std::to_string(randomNum);
  consumerID=std::hash<std::string>()(randomNumStr);
}

SubConsumerCbr::~SubConsumerCbr()
{
}
/*
void
SubConsumerCbr::SendPacket(){
	 if (!m_active)
	    return;

	  NS_LOG_FUNCTION_NOARGS();

	  std::string str=std::to_string(consumerID);
	  shared_ptr<Name> name = make_shared<Name>(str);

	  // shared_ptr<Interest> interest = make_shared<Interest> ();
	  shared_ptr<Interest> interest = make_shared<Interest>();
	  interest->setNonce(m_rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
	  interest->setName(*name);
	  interest->setHashFlag(11);
	  interest->setHashTTL(4);
	  interest->setConsumerID(this->getConsumerId());

	  NS_LOG_DEBUG("SubConsumer Name=" <<interest->getName());
	  time::milliseconds interestLifeTime(m_interestLifeTime.GetMilliSeconds());
	  interest->setInterestLifetime(interestLifeTime);

	  m_transmittedInterests(interest, this, m_face);
	  m_appLink->onReceiveInterest(*interest);



	  ScheduleNextPacket();
}


std::size_t
SubConsumerCbr::getConsumerId(){
	return consumerID;
}*/

void
SubConsumerCbr::ScheduleNextPacket()
{
  // double mean = 8.0 * m_payloadSize / m_desiredRate.GetBitRate ();
  // std::cout << "next: " << Simulator::Now().ToDouble(Time::S) + mean << "s\n";

  if (m_firstTime) {
    m_sendEvent = Simulator::Schedule(Seconds(0.0), &Consumer::SendPacket, this);
    m_firstTime = false;
  }
  else if (!m_sendEvent.IsRunning())
    m_sendEvent = Simulator::Schedule((m_random == 0) ? Seconds(1.0 / m_frequency)
                                                      : Seconds(m_random->GetValue()),
                                      &Consumer::SendPacket, this);
}

void
SubConsumerCbr::SetRandomize(const std::string& value)
{
  if (value == "uniform") {
    m_random = CreateObject<UniformRandomVariable>();
    m_random->SetAttribute("Min", DoubleValue(0.0));
    m_random->SetAttribute("Max", DoubleValue(2 * 1.0 / m_frequency));
  }
  else if (value == "exponential") {
    m_random = CreateObject<ExponentialRandomVariable>();
    m_random->SetAttribute("Mean", DoubleValue(1.0 / m_frequency));
    m_random->SetAttribute("Bound", DoubleValue(50 * 1.0 / m_frequency));
  }
  else
    m_random = 0;

  m_randomType = value;
}

std::string
SubConsumerCbr::GetRandomize() const
{
  return m_randomType;
}

} // namespace ndn
} // namespace ns3



