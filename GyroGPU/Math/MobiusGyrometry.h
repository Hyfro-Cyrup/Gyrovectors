#pragma once
#include <cmath>
#include <complex>
#include <cstdlib>
#include <vector>

#include "MobiusGyrovector.h"
#include "MobiusGyroline.h"

/// <summary>
/// Finds the nearest point on a line to a given point. Assumes that you get the same answer in the gyrometric as the euclidean metric. 
/// </summary>
/// <param name="z">The point off the line.</param>
/// <param name="line">The line to project onto.</param>
/// <returns>The gyrovector on the line closes to `z`</returns>
inline MobiusGyrovector NearestPointOnLine(MobiusGyrovector z, MobiusGyroline line) {
    Complex point{ z.x, z.y };
    if (isinf(line.radius) || isnan(line.radius) || abs(line.radius) > 100000) {
        // just project onto the euclidean line abs
        Complex a{ line.a.x, line.a.y };
        Complex b{ line.b.x, line.b.y };
        Complex diff = b - a;
        return a + (MobiusGyrovector::EInnerProduct(point - a, diff) / MobiusGyrovector::ENormSquared(diff)) * (diff);
    }
    Complex delta = point - line.center;
    return MobiusGyrovector{ line.center + line.radius * delta / abs(delta) };
}

inline MobiusGyrovector NearestPointOnLineSegment(MobiusGyrovector z, MobiusGyroline line) {
    MobiusGyrovector projection = NearestPointOnLine(z, line);
    MobiusGyrovector midpoint = line.a + (-line.a + line.b) * 0.5;
    if (2 * MobiusGyrovector::Distance(projection, midpoint) <= MobiusGyrovector::Distance(line.a, line.b)) {
        // projection is as close to the midpoint of AB as either A or B; projection is on the line segment. 
        return projection;
    }
    // projection is outside the segment. get the closest endpoint.
    return MobiusGyrovector::Distance(projection, line.a) <= MobiusGyrovector::Distance(projection, line.b) ? line.a : line.b;
}

/// <summary>
/// Determine if point is inside convex polygon
/// </summary>
/// <param name="point">any gyrovector</param>
/// <param name="polygon">a list of gyrovectors ordered CCW. Must define a convex region</param>
/// <returns>True if point is fully inside. False if outside or on edge (assumed narrow enough edge case to ignore)</returns>
inline bool IsPointInsidePolygon(MobiusGyrovector point, std::vector<MobiusGyrovector> polygon) {
    // vertices must be ordered CCW relative to every interior point. If any are CW, return false
    polygon.push_back(polygon[0]);
    for (int i = 0; i < polygon.size() - 1; i++) {
        MobiusGyrovector a = -point + polygon[i];
        MobiusGyrovector b = -point + polygon[i + 1];
        if (MobiusGyrovector::IsClockwise(a, b)) { return false; }
    }
    return true;
}

inline bool DoLineSegmentsIntersect(MobiusGyroline AB, MobiusGyroline CD) {
    // AB crosses CD iff A,B lie on opposite sides of CD and C, D lie on opposite sides of AB
    // ignore collinear edge case until necessary
    MobiusGyrovector a = AB.a, b = AB.b, c = CD.a, d = CD.b;
    return (MobiusGyrovector::IsClockwise(-a + c, -a + d) != MobiusGyrovector::IsClockwise(-b + c, -b + d))
        && (MobiusGyrovector::IsClockwise(-c + a, -c + b) != MobiusGyrovector::IsClockwise(-d + a, -d + b));
}