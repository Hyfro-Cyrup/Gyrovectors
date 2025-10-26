#include "Circle.h"

#include "Point.h"
#include "LineSegment.h"
#include "Polygon.h"

#include "Math/MobiusGyrovector.h"
#include "Math/MobiusGyrometry.h"


namespace collision {
	bool Circle::Intersects(Point* other)
	{
		return MobiusGyrovector::Distance(this->_center, other->getVector()) <= this->_radius;
	}

	bool Circle::Intersects(Circle* other)
	{
		return MobiusGyrovector::Distance(this->_center, other->_center) <= this->_radius + other->_radius;
	}

	bool Circle::Intersects(LineSegment* other)
	{
		MobiusGyrovector projection = NearestPointOnLineSegment(this->_center, other->getLine());
		return MobiusGyrovector::Distance(projection, this->_center) <= this->_radius;
	}

	bool Circle::Intersects(Polygon* other)
	{
		std::vector<MobiusGyrovector> vertices = other->getVertices();
		if (IsPointInsidePolygon(this->_center, vertices)) {
			return true;
		}
		// point is outside the polygon, so one of the edges of the polygon needs to intersect the boundary of the circle
		vertices.push_back(vertices[0]);
		for (int i = 0; i < vertices.size() - 1; i++) {
			MobiusGyrovector projection = NearestPointOnLineSegment(this->_center, MobiusGyroline{vertices[i], vertices[i + 1]});
			if (MobiusGyrovector::Distance(projection, this->_center) <= this->_radius) {
				return true;
			}
		}
		return false;
	}
}