/*
 * sub-consumer.cpp
 *
 *  Created on: 2021/05/11
 *      Author: fukudanatsuko
 */


#include "sub-consumer.hpp"

#include <boost/random/uniform_int_distribution.hpp>
#include<random>


#include <functional>
#include <string>

namespace app {

SubConsumer::SubConsumer(ndn::KeyChain& keyChain)
  : m_keyChain(keyChain)
  , m_scheduler(m_face.getIoService())
{
	std::random_device rnd;
	std::mt19937 mt;
	mt.seed(rnd());
	boost::random::uniform_int_distribution<> randomNumGenerator(1,100000);

	int randomNum=randomNumGenerator(mt);


	std::string randomNumStr=std::to_string(randomNum);
	consumerID=std::hash<std::string>()(randomNumStr);

}

void
SubConsumer::run()
{
  std::string interestName=std::to_string(this->getConsumerId());
  ndn::Interest interest(interestName);

  interest.setHashFlag(11);
  interest.setHashTTL(2);
  interest.setConsumerID(this->getConsumerId());
  interest.setInterestLifetime(ndn::time::milliseconds(1000));
  interest.setMustBeFresh(true);

  m_face.expressInterest(interest,
                        std::bind(&SubConsumer::onData, this, _1, _2),
                        std::bind(&SubConsumer::onNack, this, _1, _2),
                        std::bind(&SubConsumer::onTimeout, this, _1));
  std::cout << "Sending: " << interest << std::endl;

   m_face.processEvents(); // ok (will not block and do nothing)
   // m_faceConsumer.getIoService().run(); // will crash
}

std::size_t
SubConsumer::getConsumerId(){
	return consumerID;
}

void
SubConsumer::onData(const ndn::Interest& interest, const ndn::Data& data)
{
  std::cout << data << std::endl;
}

void
SubConsumer::onNack(const ndn::Interest& interest, const ndn::lp::Nack& nack)
{
  std::cout << "received Nack with reason " << nack.getReason() << " for interest" << interest << std::endl;
}

void
SubConsumer::onTimeout(const ndn::Interest& interest)
{
  std::cout << "Timeout " << interest << std::endl;
}

} // namespace app
