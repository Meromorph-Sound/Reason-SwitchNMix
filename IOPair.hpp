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

#ifdef USE_IOPAIR

struct BufferPair {
	std::vector<float32> left;
	std::vector<float32> right;

	BufferPair() : left(), right() {};
	BufferPair(const uint32 N,const float32 value) : left(N,value), right(N,value) {};
	virtual ~BufferPair() = default;

	void assign(const uint32 N,const float32 value) {
		left.assign(N,value);
		right.assign(N,value);
	}
};

using port_t = TJBox_ObjectRef;
using data_t = std::vector<float32>;

class Port {
private:
	static const uint32 IN_BUFFER = kJBox_AudioInputBuffer;
		static const uint32 OUT_BUFFER = kJBox_AudioOutputBuffer;
		static const uint32 IN_CONN = kJBox_AudioInputConnected;
		static const uint32 OUT_CONN = kJBox_AudioOutputConnected;

	port_t portL;
	port_t portR;
	uint32 bufferKey;
	uint32 connectedKey;

	void write(const port_t port,float32 *data,const uint32 N=64);
	uint32 read(const port_t port,float32 *data,const uint32 N=64);

public:
	enum Direction {
		IN,
		OUT
	};
	enum Channel : char {
		LEFT = 'L',
		RIGHT = 'R'
	};
	enum Mode {
		SILENT,
		MONO,
		STEREO
	};

	static port_t getPort(const Direction direction,const Channel channel,const uint32 N=0);

	Port(const Direction direction,const uint32 N=0);
	virtual ~Port() = default;

	Mode checkConnected();

	void write(BufferPair &buffer,const uint32 N=64);
	void read(BufferPair &buffer,const uint32 N=64);
};



#endif

} /* namespace switchnmix */
} /* namespace meromorph */



#endif /* IOPAIR_HPP_ */
