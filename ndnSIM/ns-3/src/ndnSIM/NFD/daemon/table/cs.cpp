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

#include "cs.hpp"
#include "core/logger.hpp"
#include "core/algorithm.hpp"
#include <ndn-cxx/lp/tags.hpp>

#include "cs-entry-impl.hpp"
#include "face/null-face.hpp"

#include<functional>
#include<string>
#include<cmath>

NFD_LOG_INIT("ContentStore");

namespace nfd {
namespace cs {

// http://en.cppreference.com/w/cpp/concept/ForwardIterator
BOOST_CONCEPT_ASSERT((boost::ForwardIterator<Cs::const_iterator>));
// boost::ForwardIterator follows SGI standard http://www.sgi.com/tech/stl/ForwardIterator.html,
// which doesn't require DefaultConstructible
#ifdef HAVE_IS_DEFAULT_CONSTRUCTIBLE
static_assert(std::is_default_constructible<Cs::const_iterator>::value,
              "Cs::const_iterator must be default-constructible");
#else
BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<Cs::const_iterator>));
#endif // HAVE_IS_DEFAULT_CONSTRUCTIBLE

unique_ptr<Policy>
makeDefaultPolicy()
{
  const std::string DEFAULT_POLICY = "priority_fifo";
  return Policy::create(DEFAULT_POLICY);
}

Cs::Cs(size_t nMaxPackets, unique_ptr<Policy> policy)
{
  this->setPolicyImpl(std::move(policy));
  m_policy->setLimit(nMaxPackets);
}

void
Cs::setLimit(size_t nMaxPackets)
{
  m_policy->setLimit(nMaxPackets);
}

size_t
Cs::getLimit() const
{
  return m_policy->getLimit();
}

void
Cs::setPolicy(unique_ptr<Policy> policy)
{
  BOOST_ASSERT(policy != nullptr);
  BOOST_ASSERT(m_policy != nullptr);
  size_t limit = m_policy->getLimit();
  this->setPolicyImpl(std::move(policy));
  m_policy->setLimit(limit);
}

void
Cs::insert(const Data& data, bool isUnsolicited)
{
  NFD_LOG_DEBUG("insert " << data.getName());

  if (m_policy->getLimit() == 0) {
    // shortcut for disabled CS
    return;
  }

  // recognize CachePolicy
  shared_ptr<lp::CachePolicyTag> tag = data.getTag<lp::CachePolicyTag>();
  if (tag != nullptr) {
    lp::CachePolicyType policy = tag->get().getPolicy();
    if (policy == lp::CachePolicyType::NO_CACHE) {
      return;
    }
  }

  bool isNewEntry = false;
  iterator it;
  // use .insert because gcc46 does not support .emplace
  std::tie(it, isNewEntry) = m_table.insert(EntryImpl(data.shared_from_this(), isUnsolicited));
  EntryImpl& entry = const_cast<EntryImpl&>(*it);

  entry.updateStaleTime();

  if (!isNewEntry) { // existing entry
    // XXX This doesn't forbid unsolicited Data from refreshing a solicited entry.
    if (entry.isUnsolicited() && !isUnsolicited) {
      entry.unsetUnsolicited();
    }

    m_policy->afterRefresh(it);
  }
  else {
    m_policy->afterInsert(it);
  }
}

void
Cs::find(const Interest& interest,
         const HitCallback& hitCallback,
         const MissCallback& missCallback) const
{
  BOOST_ASSERT(static_cast<bool>(hitCallback));
  BOOST_ASSERT(static_cast<bool>(missCallback));

  const Name& prefix = interest.getName();
  bool isRightmost = interest.getChildSelector() == 1;
  NFD_LOG_DEBUG("find " << prefix << (isRightmost ? " R" : " L"));

  iterator first = m_table.lower_bound(prefix);
  iterator last = m_table.end();
  if (prefix.size() > 0) {
    last = m_table.lower_bound(prefix.getSuccessor());
  }

  iterator match = last;
  if (isRightmost) {
    match = this->findRightmost(interest, first, last);
  }
  else {
    match = this->findLeftmost(interest, first, last);
  }

  if (match == last) {
    NFD_LOG_DEBUG("  no-match");
    missCallback(interest);
    return;
  }
  NFD_LOG_DEBUG("  matching " << match->getName());
  m_policy->beforeUse(match);
  hitCallback(interest, match->getData());
}

