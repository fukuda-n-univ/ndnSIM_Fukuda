/*
 * ndn-sub-consumer-cbr.hpp
 *
 *  Created on: 2021/06/22
 *      Author: fukudanatsuko
 */


#ifndef NDN_SUB_CONSUMER_CBR_H
#define NDN_SUB_CONSUMER_CBR_H

#include "ns3/ndnSIM/model/ndn-common.hpp"

#include "ndn-consumer.hpp"

namespace ns3 {
namespace ndn {


class SubConsumerCbr : public Consumer {
public:
  static TypeId
  GetTypeId();

  SubConsumerCbr();
  virtual ~SubConsumerCbr();
/*
  void
  SendPacket();

  std::size_t
  getConsumerId();*/

protected:

  virtual void
  ScheduleNextPacket();

  void
  SetRandomize(const std::string& value);

  std::string
  GetRandomize() const;

protected:
  double m_frequency; // Frequency of interest packets (in hertz)
  bool m_firstTime;
  Ptr<RandomVariableStream> m_random;
  std::string m_randomType;

  std::size_t consumerID;
};

} // namespace ndn
} // namespace ns3

#endif
