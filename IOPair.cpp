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

void IOPair::portName(char *tmp,const PortChannel channel,const uint32 N) {
			char ext[3];

			if(direction==PortDirection::INPUT) strcpy(tmp,baseNameIn);
			else strcpy(tmp,baseNameOut);

			ext[0]=(channel==PortChannel::LEFT) ? 'L' : 'R';
			ext[1]=(N>0) ? '0'+(char)N : '\0';
			ext[2]='\0';
			strcat(tmp,ext);
		}

IOPair::IOPair(const PortDirection d,const uint32 N) : direction(d) {
	char tmp[80];
	portName(tmp,LEFT,N);
	left=JBox_GetMotherboardObjectRef(tmp);
	portName(tmp,RIGHT,N);
	right=JBox_GetMotherboardObjectRef(tmp);

	CONN = (direction==PortDirection::INPUT) ? IN_CONN : OUT_CONN;
	BUFFER = (direction==PortDirection::INPUT) ? IN_BUFFER : OUT_BUFFER;
}

IOPair::IOPair(const PortDirection d) : IOPair(d,0) {}

IOPair::Mode IOPair::checkMode() {
	auto l=toInt(JBox_LoadMOMPropertyByTag(left,CONN));
	auto r=toInt(JBox_LoadMOMPropertyByTag(right,CONN));

	if(l && r) mode = STEREO;
	else if(l) mode = MONO;
	else mode = SILENT;
	return mode;
}

void IOPairOut::writeChannel(TJBox_ObjectRef ch,data_t &buffer) {
	auto refL = JBox_LoadMOMPropertyByTag(ch, BUFFER);
	JBox_SetDSPBufferData(refL, 0, buffer.size(),buffer.data());
}

void IOPairOut::write(data_t &bufferL,data_t &bufferR) {
	switch(mode) {
	case MONO:
		writeChannel(left,bufferL);
		break;
	case STEREO:
		writeChannel(left,bufferL);
		writeChannel(right,bufferR);
		break;
	default:
		break;
	}
}


uint32 IOPairIn::readChannel(TJBox_ObjectRef ch,data_t &buffer) {
	auto ref = JBox_LoadMOMPropertyByTag(ch, BUFFER);
	auto length = std::min<int64>(JBox_GetDSPBufferInfo(ref).fSampleCount,BUFFER_SIZE);
	if(length>0) JBox_GetDSPBufferData(ref, 0, length, buffer.data());
	return static_cast<int32>(length);
}

uint32 IOPairIn::read(data_t &bufferL,data_t &bufferR) {
	switch(mode) {
	case MONO:
		return readChannel(left,bufferL) > 0;
		break;
	case STEREO: {
		auto l=readChannel(left,bufferL) > 0;
		auto r=readChannel(right,bufferR) > 0;
		return l && r;
		break; }
	default:
		return false;
	}
}



} /* namespace switchnmix */
} /* namespace meromorph */
