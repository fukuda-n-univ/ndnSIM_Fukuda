/*
 * ndn-consumer-hash.cpp
 *
 *  Created on: 2020/09/02
 *      Author: fukudanatsuko
 */



#include "ndn-consumer-hash.hpp"
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
//#include "size-t.h"

#include <functional>
#include <string>

#include<ndn-cxx/signature.hpp>

NS_LOG_COMPONENT_DEFINE("ndn.ConsumerHash");

namespace ns3{
namespace ndn{

NS_OBJECT_ENSURE_REGISTERED(ConsumerHash);

TypeId
ConsumerHash::GetTypeId(void){
	static TypeId tid =
			TypeId("ns3::ndn::ConsumerHash")
			.SetGroupName("Ndn")
			.SetParent<Consumer>()
			.AddConstructor<ConsumerHash>()
			.AddAttribute("Frequency", "Frequency of interest packets", StringValue("1.0"),
                    MakeDoubleAccessor(&ConsumerHash::m_frequency), MakeDoubleChecker<double>())
			.AddAttribute("Randomize",
					                    "Type of send time randomization: none (default), uniform, exponential",
					                    StringValue("none"),
					                    MakeStringAccessor(&ConsumerHash::SetRandomize, &ConsumerHash::GetRandomize),
					                    MakeStringChecker())

			.AddAttribute("MaxSeq", "Maximum sequence number to request",
					                    IntegerValue(std::numeric_limits<uint32_t>::max()),
					                    MakeIntegerAccessor(&ConsumerHash::m_seqMax), MakeIntegerChecker<uint32_t>());
	/*		.AddAttribute("ConsumerID","Hash number of appID",Size_tValue(0),
					MakeSize_tAccessor(&ConsumerHash::consumerID),
							MakeSize_tChecker<std::size_t>());*/
	return tid;
}

ConsumerHash::ConsumerHash()
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


      NS_LOG_DEBUG(randomNum);

      std::string randomNumStr=std::to_string(randomNum);
      consumerID=std::hash<std::string>()(randomNumStr);

      NS_LOG_DEBUG(consumerID);
}

ConsumerHash::~ConsumerHash(){
}

void
ConsumerHash::OnData(shared_ptr<const Data> contentObject){
	  if (!m_active)
	    return;

	  shared_ptr<const Data> data;

	  //if(contentObject->getHashFlag()==true){

		//  const Name& trueName= contentObject->getSubName();

		//  shared_ptr<Data> datatmp=const_pointer_cast<Data>(contentObject);

		 // datatmp->setName(trueName);
		  //datatmp->resetHashFlag();

		/*  ndn::Signature signature;
		  ndn::SignatureInfo signatureInfo(static_cast< ::ndn::tlv::SignatureTypeValue>(255));

		  if(m_keyLocator.size() > 0){
			  signatureInfo.setKeyLocator(m_keyLocator);
		  }

		  signature.setInfo(signatureInfo);
		  signature.setValue(::ndn::makeNonNegativeIntegerBlock(::ndn::tlv::SignatureValue, m_signature));

		  datatmp->setSignature(signature);
*/
		  //data=const_pointer_cast<const Data>(datatmp);

	  //}else{
		  data=contentObject;
	 // }


	  App::OnData(data); // tracing inside

	  NS_LOG_FUNCTION(this << data);

	  // NS_LOG_INFO ("Received content object: " << boost::cref(*data));

	  // This could be a problem......
	  uint32_t seq = data->getName().at(-1).toSequenceNumber();
	  NS_LOG_INFO("< DATA for " << seq);

	  int hopCount = 0;
/*	  auto hopCountTag = data->getTag<lp::HopCountTag>();
	  if (hopCountTag != nullptr) { // e.g., packet came from local node's cache
	    hopCount = *hopCountTag;
	  }
	  NS_LOG_DEBUG("Hop count: " << hopCount);*/

	  SeqTimeoutsContainer::iterator entry = m_seqLastDelay.find(seq);
	  if (entry != m_seqLastDelay.end()) {
	    m_lastRetransmittedInterestDataDelay(this, seq, Simulator::Now() - entry->time, hopCount);
	  }

	  entry = m_seqFullDelay.find(seq);
	  if (entry != m_seqFullDelay.end()) {
	    m_firstInterestDataDelay(this, seq, Simulator::Now() - entry->time, m_seqRetxCounts[seq], hopCount);
	  }

	  m_seqRetxCounts.erase(seq);
	  m_seqFullDelay.erase(seq);
	  m_seqLastDelay.erase(seq);

	  m_seqTimeouts.erase(seq);
	  m_retxSeqs.erase(seq);

	  m_rtt->AckSeq(SequenceNumber32(seq));
}

void
ConsumerHash::SendPacket(){
	 if (!m_active)
	    return;

	  NS_LOG_FUNCTION_NOARGS();

	  uint32_t seq = std::numeric_limits<uint32_t>::max(); // invalid

	  while (m_retxSeqs.size()) {
	    seq = *m_retxSeqs.begin();
	    m_retxSeqs.erase(m_retxSeqs.begin());
	    break;
	  }

	  if (seq == std::numeric_limits<uint32_t>::max()) {
	    if (m_seqMax != std::numeric_limits<uint32_t>::max()) {
	      if (m_seq >= m_seqMax) {
	        return; // we are totally done
	      }
	    }

	    seq = m_seq++;
	  }


	  std::string str=std::to_string(consumerID);
	  shared_ptr<Name> nameWithSequence = make_shared<Name>(str);
	  nameWithSequence->appendSequenceNumber(seq);


	  // shared_ptr<Interest> interest = make_shared<Interest> ();
	  shared_ptr<Interest> interest = make_shared<Interest>();
	  interest->setNonce(m_rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
	  interest->setName(*nameWithSequence);
	  interest->setHashFlag(11);
	  interest->setHashTTL(4);
	  interest->setConsumerID(this->getConsumerId());

	  NS_LOG_DEBUG("consumerID" <<interest->getConsumerID() );
	  time::milliseconds interestLifeTime(m_interestLifeTime.GetMilliSeconds());
	  interest->setInterestLifetime(interestLifeTime);

	  // NS_LOG_INFO ("Requesting Interest: \n" << *interest);
	  NS_LOG_INFO("> Interest for " << seq);

	  WillSendOutInterest(seq);

	  m_transmittedInterests(interest, this, m_face);
	  m_appLink->onReceiveInterest(*interest);



	  ScheduleNextPacket();
}

std::size_t
ConsumerHash::getConsumerId(){
	return consumerID;
}

void
ConsumerHash::ScheduleNextPacket(){
	 // double mean = 8.0 * m_payloadSize / m_desiredRate.GetBitRate ();
	  // std::cout << "next: " << Simulator::Now().ToDouble(Time::S) + mean << "s\n";

	  if (m_firstTime) {
	    m_sendEvent = Simulator::Schedule(Seconds(0.0), &ConsumerHash::SendPacket, this);
	    m_firstTime = false;
	  }
	  else if (!m_sendEvent.IsRunning())
	    m_sendEvent = Simulator::Schedule((m_random == 0) ? Seconds(1.0 / m_frequency)
	                                                      : Seconds(m_random->GetValue()),
	                                      &ConsumerHash::SendPacket, this);
}

void
ConsumerHash::SetRandomize(const std::string& value)
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
ConsumerHash::GetRandomize() const
{
  return m_randomType;
}
/*
std::size_t
ConsumerHash::getConsumerID() const {
	return consumerID;
}

void
ConsumerHash::setConsumerID(){
	consumerID=std::hash<uint32_t>()(this->GetId());
}
*/
}
}


