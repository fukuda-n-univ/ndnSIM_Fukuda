#ifndef NDNSIM_EXAMPLES_NDN_CXX_SIMPLE_CUSTOM_CONSUMER_HPP
#define NDNSIM_EXAMPLES_NDN_CXX_SIMPLE_CUSTOM_CONSUMER_HPP

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/interest.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/util/scheduler.hpp>

#include <iostream>

namespace app {

class CustomConsumer
{
public:
  CustomConsumer(ndn::KeyChain&);

  void
  run();

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

};

} // namespace app

#endif // NDNSIM_EXAMPLES_NDN_CXX_SIMPLE_REAL_APP_HPP
