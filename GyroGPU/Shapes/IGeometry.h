#pragma once
#include "Math/MobiusTransformation.h"


namespace collision {
	// forward-declare all subclasses
	class Point; class Circle; class LineSegment; class Polygon;

	struct IGeometry {
		virtual void Transform(MobiusTransformation transform) = 0;

		// double dispatch
		virtual bool Intersects(IGeometry* other) = 0;

		// default intersection checks to false (e.g. Point to Point intersection)
		virtual bool Intersects(Point* other) { return false; }
		virtual bool Intersects(Circle* other) { return false; }
		virtual bool Intersects(LineSegment* other) { return false; }
		virtual bool Intersects(Polygon* other) { return false; }
	};
}