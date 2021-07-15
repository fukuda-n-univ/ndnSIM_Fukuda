/*
 * cs-inface.hpp
 *
 *  Created on: 2019/11/22
 *      Author: fukudanatsuko
 */

#ifndef NFD_DAEMON_TABLE_CS_INFACE_HPP_
#define NFD_DAEMON_TABLE_CS_INFACE_HPP_

#include "core/common.hpp"
#include "face/face.hpp"

namespace nfd{
namespace cs{

class InFace{
public:
	explicit
	InFace(Face& face);

	Face&
	getFace() const{
		return *m_face;
	}



private:
	Face* m_face;
};
}
}



#endif /* SRC_NDNSIM_NFD_DAEMON_TABLE_CS_INFACE_HPP_ */
