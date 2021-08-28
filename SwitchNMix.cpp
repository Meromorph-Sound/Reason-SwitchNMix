/*
 * SwitchNMix.cpp
 *
 *  Created on: 12 Aug 2021
 *      Author: julianporter
 */

#include "SwitchNMix.hpp"

namespace meromorph {
namespace switchnmix {

uint32 SwitchNMix::read(const port_t port,float32 *data) {
	auto ref = JBox_LoadMOMPropertyByTag(port, kJBox_AudioInputBuffer);
	auto length = std::min<int64>(JBox_GetDSPBufferInfo(ref).fSampleCount,BUFFER_SIZE);
	if(length>0) JBox_GetDSPBufferData(ref, 0, length, data);
	return static_cast<int32>(length);
}

void SwitchNMix::write(const port_t port,float32 *data) {
	auto refL = JBox_LoadMOMPropertyByTag(port, kJBox_AudioOutputBuffer);
	JBox_SetDSPBufferData(refL, 0, BUFFER_SIZE,data);
}

void portName(char *tmp,const char channel,const uint32 N,const char *baseName) {
			char ext[3];

			strcpy(tmp,baseName);
			ext[0]=channel;
			ext[1]=(N>0) ? '0'+(char)N : '\0';
			ext[2]='\0';
			strcat(tmp,ext);
}

port_t getPort(const char channel,const uint32 N,const bool in) {
	char tmp[160];
	if(in) portName(tmp,channel,N,"/audio_inputs/AudioIn");
	else portName(tmp,channel,N,"/audio_outputs/AudioOut");
	return JBox_GetMotherboardObjectRef(tmp);
}




SwitchNMix::SwitchNMix() : RackExtension(),
				carryInL(IOPair::BUFFER_SIZE,0), carryInR(IOPair::BUFFER_SIZE,0),
		carryOutL(IOPair::BUFFER_SIZE,0), carryOutR(IOPair::BUFFER_SIZE,0),
		tempL(IOPair::BUFFER_SIZE,0), tempR(IOPair::BUFFER_SIZE,0){

	bypassed = new bool[NUM_PORTS];
	factor = new float32[NUM_PORTS];
	kind = new Block::Kind[NUM_PORTS];

	insL = new port_t[NUM_PORTS];
	insR = new port_t[NUM_PORTS];
	outsL = new port_t[NUM_PORTS];
	outsR = new port_t[NUM_PORTS];

	for(auto n=0;n<NUM_PORTS;n++) {
		bypassed[n]=false;
		factor[n]=1.0;
		kind[n]=Block::Kind::SERIAL;

		insL[n] = getPort('L',n+1,true);
		insR[n] = getPort('R',n+1,true);
		outsL[n] = getPort('L',n+1,false);
		outsR[n] = getPort('R',n+1,false);
	}

	inL = getPort('L',0,true);
	inR = getPort('R',0,true);
	outL = getPort('L',0,false);
	outR = getPort('R',0,false);

}

SwitchNMix::~SwitchNMix() {
	delete [] bypassed;
	delete [] factor;
	delete [] kind;

	delete [] insL;
	delete [] insR;
	delete [] outsL;
	delete [] outsR;
}

bool didConnect(TJBox_Value value) {
	if(JBox_GetType(value)==kJBox_Boolean) {
		return JBox_GetBoolean(value) != 0;
	}
	else {
		trace("Bad value in Boolean conversion");
		return false;
	}
}


void SwitchNMix::reset() {
	carryInL.assign(IOPair::BUFFER_SIZE,0);
	carryInR.assign(IOPair::BUFFER_SIZE,0);
	carryOutL.assign(IOPair::BUFFER_SIZE,0);
	carryOutR.assign(IOPair::BUFFER_SIZE,0);
}


void SwitchNMix::processApplicationMessage(const TJBox_PropertyDiff &diff) {
	Tag tag = diff.fPropertyTag;
	switch(tag) {
	case kJBox_CustomPropertiesOnOffBypass: {
			trace("On/off/bypass status change");
			break; }
	case kJBox_AudioInputConnected: {
		trace("**** Audio In  event");
		bool value = didConnect(diff.fCurrentValue);
		if(value) trace("Connected");
		else trace("Disconnected");
		break; }
	case kJBox_AudioOutputConnected: {
		trace("**** Audio Out  event");
		bool value = didConnect(diff.fCurrentValue);
		if(value) trace("Connected");
		else trace("Disconnected");
		break; }
	case Tags::GAIN: {
		trace("Gain change");
		gain = toFloat(diff.fCurrentValue);
		trace("Amplitude is ^0",gain);
		break; }
	default:
		trace("ANOther event ^0",tag);
		if(tag>Tags::CONNECT) {
			trace("CONNECT");
			auto offset=tag-Tags::CONNECT;
			if(offset>0 && offset<=N_PORTS) {
				auto b = toBool(diff.fCurrentValue);
				kind[offset-1] = b ? Block::Kind::SERIAL : Block::Kind::PARALLEL;
			}
		}
		else if(tag>Tags::BYPASS) {
			trace("BYPASS");
			auto offset=tag-Tags::BYPASS;
			if(offset>0 && offset<=N_PORTS) {
				bypassed[offset-1]=toBool(diff.fCurrentValue);
			}
		}
		else if(tag>Tags::DRY_WET) {
			trace("DRY/WET");
			auto offset=tag-Tags::DRY_WET;
			if(offset>0 && offset<=N_PORTS) {
				factor[offset-1]=toFloat(diff.fCurrentValue);
				trace("DRY/WET ^0 is ^1",offset-1,factor[offset-1]);
			}
		}
		break;
	}
	/*
	if(needsCompute) {
		trace("Computing");
		blocks[0]->setFirst(true);
		for(auto n=1;n<NUM_PORTS;n++) {
			auto k1=blocks[n]->getKind();
			auto k2=blocks[n-1]->getKind();
			if(blocks[n-1]->isBypassed()) blocks[n]->setFirst((k1!=k2)^blocks[n-1]->first());
			else blocks[n]->setFirst(k1!=k2);
		}
		trace("Displaying ^0 blocks",blocks.size());
		for(auto it=blocks.begin();it!=blocks.end();it++) {
			auto b=*it;
			trace(" Gain ^0",b->getFactor());
			trace(" Bypassed ^0",b->isBypassed());
			trace(" Kind ^0 First ^1",b->getKind(),b->first());
		}
	}
	initialised=true;
	*/

}






void SwitchNMix::process() {
	read(inL,carryInL.data());
		read(inR,carryInR.data());
		for(auto i=0;i<N_PORTS;i++) {
			if(!bypassed[i]) {
				write(outsL[i],carryInL.data());
				write(outsR[i],carryInR.data());

				read(insL[i],tempL.data());
				read(insR[i],tempR.data());
				for(auto n=0;n<IOPair::BUFFER_SIZE;n++) {
					carryInL[n]=tempL[n]*factor[i];
					carryInR[n]=tempR[n]*factor[i];
				}
			}
		}
		write(outL,carryInL.data());
		write(outR,carryInR.data());
}

} /* namespace switchnmix */
} /* namespace meromorph */
