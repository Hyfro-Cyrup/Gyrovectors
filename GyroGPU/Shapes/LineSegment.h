#pragma once

#include "IGeometry.h"
#include "Math/MobiusGyroline.h"
#include "Math/MobiusGyrovector.h"
#include "Math/MobiusTransformation.h"

namespace collision {
	class LineSegment : public IGeometry {
		MobiusGyroline _line;

	public:
		MobiusGyroline& getLine() { return _line; }

		LineSegment(MobiusGyroline line) : _line{ line } {}
		LineSegment(MobiusGyrovector a, MobiusGyrovector b) : _line{ a, b } {}

		inline void Transform(MobiusTransformation transform) override {
			_line = MobiusGyroline(transform(_line.a), transform(_line.b));
		}

		// double dispatch
		inline bool Intersects(IGeometry* other) override {
			return other->Intersects(this);
		}

		// overridden intersect methods
		bool Intersects(Circle* other) override;
		bool Intersects(LineSegment* other) override;
		bool Intersects(Polygon* other) override;
	};
}