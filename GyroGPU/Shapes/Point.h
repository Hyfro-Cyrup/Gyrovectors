#pragma once
#include "IGeometry.h"
#include "Math/MobiusGyrovector.h"
#include "Math/MobiusTransformation.h"

namespace collision {
	class Point : public IGeometry {
		MobiusGyrovector _point;

	public:
		MobiusGyrovector& getVector() { return _point; }

		inline void Transform(MobiusTransformation transform) override {
			_point = transform(_point);
		}

		// double dispatch
		inline bool Intersects(IGeometry* other) override { 
			return other->Intersects(this); 
		}

		// overridden intersect methods
		bool Intersects(Circle* other) override;
		bool Intersects(Polygon* other) override;
	};
}

