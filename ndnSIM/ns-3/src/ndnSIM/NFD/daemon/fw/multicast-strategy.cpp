/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2016,  Regents of the University of California,
 *                           Arizona Board of Regents,
 *                           Colorado State University,
 *                           University Pierre & Marie Curie, Sorbonne University,
 *                           Washington University in St. Louis,
 *                           Beijing Institute of Technology,
 *                           The University of Memphis.
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "multicast-strategy.hpp"
#include "algorithm.hpp"

#include <boost/random/uniform_int_distribution.hpp>
#include<random>


namespace nfd {
namespace fw {

const Name MulticastStrategy::STRATEGY_NAME("ndn:/localhost/nfd/strategy/multicast/%FD%01");
NFD_REGISTER_STRATEGY(MulticastStrategy);

MulticastStrategy::MulticastStrategy(Forwarder& forwarder, const Name& name)
  : Strategy(forwarder, name)
{
}

void
MulticastStrategy::afterReceiveInterest(const Face& inFace, const Interest& interest,
                                        const shared_ptr<pit::Entry>& pitEntry)
{
	if(interest.getName().getPrefix(10)!="localhost/"){

	  if(interest.getHashFlag()>0){
		  if(this->isCsEmpty()==false){
		 			  Face& nearestHashDataFace=this->lookupCs(*pitEntry);

		 			 if(!wouldViolateScope(inFace,interest,nearestHashDataFace)&&
		 				  canForwardToLegacy(*pitEntry,nearestHashDataFace)
						  && (nearestHashDataFace.getId() >= 256)){ // Nakazato
		 		 		  this->sendInterest(pitEntry,nearestHashDataFace,interest);

		 		 		  return;
		 			 }
		  }else{
		 			 Face* randomFace;
		 			  		int id;

		 			  		const FaceTable& faceTable=this->getFaceTable();

		 			  		int max=(int)faceTable.getLastFaceId();

		 			  		int inFaceId = inFace.getId();

		 			  		do{
		 			  			std::random_device rnd;
		 			  		    std::mt19937 mt;
		 			  			mt.seed(rnd());
		 			  			boost::random::uniform_int_distribution<> randomNumGenerator(256,max);

		 			  			id=randomNumGenerator(mt);
		 			  			randomFace=faceTable.get(id);

		 			  		}while(id == inFaceId || randomFace==nullptr || (randomFace->getRemoteUri()).getScheme() == "internal" ||randomFace->getId()==face::INVALID_FACEID);
// Nakazato
		 			  		this->sendInterest(pitEntry,*randomFace,interest);
		 			  		return;

		  }
	  }

	}
  const fib::Entry& fibEntry = this->lookupFib(*pitEntry);
  const fib::NextHopList& nexthops = fibEntry.getNextHops();

  for (fib::NextHopList::const_iterator it = nexthops.begin(); it != nexthops.end(); ++it) {
    Face& outFace = it->getFace();
    if (!wouldViolateScope(inFace, interest, outFace) &&
        canForwardToLegacy(*pitEntry, outFace)) {
      this->sendInterest(pitEntry, outFace, interest);
    }
  }

  if (!hasPendingOutRecords(*pitEntry)) {
    this->rejectPendingInterest(pitEntry);
  }
}

} // namespace fw
} // namespace nfd
