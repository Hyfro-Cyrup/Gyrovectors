#include "Polygon.h"
#include <vector>

#include "Point.h"
#include "Circle.h"
#include "LineSegment.h"

#include "Math/MobiusGyrometry.h"
#include "Math/MobiusGyrovector.h"

namespace collision {
	bool Polygon::Intersects(Point* other)
	{
		return IsPointInsidePolygon(other->getVector(), this->_vertices);
	}

	bool Polygon::Intersects(Circle* other)
	{
		// Function defined in Circle
		return other->Intersects(this);
	}

	bool Polygon::Intersects(LineSegment* other)
	{
		// Function defined in LineSegment
		return other->Intersects(this);
	}

	bool Polygon::Intersects(Polygon* other)
	{
		std::vector<MobiusGyrovector> vertices = this->_vertices;
		vertices.push_back(vertices[0]);
		for (int i = 0; i < vertices.size() - 1; i++) {
			LineSegment line{ vertices[i], vertices[i + 1] };
			if (line.Intersects(other)) {
				return true;
			}
		}
		return false;
	}
}
