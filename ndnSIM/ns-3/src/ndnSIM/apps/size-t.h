/*
 * size-t.h
 *
 *  Created on: 2020/09/23
 *      Author: fukudanatsuko
 */

#ifndef SIZE_T_H_
#define SIZE_T_H_


#include "ns3/attribute.h"
#include "ns3/attribute-helper.h"
#include <stdio.h>
#include <stddef.h>

namespace ns3{

ATTRIBUTE_VALUE_DEFINE_WITH_NAME (std::size_t, Size_t);
ATTRIBUTE_ACCESSOR_DEFINE (Size_t);

template <typename T>
Ptr<const AttributeChecker> MakeSize_tChecker (void);

template <typename T>
Ptr<const AttributeChecker> MakeSize_tChecker (std::size_t min);

template <typename T>
Ptr<const AttributeChecker> MakeSize_tChecker (std::size_t min, std::size_t max);

}

#include"ns3/type-name.h"

namespace ns3{

namespace internal{
Ptr<const AttributeChecker> MakeSize_tChecker (std::size_t min, std::size_t max, std::string name);

}

template <typename T>
Ptr<const AttributeChecker> MakeSize_tChecker (void)
{
  return internal::MakeUintegerChecker (std::numeric_limits<T>::min (),
                                        std::numeric_limits<T>::max (),
                                        TypeNameGet<T> ());
}

template <typename T>
Ptr<const AttributeChecker> MakeSize_tChecker (std::size_t min)
{
  return internal::MakeUintegerChecker (min,
                                        std::numeric_limits<T>::max (),
                                        TypeNameGet<T> ());
}

template <typename T>
Ptr<const AttributeChecker> MakeSize_tChecker (std::size_t min, std::size_t max)
{
  return internal::MakeUintegerChecker (min,
                                        max,
                                        TypeNameGet<T> ());
}

}

#endif /* SRC_CORE_MODEL_SIZE_T_H_ */
