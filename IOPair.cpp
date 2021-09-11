/*
 * IOPair.cpp
 *
 *  Created on: 12 Aug 2021
 *      Author: julianporter
 */

#include "IOPair.hpp"

namespace meromorph {
namespace switchnmix {

#define baseNameIn "/audio_inputs/AudioIn"
#define baseNameOut "/audio_outputs/AudioOut"

Port::Port(const Direction direction,const char *name) : port(JBox_GetMotherboardObjectRef(name)), connected(false) {
	bufferKey = (direction == IN) ? IN_BUFFER : OUT_BUFFER;
	connectedKey = (direction == IN) ? IN_CONN : OUT_CONN;
}

bool Port::checkConnected() {
	connected = toBool(JBox_LoadMOMPropertyByTag(port,connectedKey));
	return connected;
}

void Port::write(float32 *data,const uint32 N) {
	auto ref = JBox_LoadMOMPropertyByTag(port, bufferKey);
	JBox_SetDSPBufferData(ref, 0, N,data);
}
uint32 Port::read(float32 *data,const uint32 N) {
	auto ref = JBox_LoadMOMPropertyByTag(port, bufferKey);
	auto length = std::min<int64>(JBox_GetDSPBufferInfo(ref).fSampleCount,N);
	if(length>0) JBox_GetDSPBufferData(ref, 0, length, data);
	return static_cast<int32>(length);
}




} /* namespace switchnmix */
} /* namespace meromorph */