iterator
Cs::findLeftmost(const Interest& interest, iterator first, iterator last) const
{
  return std::find_if(first, last, bind(&cs::EntryImpl::canSatisfy, _1, interest));
}

iterator
Cs::findRightmost(const Interest& interest, iterator first, iterator last) const
{
  // Each loop visits a sub-namespace under a prefix one component longer than Interest Name.
  // If there is a match in that sub-namespace, the leftmost match is returned;
  // otherwise, loop continues.

  size_t interestNameLength = interest.getName().size();
  for (iterator right = last; right != first;) {
    iterator prev = std::prev(right);

    // special case: [first,prev] have exact Names
    if (prev->getName().size() == interestNameLength) {
      NFD_LOG_TRACE("  find-among-exact " << prev->getName());
      iterator matchExact = this->findRightmostAmongExact(interest, first, right);
      return matchExact == right ? last : matchExact;
    }

    Name prefix = prev->getName().getPrefix(interestNameLength + 1);
    iterator left = m_table.lower_bound(prefix);

    // normal case: [left,right) are under one-component-longer prefix
    NFD_LOG_TRACE("  find-under-prefix " << prefix);
    iterator match = this->findLeftmost(interest, left, right);
    if (match != right) {
      return match;
    }
    right = left;
  }
  return last;
}

iterator
Cs::findRightmostAmongExact(const Interest& interest, iterator first, iterator last) const
{
  return find_last_if(first, last, bind(&EntryImpl::canSatisfy, _1, interest));
}

const bool
Cs::isfindData(const Data& data) const{
	const Name& prefix=data.getName();

	iterator first=m_table.lower_bound(prefix);
	iterator last=m_table.end();
	iterator match=last;

	if(first!=last){
		for(auto itr=first;itr!=last;itr++){
			if(itr->getName()==data.getName()){
				match=itr;
				break;
			}
		}
	}

	if(match==last){
		return false;
	}else{
		return true;
	}
}

bool
Cs::isEmpty() const{
	iterator first=m_table.begin();
	iterator last=m_table.end();
	if(first==last){
		return true;
	}else{
		return false;
	}
}

const Data&
Cs::searchNearestConsumerIDData(const Interest& interest) const{

		const std::size_t consumerID=interest.getConsumerID();

		iterator first=m_table.begin();
		iterator last=m_table.end();

		iterator nearestHashData;

		std::size_t leastDeviation=SIZE_MAX-1;
		std::size_t tmp=0;
		std::size_t tmp_dev=0;

	  if(first!=last){
		for(auto itr=first; itr!=last; itr++){
			tmp=itr->getHash();

			if(consumerID>tmp){
				tmp_dev=consumerID-tmp;
			}else{
				tmp_dev=tmp-consumerID;
			}

			if(tmp_dev < leastDeviation){
				leastDeviation=tmp_dev;
				nearestHashData=itr;
			}
		 }
	  }

		NFD_LOG_DEBUG("data having nearest hash="<<nearestHashData->getName());
		return nearestHashData->getData();


}

bool
Cs::checkForConsumerID(const Interest& interest) const{

	if(this->isEmpty()==true){
		return false;
	}

	const Data& nearestHashData=this->searchNearestConsumerIDData(interest);
	const std::size_t hashNum=std::hash<std::string>()(nearestHashData.getName().toUri());

	const std::size_t range=SIZE_MAX/20;
	const std::size_t consumerID=interest.getConsumerID();

	if(consumerID<range){
		if(hashNum<=(consumerID+range)){
			return true;
		}else{
			return false;
		}
	}else if(consumerID>(SIZE_MAX-1-range)){
		if(hashNum>=(consumerID-range)){
			return true;
		}else{
			return false;
		}
	}else{
		if((consumerID-range)<=hashNum && hashNum<=(consumerID+range)){
			return true;
		}else{
			return false;
		}
	}
}


