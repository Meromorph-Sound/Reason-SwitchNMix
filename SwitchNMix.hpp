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
	const static uint32 BUFFER_SIZE = 64;

	static const uint32 IN_BUFFER = kJBox_AudioInputBuffer;
	static const uint32 OUT_BUFFER = kJBox_AudioOutputBuffer;
	static const uint32 IN_CONN = kJBox_AudioInputConnected;
	static const uint32 OUT_CONN = kJBox_AudioOutputConnected;

	enum Kind {
			SERIAL,
			PARALLEL,
			INITIAL
		};

	enum Mode {
				SILENT,
				MONO,
				STEREO
			};

	port_t inL, inR;
	port_t outL, outR;

	Mode inMode;
	Mode outMode;

	bool *bypassed;
	float32 *factor;
	Kind *kind;
	bool *first;

	port_t *insL, *insR;
	port_t *outsL, *outsR;

	Mode *insMode;
	Mode *outsMode;

	std::vector<float32> carryInL, carryInR;
	std::vector<float32> carryOutL, carryOutR;
	std::vector<float32> tempL, tempR;


	float32 gain=1;
	bool shouldCheck=true;
	bool shouldRecalculate=true;

	static uint32 read(const port_t,float32 *);
	static void write(const port_t,float32 *);

	static Mode modeIn(const port_t,const port_t);
	static Mode modeOut(const port_t,const port_t);

	void process(const uint32 N);
	void checkConnections();
	void recalculate();




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
