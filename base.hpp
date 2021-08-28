/*
 * base.hpp
 *
 *  Created on: 2 Dec 2020
 *      Author: julianporter
 */

#ifndef DIRTYLFO_BASE_HPP_
#define DIRTYLFO_BASE_HPP_

#include "Jukebox.h"
#include <type_traits>
#include <cmath>
#include <algorithm>
#include <vector>
#include <complex>


namespace meromorph {

using uint8 = TJBox_UInt8;
using uint16 = TJBox_UInt16;
using int32 = TJBox_Int32;
using uint32 = TJBox_UInt32;
using int64 = TJBox_Int64;
using uint64 = TJBox_UInt64;
using float64 = TJBox_Float64;
using float32 = TJBox_AudioSample;

using cx32 = std::complex<float32>;

using Tag = uint32;
using Channel = uint32;

enum State : int32 {
	Off = 0, On = 1, Bypassed = 2
};

namespace cx {
 	const inline cx32 Zero = cx32();
 	const inline cx32 I = cx32(0,1);
 	const inline cx32 One = cx32(1);

 	float32 deg2rad(const float32 deg);
 	cx32 purePhase(const float32 theta);
 }


void trace(const char *tmpl);
void trace(const char *tmpl,const float32 value);
void trace(const char *tmpl,const float32 value1,const float32 value2);


 float32 toFloat(const TJBox_Value diff);
 float32 clampedFloat(const TJBox_Value diff);
 float32 scaledFloat(const TJBox_Value diff,const float32 min=0.0,const float32 max=1.0);
 float32 scaledDownFloat(const TJBox_Value diff,const float32 min=0.0,const float32 max=1.0);
 float32 phaseArgument(const TJBox_PropertyDiff &diff,float32 minFreq,float32 maxFreq,uint32 N);
 bool toBool(const TJBox_Value diff);
 int32 toInt(const TJBox_Value diff);

 float32 intRangeToFloat(const TJBox_Value diff,const uint32 steps,const float32 min=0.0,const float32 max=1.0);

 template<typename T>
T clamp(const T lo,const T hi,const T value) {
	return std::min<T>(hi,std::max<T>(lo,value));
}

 void append(char *str,const char *root,const char *ext);

 inline float32 const Pi = 3.141592653589793f;
 inline float32 const TwoPi = 2.f*Pi;





enum TriggerMode : uint32 {
	MANUAL = 0,
	EXT_CLOCK = 1,
	INT_CLOCK = 2
};

}




#endif /* DIRTYLFO_BASE_HPP_ */
