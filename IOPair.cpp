/*
 * IOPair.cpp
 *
 *  Created on: 12 Aug 2021
 *      Author: julianporter
 */

#include "IOPair.hpp"

namespace meromorph {
namespace switchnmix {

#ifdef USE_IOPAIR

#define baseNameIn "/audio_inputs/AudioIn"
#define baseNameOut "/audio_outputs/AudioOut"

void portName(char *tmp,const char channel,const uint32 N,const char *baseName) {
	char ext[3];
	strcpy(tmp,baseName);
	ext[0]=channel;
	ext[1]=(N>0) ? '0'+(char)N : '\0';
	ext[2]='\0';
	strcat(tmp,ext);
}

port_t Port::getPort(const Direction direction,const Channel channel,const uint32 N) {
	char tmp[160];
	if(direction==IN) portName(tmp,static_cast<char>(channel),N,baseNameIn);
	else portName(tmp,static_cast<char>(channel),N,baseNameOut);
	return JBox_GetMotherboardObjectRef(tmp);
}


Port::Port(const Direction direction,const uint32 N) {
	bufferKey = (direction == IN) ? IN_BUFFER : OUT_BUFFER;
	connectedKey = (direction == IN) ? IN_CONN : OUT_CONN;

	portL = getPort(direction,LEFT,N);
	portR = getPort(direction,RIGHT,N);
}



Port::Mode Port::checkConnected() {
	auto l=toBool(JBox_LoadMOMPropertyByTag(portL,connectedKey));
	auto r=toBool(JBox_LoadMOMPropertyByTag(portR,connectedKey));
	return r ? STEREO : (l ? MONO : SILENT);
}

void Port::write(const port_t port,float32 *data,const uint32 N) {
	auto ref = JBox_LoadMOMPropertyByTag(port, bufferKey);
	JBox_SetDSPBufferData(ref, 0, N,data);
}

void Port::write(BufferPair &buffer,const uint32 N) {
	write(portL,buffer.left.data(),N);
	write(portR,buffer.right.data(),N);
}

uint32 Port::read(const port_t port,float32 *data,const uint32 N) {
	auto ref = JBox_LoadMOMPropertyByTag(port, bufferKey);
	auto length = std::min<int64>(JBox_GetDSPBufferInfo(ref).fSampleCount,N);
	if(length>0) JBox_GetDSPBufferData(ref, 0, length, data);
	return static_cast<int32>(length);
}

void Port::read(BufferPair &buffer,const uint32 N) {
	read(portL,buffer.left.data(),N);
	read(portR,buffer.right.data(),N);
}

#endif

} /* namespace switchnmix */
} /* namespace meromorph */
