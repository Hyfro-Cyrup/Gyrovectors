#pragma once
#include <vector>

#include "IGeometry.h"
#include "Math/MobiusGyrovector.h"
#include "Math/MobiusTransformation.h"

namespace collision {
	class Circle;

	// Vertices must be convex and ordered counter-clockwise. 
	class Polygon : public IGeometry {
		std::vector<MobiusGyrovector> _vertices;

	public:
		Polygon(std::vector<MobiusGyrovector> verts) : _vertices{ verts } {}

		std::vector<MobiusGyrovector>& getVertices() { return _vertices; }

		inline void Transform(MobiusTransformation transform) override {
			for (int i = 0; i < _vertices.size(); i++) {
				_vertices[i] = transform(_vertices[i]);
			}
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

