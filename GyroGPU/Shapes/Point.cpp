#include "Point.h"
#include "Circle.h"
#include "Polygon.h"

namespace collision {
    bool Point::Intersects(Circle* other)
    {
        // Function defined in Circle
        return other->Intersects(this);
    }

    bool Point::Intersects(Polygon* other)
    {
        // Function defined in Polygon
        return other->Intersects(this);
    }
}
