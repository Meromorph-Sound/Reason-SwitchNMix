/*
 * IOPair.hpp
 *
 *  Created on: 12 Aug 2021
 *      Author: julianporter
 */

#ifndef IOPAIR_HPP_
#define IOPAIR_HPP_

#include "base.hpp"
#include "Jukebox.h"

namespace meromorph {
namespace switchnmix {

using port_t = TJBox_ObjectRef;
using data_t = std::vector<float32>;

class Port {
private:
	static const uint32 IN_BUFFER = kJBox_AudioInputBuffer;
		static const uint32 OUT_BUFFER = kJBox_AudioOutputBuffer;
		static const uint32 IN_CONN = kJBox_AudioInputConnected;
		static const uint32 OUT_CONN = kJBox_AudioOutputConnected;

	port_t port;
	uint32 bufferKey;
	uint32 connectedKey;

	bool connected;

public:
	enum Direction {
		IN,
		OUT
	};

	Port(const Direction direction,const char *name);
	virtual ~Port() = default;

	bool isConnected() const { return connected; }
	bool checkConnected();

	void write(float32 *data,const uint32 N=64);
	uint32 read(float32 *data,const uint32 N=64);
};





} /* namespace switchnmix */
} /* namespace meromorph */

#endif /* IOPAIR_HPP_ */
