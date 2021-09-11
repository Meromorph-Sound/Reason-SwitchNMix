/*
 * base.cpp
 *
 *  Created on: 13 Dec 2020
 *      Author: julianporter
 */



#include "base.hpp"
#include <cstring>

namespace meromorph {
void trace(const char *tmpl) {
	JBox_Trace(__FILE__,__LINE__,tmpl);
}
void trace(const char *tmpl,const float32 value) {
	TJBox_Value v = JBox_MakeNumber(value);
	JBox_TraceValues(__FILE__,__LINE__,tmpl,&v,1);
}
void trace(const char *tmpl,const float32 value1,const float32 value2) {
	TJBox_Value vals[2];
	vals[0] = JBox_MakeNumber(value1);
	vals[1] = JBox_MakeNumber(value2);
	JBox_TraceValues(__FILE__,__LINE__,tmpl,vals,2);
}

 float32 toFloat(const TJBox_Value diff) {
	return static_cast<float32>(JBox_GetNumber(diff));
}
 float32 clampedFloat(const TJBox_Value diff) {
	 auto f = static_cast<float32>(JBox_GetNumber(diff));
	 return std::max(0.f,std::min(1.f,f));
 }
 float32 scaledFloat(const TJBox_Value diff,const float32 min,const float32 max) {
	 auto f=clampedFloat(diff);
	 return min+(max-min)*f;
 }
 float32 scaledDownFloat(const TJBox_Value diff,const float32 min_,const float32 max_) {
 	 auto f=std::max(min_,std::min(toFloat(diff),max_));
 	 return (f-min_)/(max_-min_);
  }

 float32 intRangeToFloat(const TJBox_Value diff,const uint32 steps,const float32 min,const float32 max) {
	 auto m=(float32)(steps-1);
	 auto f = std::max(0.f,std::min(toFloat(diff),m))/m;
	 return min+(max-min)*f;
 }

 float32 phaseArgument(const TJBox_PropertyDiff &diff,float32 minFreq,float32 maxFreq,uint32 N) {
 	auto def = intRangeToFloat(diff.fCurrentValue,N,minFreq,maxFreq);
 	return cx::deg2rad(def);
 }


 bool toBool(const TJBox_Value diff) {
	 auto t=JBox_GetType(diff);
	 switch(t) {
	 case kJBox_Number:
		 return static_cast<float32>(JBox_GetNumber(diff))>0;
		 break;
	 case kJBox_Boolean:
		 return JBox_GetBoolean(diff)>0;
		 break;
	 default:
		 return false;
		 break;
	 }
}
 int32 toInt(const TJBox_Value diff) {
	return static_cast<int32>(static_cast<float32>(JBox_GetNumber(diff)));
}

 void append(char *str,const char *root,const char *ext) {
	strcpy(str,root);
	strcat(str,ext);
}

 cx32 r2c(const float32 r) { return cx32(r,0); }

 namespace cx {
 	 float32 deg2rad(const float32 deg) { return deg * Pi/180.f; }
 	 cx32 purePhase(const float32 theta) { return std::polar(1.f,theta*Pi/180.f); }
 }

}