const Data&
Cs::getNearConsumerIDData(const Interest& interest) const{

	const Data& nearestHashData=this->searchNearestConsumerIDData(interest);
	const std::size_t hashNum=std::hash<std::string>()(nearestHashData.getName().toUri());

	const std::size_t range=SIZE_MAX/20;
	const std::size_t consumerID=interest.getConsumerID();

	if(consumerID<range){
		if(hashNum<=(consumerID+range)){
			return nearestHashData;
		}
	}else if(consumerID>(SIZE_MAX-1-range)){
		if(hashNum>=(consumerID-range)){
			return nearestHashData;
		}
	}else{
		if((consumerID-range)<=hashNum && hashNum<=(consumerID+range)){
			return nearestHashData;
		}
	}
}



shared_ptr<lp::IncomingFaceIdTag>
Cs::searchNearestHashData(const Interest& interest) const
{

	const Name& n=interest.getName();
	const std::size_t interestHashNum=std::hash<std::string>()(n.toUri());
	bool isRightmost =interest.getChildSelector()==1;

	NFD_LOG_DEBUG("searchNearestHash"<<n<<(isRightmost ? "R":"L"));

	iterator first=m_table.begin();
	iterator last=m_table.end();

	iterator nearestHashData;
	std::size_t leastDeviation=SIZE_MAX-1;
	std::size_t tmp=0;
	std::size_t tmp_dev=0;
  if(first!=last){
	for(auto itr=first; itr!=last; itr++){
		tmp=itr->getHash();
		NFD_LOG_DEBUG(tmp);
		//tmp_dev=std::abs(interestHashNum-tmp);

		if(interestHashNum>tmp){
			tmp_dev=interestHashNum-tmp;
		}else{
			tmp_dev=tmp-interestHashNum;
		}

		if(tmp_dev < leastDeviation){
			leastDeviation=tmp_dev;
			nearestHashData=itr;
		}
	 }
  }

	NFD_LOG_DEBUG("data having nearest hash="<<nearestHashData->getName());
	const Data& d=nearestHashData->getData();

	shared_ptr<lp::IncomingFaceIdTag> incomingFaceIdTag=d.getTag<lp::IncomingFaceIdTag>();

    return incomingFaceIdTag;

}
/*
void
Cs::setOutFaceId(const Interest& interest, FaceId id){

	const Name& n=interest.getName();
	const std::size_t interestHashNum=std::hash<std::string>()(n.toUri());
	bool isRightmost =interest.getChildSelector()==1;

	NFD_LOG_DEBUG("searchNearestHash"<<n<<(isRightmost ? "R":"L"));

	iterator first=m_table.begin();
	iterator last=m_table.end();

	iterator nearestHashData;
	std::size_t leastDeviation=SIZE_MAX-1;
	std::size_t tmp=0;
	std::size_t tmp_dev=0;
  if(first!=last){
	for(auto itr=first; itr!=last; itr++){
		tmp=itr->getHash();
	//	tmp_dev=std::abs(interestHashNum-tmp);
		if(interestHashNum>tmp){
			tmp_dev=interestHashNum-tmp;

		}else{
			tmp_dev=tmp-interestHashNum;
		}

		if(tmp_dev < leastDeviation){
			leastDeviation=tmp_dev;
			nearestHashData=itr;
		}
	 }
  }

  EntryImpl entry=*nearestHashData;



  entry.setOutFaceId(id);

}*/

void
Cs::setPolicyImpl(unique_ptr<Policy> policy)
{
  NFD_LOG_DEBUG("set-policy " << policy->getName());
  m_policy = std::move(policy);
  m_beforeEvictConnection = m_policy->beforeEvict.connect([this] (iterator it) {
      m_table.erase(it);
    });

  m_policy->setCs(this);
  BOOST_ASSERT(m_policy->getCs() == this);
}

void
Cs::dump()
{
  NFD_LOG_DEBUG("dump table");
  for (const EntryImpl& entry : m_table) {
    NFD_LOG_TRACE(entry.getFullName());
  }
}

} // namespace cs
} // namespace nfd
