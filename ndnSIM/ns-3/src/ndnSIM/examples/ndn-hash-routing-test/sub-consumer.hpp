/*
 * sub-consumer.hpp
 *
 *  Created on: 2021/05/11
 *      Author: fukudanatsuko
 */


#ifndef NDNSIM_EXAMPLES_NDN_CXX_SIMPLE_SUB_CONSUMER_HPP
#define NDNSIM_EXAMPLES_NDN_CXX_SIMPLE_SUB_CONSUMER_HPP

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/interest.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/util/scheduler.hpp>

#include <iostream>

namespace app {

class SubConsumer
{
public:
  SubConsumer(ndn::KeyChain&);

  void
  run();

  std::size_t
  getConsumerId();


private:
  void
  onData(const ndn::Interest&, const ndn::Data&);

  void
  onNack(const ndn::Interest&, const ndn::lp::Nack&);

  void
  onTimeout(const ndn::Interest&);

private:
  ndn::KeyChain& m_keyChain;
  ndn::Face m_face;
  ndn::Scheduler m_scheduler;


  std::size_t consumerID;
};

} // namespace app

#endif // NDNSIM_EXAMPLES_NDN_CXX_SIMPLE_REAL_APP_HPP
