/*
 * Block.cpp
 *
 *  Created on: 28 Aug 2021
 *      Author: julianporter
 */

#include "Block.hpp"

namespace meromorph {
namespace switchnmix {

void Block::checkMode() {
	modeIn = input.checkMode();
	modeOut = output.checkMode();
}

void Block::process(data_t &dataL,data_t &dataR,
		data_t &carryL,data_t &carryR){

	if(bypassed) return;

	output.write(dataL,dataR);
	input.read(bufferL,bufferR);

	switch(kind) {
	case PARALLEL: {
		if(isFirst) {
			for(auto n=0;n<IOPair::BUFFER_SIZE;n++) {
				carryL[n]=bufferL[n]*scale;
				carryR[n]=bufferR[n]*scale;
			}
		} else {
			for(auto n=0;n<IOPair::BUFFER_SIZE;n++) {
				carryL[n]+=bufferL[n]*scale;
				carryR[n]+=bufferR[n]*scale;
			}
		}
		break; }
	case SERIAL: {
		for(auto n=0;n<IOPair::BUFFER_SIZE;n++) {
			carryL[n]=bufferL[n]*scale;
			carryR[n]=bufferR[n]*scale;
		}
		dataL.assign(carryL.begin(),carryL.end());
		dataR.assign(carryR.begin(),carryR.end());
		break; }
	}
}



void Block::calculate(const uint32 N,Kind *kinds,bool *bypassed,bool *first) {
	first[0]=true;
	for(auto n=1;n<N;n++) {
		auto k1=kinds[n];
		auto k2=kinds[n-1];
		if(bypassed[n-1]) first[n] = (k1!=k2)^first[n-1];
		else first[n]=(k1!=k2);
	}
}

}}
