/*
 * ndn-consumer-hash.hpp
 *
 *  Created on: 2020/09/02
 *      Author: fukudanatsuko
 */


#ifndef NDN_CONSUMER_HASH_H
#define NDN_CONSUMER_HASH_H


#include "ns3/ndnSIM/model/ndn-common.hpp"

#include "ndn-consumer.hpp"


namespace ns3{
namespace ndn{


class ConsumerHash: public Consumer{
public:
	static TypeId
	GetTypeId();

	ConsumerHash();
	virtual ~ConsumerHash();

	virtual void
	OnData(shared_ptr<const Data> contentObject);

	void
	SendPacket();

	std::size_t
	getConsumerId();

protected:
	virtual void
	ScheduleNextPacket();

	 void
	  SetRandomize(const std::string& value);

	 std::string
	   GetRandomize() const;
/*
	std::size_t
	getConsumerID() const;

	void
	setConsumerID();
*/

protected:
	 double m_frequency;
	 bool m_firstTime;
	   Ptr<RandomVariableStream> m_random;
	   std::string m_randomType;
	/* uint32_t m_signature;
	 Name m_keyLocator;*/
  	std::size_t consumerID;
};
}
}


#endif /* SRC_NDNSIM_APPS_NDN_CONSUMER_HASH_HPP_ */
