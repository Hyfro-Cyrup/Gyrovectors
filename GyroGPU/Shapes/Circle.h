#pragma once
#include "IGeometry.h"
#include "Math/MobiusGyrovector.h"
#include "Math/MobiusTransformation.h"

namespace collision {
	class Circle : public IGeometry {
		MobiusGyrovector _center;
		double _radius;

	public:
		Circle(MobiusGyrovector center, double radius) : _center{ center }, _radius{ radius } {}

		MobiusGyrovector& getCenter() { return _center; }
		double getRadius() const { return _radius; }

		inline void Transform(MobiusTransformation transform) override {
			_center = transform(_center);
		}

		// double dispatch
		inline bool Intersects(IGeometry* other) override {
			return other->Intersects(this);
		}

		// overridden intersect methods
		bool Intersects(Point* other) override;
		bool Intersects(Circle* other) override;
		bool Intersects(LineSegment* other) override;
		bool Intersects(Polygon* other) override;
	};
}


