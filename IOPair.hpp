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

using data_t = std::vector<float32>;

class Port {
protected:
	static const uint32 IN_BUFFER = kJBox_AudioInputBuffer;
	static const uint32 OUT_BUFFER = kJBox_AudioOutputBuffer;
	static const uint32 IN_CONN = kJBox_AudioInputConnected;
	static const uint32 OUT_CONN = kJBox_AudioOutputConnected;

	enum PortChannel {
		LEFT = 0,
		RIGHT = 1
	};

	enum PortDirection {
		INPUT = 0,
		OUTPUT = 1
	};
public:
	const static uint32 BUFFER_SIZE = 64;
};

struct IOData {
	using iterator=data_t::iterator;
	using pair=std::pair<float32,float32>;

	data_t left;
	data_t right;

	IOData() : left(Port::BUFFER_SIZE,0), right(Port::BUFFER_SIZE,0) {}
	virtual ~IOData() = default;

	uint32 size() const { return Port::BUFFER_SIZE; }
	void clear() {
		left.assign(Port::BUFFER_SIZE,0);
		right.assign(Port::BUFFER_SIZE,0);
	}

	float32 *lData() { return left.data(); }
	float32 *rData() { return right.data(); }

	iterator lBegin() { return left.begin(); }
	iterator lEnd() { return left.end(); }

	iterator rBegin() { return right.begin(); }
	iterator rEnd() { return right.end(); }
};




class IOPair : public Port {
public:
	enum Mode {
			SILENT,
			MONO,
			STEREO
		};
private:
	PortDirection direction;


	void portName(char *tmp,const PortChannel channel,const uint32 N);
protected:
	TJBox_ObjectRef left;
	TJBox_ObjectRef right;
	Mode mode = SILENT;
	uint32 CONN;
	uint32 BUFFER;
public:

	IOPair(const PortDirection d,const uint32 N);
	IOPair(const PortDirection d);
	virtual ~IOPair() = default;

	Mode checkMode();
	Mode getMode() const { return mode; }

};

class IOPairIn : public IOPair {
private:
	uint32 readChannel(TJBox_ObjectRef ch,data_t &buffer);
public:
	IOPairIn(const uint32 N) : IOPair(INPUT,N) {};
	IOPairIn() : IOPair(INPUT) {};
	virtual ~IOPairIn() = default;

	uint32 read(data_t &bufferL,data_t &buffer_r);
};

class IOPairOut : public IOPair {
private:
	void writeChannel(TJBox_ObjectRef ch,data_t &buffer);

public:
	IOPairOut(const uint32 N) : IOPair(OUTPUT,N) {};
	IOPairOut() : IOPair(OUTPUT) {};
	virtual ~IOPairOut() = default;

	void write(data_t &bufferL,data_t &buffer_r);
};










} /* namespace switchnmix */
} /* namespace meromorph */

#endif /* IOPAIR_HPP_ */
