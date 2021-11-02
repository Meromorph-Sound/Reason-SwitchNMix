/*
 * SwitchNMix.cpp
 *
 *  Created on: 12 Aug 2021
 *      Author: julianporter
 */

#include "SwitchNMix.hpp"

#define OLDCODE

namespace meromorph {
namespace switchnmix {

uint32 SwitchNMix::read(const port_t port,float32 *data) {
	auto ref = JBox_LoadMOMPropertyByTag(port, IN_BUFFER);
	auto length = std::min<int64>(JBox_GetDSPBufferInfo(ref).fSampleCount,BUFFER_SIZE);
	if(length>0) JBox_GetDSPBufferData(ref, 0, length, data);
	return static_cast<int32>(length);
}

void SwitchNMix::write(const port_t port,float32 *data) {
	auto refL = JBox_LoadMOMPropertyByTag(port, OUT_BUFFER);
	JBox_SetDSPBufferData(refL, 0, BUFFER_SIZE,data);
}



SwitchNMix::Mode SwitchNMix::modeIn(const port_t portL,const port_t portR) {
	auto l=toBool(JBox_LoadMOMPropertyByTag(portL,IN_CONN));
	auto r=toBool(JBox_LoadMOMPropertyByTag(portR,IN_CONN));
	return r ? STEREO : (l ? MONO : SILENT);
}

SwitchNMix::Mode SwitchNMix::modeOut(const port_t portL,const port_t portR) {
	auto l = toBool(JBox_LoadMOMPropertyByTag(portL,OUT_CONN));
	auto r = toBool(JBox_LoadMOMPropertyByTag(portR,OUT_CONN));
	return r ? STEREO : (l ? MONO : SILENT);
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



SwitchNMix::SwitchNMix() : RackExtension(), inMode(Mode::SILENT), outMode(Mode::SILENT),
		active(NUM_PORTS,false), delays(NUM_PORTS,false), factor(NUM_PORTS,1.0), kind(NUM_PORTS,Kind::SERIAL),
		carryInL(BUFFER_SIZE,NUM_PORTS,0), carryInR(BUFFER_SIZE,NUM_PORTS,0),
		carryOutL(BUFFER_SIZE,NUM_PORTS,0), carryOutR(BUFFER_SIZE,NUM_PORTS,0),
		insL(NUM_PORTS), insR(NUM_PORTS), outsL(NUM_PORTS), outsR(NUM_PORTS),
		insMode(NUM_PORTS,Mode::SILENT), outsMode(NUM_PORTS,Mode::SILENT),
		tempL(BUFFER_SIZE,0), tempR(BUFFER_SIZE,0){


	for(auto n=0;n<NUM_PORTS;n++) {
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



void SwitchNMix::reset() {
	//carryInL.assign(BUFFER_SIZE,0);
	//carryInR.assign(BUFFER_SIZE,0);
	carryInL.reset(0);
	carryInR.reset(0);
	carryOutL.reset(0);
	carryOutR.reset(0);
	shouldCheck=true;
	chunkCount=0;
	rmsL=0;
	rmsR=0;
	overloadL=0;
	overloadR=0;
}

inline bool isIn(const Tag base,const Tag value) {
	return (base<value) && (value<=base+N_PORTS);
}
inline uint32 offsetFrom(const Tag base,const Tag value) {
	return value-base-1;
}

void SwitchNMix::processApplicationMessage(const TJBox_PropertyDiff &diff) {
	Tag tag = diff.fPropertyTag;
	switch(tag) {
	case kJBox_CustomPropertiesOnOffBypass: {
		trace("On/off/bypass status change");
		break; }
	case kJBox_AudioInputConnected: {
		trace("**** Audio In  event");
		shouldCheck=true;
		break; }
	case kJBox_AudioOutputConnected: {
		trace("**** Audio Out  event");
		shouldCheck=true;
		break; }
	case Tags::GAIN: {
		trace("Gain change");
		gain = toFloat(diff.fCurrentValue);
		trace("Amplitude is ^0",gain);
		break; }
	default:
		if(isIn(Tags::CONNECT,tag)) {
			trace("CONNECT");
			auto offset=offsetFrom(Tags::CONNECT,tag);
			auto b = toBool(diff.fCurrentValue);
			kind[offset] = b ? Kind::PARALLEL : Kind::SERIAL;
		}
		else if(isIn(Tags::BYPASS,tag)) {
			trace("BYPASS");
			auto offset=offsetFrom(Tags::BYPASS,tag);
			active[offset]=toBool(diff.fCurrentValue);
		}
		else if(isIn(Tags::DRY_WET,tag)) {
			trace("DRY/WET");
			auto offset=offsetFrom(Tags::DRY_WET,tag);
			factor[offset]=toFloat(diff.fCurrentValue);
			trace("DRY/WET ^0 is ^1",offset,factor[offset]);
		}
		else if(isIn(Tags::DELAY,tag)) {
			trace("DELAY");
			auto offset=offsetFrom(Tags::DELAY,tag);
			delays[offset]=toBool(diff.fCurrentValue);
		}
		else {
			trace("Another event ^0",tag);
		}
		break;
	}




}

bool SwitchNMix::checkModeChangeIn(const uint32 n) {
	auto oldMode=insMode[n];
	insMode[n]=modeIn(insL[n],insR[n]);
	return oldMode!=insMode[n];
}

bool SwitchNMix::checkModeChangeIn() {
	auto oldMode=inMode;
	inMode=modeIn(inL,inR);
	return oldMode!=inMode;
}

bool SwitchNMix::checkModeChangeOut(const uint32 n) {
	auto oldMode=outsMode[n];
	outsMode[n]=modeOut(outsL[n],outsR[n]);
	return oldMode!=outsMode[n];
}

bool SwitchNMix::checkModeChangeOut() {
	auto oldMode=outMode;
	outMode=modeOut(outL,outR);
	return oldMode!=outMode;
}

double SwitchNMix::rms(std::vector<float32> &buffer,bool &overload) {
	auto sum = 0.f;
	std::for_each(buffer.begin(),buffer.end(),[&sum](auto v) { sum+=v*v; });
	auto r=sqrt(sum/(float32)BUFFER_SIZE);
	overload=r>1.f;
	return std::min(1.f,r);
}

bool nonZero(std::vector<float32> &buffer) {
	auto mima=std::minmax_element(buffer.begin(),buffer.end());
	return (*mima.first != 0) || (*mima.second !=0);
}



void SwitchNMix::process() {

	if(shouldCheck) {
		shouldCheck=false;
		trace("Checking connections");
		//auto inC = checkModeChangeIn();
		//auto outC = checkModeChangeOut();

		for(auto i=0;i<NUM_PORTS;i++) {
			auto inC=checkModeChangeIn(i);
			if(inC) {
				uint32 value = (insMode[i]==SILENT) ? 0 : 1;
				set(value,Tags::IN_LEDS+i+1);
			}
			auto outC=checkModeChangeOut(i);
			if(outC) {
				uint32 value = (outsMode[i]==SILENT) ? 0 : 1;
				set(value,Tags::OUT_LEDS+i+1);
			}
		}
	}

	carryInL.step();
	carryInR.step();

	carryOutL.step();
	carryOutR.step();

	read(inL,carryInL.data());
	read(inR,carryInR.data());
	carryOutL.fillPage(0);
	carryOutR.fillPage(0);

	//carryOutL.assign(BUFFER_SIZE,0);
	//carryOutR.assign(BUFFER_SIZE,0);

	// TODO: offset is in fact always a multiple of BUFFER_SIZE, so we just have a rotating array of 64-long
	// buffers.  This makes it much simpler.

	for(auto i=0;i<NUM_PORTS;i++) {
		auto fac=factor[i];
		if(active[i]) {

			if(kind[i] == Kind::SERIAL) {
				// parallel case
				// push input data through function
				write(outsL[i],carryInL.data());
				write(outsR[i],carryInR.data());

				read(insL[i],tempL.data());
				read(insR[i],tempR.data());


				for(auto n=0;n<BUFFER_SIZE;n++) {
					carryOutL(n) = carryOutL(n) + tempL[n]*fac;
					carryOutR(n) = carryOutR(n) + tempR[n]*fac;
				}
			}
			else {
				// serial case
				// input is carryIn; sum, of input and output of previous stage
				// goes through function to form next stage in
				// last stage in is outIn; sum is temp
				auto delay = delays[i] ? -i : 0;
				for(auto n=0;n<BUFFER_SIZE;n++) {
					// temp is ouput of last stage + raw input
					tempL[n] = carryOutL(n) + carryInL(delay,n)*fac;
					tempR[n] = carryOutR(n) + carryInR(delay,n)*fac;
				}


				write(outsL[i],tempL.data());
				write(outsR[i],tempR.data());

				read(insL[i],carryOutL.data());
				read(insR[i],carryOutR.data());
			}
		}
		else {
			for(auto n=0;n<BUFFER_SIZE;n++) {
				carryOutL(n) += carryInL(n)*fac;
				carryOutR(n) += carryInR(n)*fac;
			}
		}
	}
	for(auto i=0;i<BUFFER_SIZE;i++) {
		tempL[i]=carryOutL(i)*gain;
		tempR[i]=carryOutR(i)*gain;
	}
	write(outL,tempL.data());
	write(outR,tempR.data());

	if(chunkCount==0) {
		bool overload=false;
		auto lr = rms(tempL,overload);
		if(lr!=rmsL) set(lr,Tags::LEFT_VOL);
		rmsL=lr;
		if(overload!=overloadL) set(overload,Tags::LEFT_MAX);
		overloadL=overload;

		overload=false;
		auto rr = rms(tempR,overload);
		if(rr!=rmsR) set(rr,Tags::RIGHT_VOL);
		rmsR=rr;
		if(overload!=overloadR) set(overload,Tags::RIGHT_MAX);
		overloadR=overload;
	}
	chunkCount=(chunkCount+1) & 0x3;



	/*
	for(auto i=0;i<N_PORTS;i++) {
		auto thisKind = kind[i];
		bool isSerial = thisKind==Kind::SERIAL;
		bool isFirst = thisKind!=preceder;
		if(!bypassed[i]) {
			// transition actions
			// transitions can only occur at index 1 et seq
			if(i>0 && isFirst && isSerial) {
				std::copy(carryOutL.begin(),carryOutL.end(),carryInL.begin());
				std::copy(carryOutR.begin(),carryOutR.end(),carryInR.begin());
			}
			// do the write
			write(outsL[i],carryInL.data());
			write(outsR[i],carryInR.data());
			// read into temp buffer
			read(insL[i],tempL.data());
			read(insR[i],tempR.data());

			auto fac=factor[i];
			if(isSerial) { // serial case
				for(auto n=0;n<BUFFER_SIZE;n++) {
					carryInL[n]=tempL[n]*fac;
					carryInR[n]=tempR[n]*fac;
					// copy to output
					std::copy(carryInL.begin(),carryInL.end(),carryOutL.begin());
					std::copy(carryInR.begin(),carryInR.end(),carryOutR.begin());
				}
			}
			else { // parallel case
				if(isFirst) {
					for(auto n=0;n<BUFFER_SIZE;n++) {
						carryOutL[n]=tempL[n]*fac;
						carryOutR[n]=tempR[n]*fac;
					}
				}
				else {
					for(auto n=0;n<BUFFER_SIZE;n++) {
						carryOutL[n]+=tempL[n]*fac;
						carryOutR[n]+=tempR[n]*fac;
					}
				}
			}
		}
	}
	for(auto i=0;i<BUFFER_SIZE;i++) {
			tempL[i]=carryOutL[i]*gain;
			tempR[i]=carryOutR[i]*gain;
		}
	write(outL,tempL.data());
	write(outR,tempR.data());
	 */
}

}}




//}

//} /* namespace switchnmix */
//} /* namespace meromorph */
