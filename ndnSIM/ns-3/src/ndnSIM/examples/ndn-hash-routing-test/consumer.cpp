#include "consumer.hpp"

namespace app {

CustomConsumer::CustomConsumer(ndn::KeyChain& keyChain)
  : m_keyChain(keyChain)
  , m_scheduler(m_face.getIoService())
{

}

void
CustomConsumer::run()
{
  ndn::Interest interest(ndn::Name("/ndn-fc/f1"));
  interest.setHashFlag(2);
  interest.setHashTTL(2);
  interest.setInterestLifetime(ndn::time::milliseconds(1000));
  interest.setMustBeFresh(true);

  m_face.expressInterest(interest,
                        std::bind(&CustomConsumer::onData, this, _1, _2),
                        std::bind(&CustomConsumer::onNack, this, _1, _2),
                        std::bind(&CustomConsumer::onTimeout, this, _1));
  std::cout << "Sending: " << interest << std::endl;

   m_face.processEvents(); // ok (will not block and do nothing)
   // m_faceConsumer.getIoService().run(); // will crash
}

void
CustomConsumer::onData(const ndn::Interest& interest, const ndn::Data& data)
{
  std::cout << data << std::endl;
}

void
CustomConsumer::onNack(const ndn::Interest& interest, const ndn::lp::Nack& nack)
{
  std::cout << "received Nack with reason " << nack.getReason() << " for interest" << interest << std::endl;
}

void
CustomConsumer::onTimeout(const ndn::Interest& interest)
{
  std::cout << "Timeout " << interest << std::endl;
}

} // namespace app
