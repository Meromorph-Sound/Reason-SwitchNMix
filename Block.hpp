/*
 * Block.hpp
 *
 *  Created on: 28 Aug 2021
 *      Author: julianporter
 */

#ifndef BLOCK_HPP_
#define BLOCK_HPP_

#include "base.hpp"
#include "IOPair.hpp"

namespace meromorph { namespace switchnmix {

class Block {
public:
	enum Kind {
		SERIAL,
		PARALLEL
	};
private:
	uint32 N;
	IOPairIn input;
	IOPairOut output;

	data_t bufferL, bufferR;
	IOPair::Mode modeIn, modeOut;
	bool bypassed=false;
	float32 scale=1.0;
	Kind kind = SERIAL;
	bool isFirst = false;
	void checkMode();

public:


	Block(const uint32 N_) : N(N_),
		input(N), output(N),
		bufferL(Port::BUFFER_SIZE,0), bufferR(Port::BUFFER_SIZE,0),
		modeIn(IOPair::Mode::SILENT), modeOut(IOPair::Mode::SILENT) {}
	virtual ~Block()=default;


	void process(
			std::vector<float32> &dataL,std::vector<float32> &dataR,
			std::vector<float32> &carryL,std::vector<float32> &carryR);

	void setBypass(const bool b) { bypassed=b; }
	void setFirst(const bool b) { isFirst=b; }
	void setScale(const float32 s) { scale=s; }
	void setKind(const Kind k) { kind=k; }

	Kind getKind() const { return kind; }
	bool isBypassed() const { return bypassed; }
	bool first() const { return isFirst; }
	float32 getFactor() const { return scale; }

	static void calculate(const uint32 N,Kind *kinds,bool *bypassed, bool *first);

};
}}

#endif /* BLOCK_HPP_ */
