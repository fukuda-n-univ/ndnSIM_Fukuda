/*
 * size-t.cc
 *
 *  Created on: 2020/09/23
 *      Author: fukudanatsuko
 */


#include "size-t.h"

#include "fatal-error.h"
#include "log.h"
#include <sstream>


namespace ns3{

NS_LOG_COMPONENT_DEFINE ("Size_t");

ATTRIBUTE_VALUE_IMPLEMENT_WITH_NAME (std::size_t,Size_t);

namespace internal{

Ptr<const AttributeChecker> MakeSize_tChecker (std::size_t min, std::size_t max, std::string name){
	NS_LOG_FUNCTION (min << max << name);
	struct Checker : public AttributeChecker
	  {
	    Checker (std::size_t minValue, std::size_t maxValue, std::string name)
	      : m_minValue (minValue),
	        m_maxValue (maxValue),
	        m_name (name) {}
	    virtual bool Check (const AttributeValue &value) const {
	      NS_LOG_FUNCTION (&value);
	      const Size_tValue *v = dynamic_cast<const Size_tValue *> (&value);
	      if (v == 0)
	        {
	          return false;
	        }
	      return v->Get () >= m_minValue && v->Get () <= m_maxValue;
	    }
	    virtual std::string GetValueTypeName (void) const {
	      NS_LOG_FUNCTION_NOARGS ();
	      return "ns3::Size_tValue";
	    }
	    virtual bool HasUnderlyingTypeInformation (void) const {
	      NS_LOG_FUNCTION_NOARGS ();
	      return true;
	    }
	    virtual std::string GetUnderlyingTypeInformation (void) const {
	      NS_LOG_FUNCTION_NOARGS ();
	      std::ostringstream oss;
	      oss << m_name << " " << m_minValue << ":" << m_maxValue;
	      return oss.str ();
	    }
	    virtual Ptr<AttributeValue> Create (void) const {
	      NS_LOG_FUNCTION_NOARGS ();
	      return ns3::Create<Size_tValue> ();
	    }
	    virtual bool Copy (const AttributeValue &source, AttributeValue &destination) const {
	      NS_LOG_FUNCTION (&source << &destination);
	      const Size_tValue *src = dynamic_cast<const Size_tValue *> (&source);
	      Size_tValue *dst = dynamic_cast<Size_tValue *> (&destination);
	      if (src == 0 || dst == 0)
	        {
	          return false;
	        }
	      *dst = *src;
	      return true;
	    }
	    std::size_t m_minValue;
	    std::size_t m_maxValue;
	    std::string m_name;
	  } *checker = new Checker (min, max, name);
	  return Ptr<const AttributeChecker> (checker, false);

}

}

}

