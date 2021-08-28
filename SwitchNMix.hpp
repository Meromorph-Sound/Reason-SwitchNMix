/*
 * SwitchNMix.hpp
 *
 *  Created on: 12 Aug 2021
 *      Author: julianporter
 */

#ifndef SWITCHNMIX_HPP_
#define SWITCHNMIX_HPP_

#include "base.hpp"
#include "RackExtension.hpp"
#include "IOPair.hpp"
#include "Block.hpp"
#include <memory>

namespace meromorph {
namespace switchnmix {


enum Tags : uint32 {
	GAIN=1,
	DRY_WET=10,
	BYPASS=20,
	CONNECT=30
};

#define N_PORTS 4

using port_t = TJBox_ObjectRef;

class SwitchNMix: public RackExtension {
private:
	static const inline uint32 NUM_PORTS = 4;

	port_t inL, inR;
	port_t outL, outR;

	bool *bypassed;
	float32 *factor;
	Block::Kind *kind;

	port_t *insL, *insR;
	port_t *outsL, *outsR;

	std::vector<float32> carryInL, carryInR;
	std::vector<float32> carryOutL, carryOutR;
	std::vector<float32> tempL, tempR;


	float32 gain=1;

	static uint32 read(const port_t,float32 *);
	static void write(const port_t,float32 *);

	void process(const uint32 N);




protected:
	virtual void processApplicationMessage(const TJBox_PropertyDiff &diff);
	virtual void process();
	virtual void reset();

public:
	SwitchNMix();
	virtual ~SwitchNMix();
	SwitchNMix(const SwitchNMix &other) = default;
	SwitchNMix & operator=(const SwitchNMix &other) = default;
};

} /* namespace switchnmix */
} /* namespace meromorph */

#endif /* SWITCHNMIX_HPP_ */
