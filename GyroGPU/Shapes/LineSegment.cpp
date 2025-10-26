#include "LineSegment.h"

#include <vector>

#include "Circle.h"
#include "Polygon.h"
#include "Math/MobiusGyrovector.h"
#include "Math/MobiusGyrometry.h"

namespace collision {
	bool LineSegment::Intersects(Circle* other)
	{
		// Function defined in Circle
		return other->Intersects(this);
	}

	bool LineSegment::Intersects(LineSegment* other)
	{
		return DoLineSegmentsIntersect(this->_line, other->_line);
	}

	bool LineSegment::Intersects(Polygon* other)
	{
		std::vector<MobiusGyrovector> vertices = other->getVertices();
		if (IsPointInsidePolygon(this->_line.a, vertices) || IsPointInsidePolygon(this->_line.b, vertices)) {
			return true;
		}
		vertices.push_back(vertices[0]);
		for (int i = 0; i < vertices.size() - 1; i++) {
			if (DoLineSegmentsIntersect(this->_line, { vertices[0], vertices[1] })) {
				return true;
			}
		}
		return false;
	}
}
