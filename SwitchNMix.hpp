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
#include "PagedVector.hpp"
#include <memory>

namespace meromorph {
namespace switchnmix {


enum Tags : uint32 {
	GAIN=1,
	LEFT_VOL=2,
	RIGHT_VOL=3,
	LEFT_MAX=4,
	RIGHT_MAX=5,
	DRY_WET=10,
	BYPASS=20,
	CONNECT=30,
	IN_LEDS=40,
	OUT_LEDS=50,
	DELAY=60
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

	std::vector<bool> active;
	std::vector<bool> delays;
	std::vector<float32> factor;
	std::vector<Kind> kind;

	std::vector<port_t> insL, insR;
	std::vector<port_t> outsL, outsR;

	std::vector<Mode> insMode;
	std::vector<Mode> outsMode;

	AutoPagedVector<float32> carryInL, carryInR;
	//std::vector<float32> carryInL, carryInR;
	AutoPagedVector<float32> carryOutL, carryOutR;
	std::vector<float32> tempL, tempR;


	float32 gain=1;
	bool shouldCheck=true;
	uint32 chunkCount = 0;
	float32 rmsL=0;
	float32 rmsR=0;
	bool overloadL = false;
	bool overloadR = false;


	static double rms(std::vector<float32> &,bool &);
	static uint32 read(const port_t,float32 *);
	static void write(const port_t,float32 *);

	static Mode modeIn(const port_t,const port_t);
	static Mode modeOut(const port_t,const port_t);

	bool checkModeChangeIn(const uint32 n);
	bool checkModeChangeIn();
	bool checkModeChangeOut(const uint32 n);
	bool checkModeChangeOut();




protected:
	virtual void processApplicationMessage(const TJBox_PropertyDiff &diff);
	virtual void process();
	virtual void reset();

public:
	SwitchNMix();
	virtual ~SwitchNMix() = default;
	SwitchNMix(const SwitchNMix &other) = default;
	SwitchNMix & operator=(const SwitchNMix &other) = default;
};

} /* namespace switchnmix */
} /* namespace meromorph */

#endif /* SWITCHNMIX_HPP_ */
