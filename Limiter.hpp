/*
 * Limiter.hpp
 *
 *  Created on: 20 Feb 2021
 *      Author: julianporter
 */

#ifndef LIMITER_HPP_
#define LIMITER_HPP_

#import "base.hpp"

namespace meromorph {

class Limiter {
public:
	enum Mode : uint32 {
		HARD=1,
		SOFT=2
	};

private:

	float32 scale;
	Mode mode;
	bool active;
	bool didLimit;

	static constexpr float32 SOFT_THRESHOLD=0.15;
	static constexpr float32 epsilon=1.0e-5;
public:

	Limiter() : scale(1.0), mode(Mode::HARD), active(true), didLimit(false) {};
	virtual ~Limiter() = default;
	Limiter(const Limiter &other) = default;
	Limiter& operator=(const Limiter &other) = default;

	void setLimit(const float32 s) { scale=s; }
	void setMode(const Mode m) { mode=m; }
	void setActive(const bool a) { active=a; }

	template<typename T>
	bool limit(T *data,const uint32 n) {
		didLimit=false;
		if(!active) return false;

		switch(mode) {
		case Mode::SOFT:
			for(auto i=0;i<n;i++) {
				auto in = data[i];
				auto d=scale*tanh(in/scale);
				data[i]=d;
				didLimit = didLimit || (abs(d-in)>SOFT_THRESHOLD);
			}
			break;
		case Mode::HARD:
			for(auto i=0;i<n;i++) {
				auto n = std::max(scale,std::abs(data[i]));
				data[i]=data[i]/n;
				didLimit = didLimit || (n>1);
			}
			break;
		}
		return didLimit;
	}
	template<typename T>
	bool limit(std::vector<T> &v) { return limit(v.data(),v.size()); }
};

} /* namespace meromorph */

#endif /* LIMITER_HPP_ */
