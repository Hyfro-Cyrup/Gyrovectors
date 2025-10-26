#pragma once
#include "IVector.h"

template <typename V, typename S>
class IGyrovector : public IVector<V, S> {
public:
	virtual ~IGyrovector() = default;
	
	virtual V Gyr(const V& b, const V& c) const = 0;

	static V CoAddition(const V& a, const V& b) {
		return a + a.Gyr(-b, b);
	}

	static V CoSubtraction(const V& a, const V& b) {
		return CoAddition(a, -b);
	}

	V BoxPlus(V& other) {
		return CoAddition(*static_cast<V*>(this), other);
	}

	V BoxMinus(V& other) {
		return CoSubtraction(*static_cast<V*>(this), other);
	}
};