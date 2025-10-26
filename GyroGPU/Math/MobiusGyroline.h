#pragma once
#include <complex>
#include <stdexcept>

#include "MobiusGyrovector.h"

using Complex = std::complex<double>;

class MobiusGyroline {
public:
	MobiusGyrovector a, b;
	Complex center;
	double radius;

    MobiusGyroline(MobiusGyrovector a, MobiusGyrovector b) :
        a{ a }, b{ b },
        center{ CalculateCenter(a, b) },
        radius{ std::abs(Complex(center.real() - a.x, center.imag() - a.y)) } 
    {}

    static MobiusGyrovector NearestPointOnLine(MobiusGyrovector point, MobiusGyroline line) {
        if constexpr (MobiusGyrovector::S != 1.0) {
            throw std::logic_error("NearestPointOnLine not implemented for S!=1. Distance from point to line relies on assumptions about standard Poincare Disk");
        }
        // Get the intersection between segment point--center and the circle (center, radius)
        Complex delta = point._value - line.center;
        return MobiusGyrovector(line.center + line.radius * delta / std::abs(delta));
    }

private:
    // TODO: Honestly what do we do about the edge cases where a and b are collinear with 0? Just pick a big number to make the center and radius?
    // That edge case happens when we divide by something that we haven't checked to be nonzero, e.g. `a.x`.
	Complex CalculateCenter(MobiusGyrovector& a, MobiusGyrovector& b) {
        double center_x, center_y;
        if (a.x == b.x) {
            if (a.y == b.y)
            {
                throw std::invalid_argument("Gyrovectors a=" + a.ToString() + " and b=" + b.ToString() + " cannot be the same.");
            }
            // Since the x values are equal, the perpinducular bisector restraint is simpler
            center_y = (a.y + b.y) / 2;

            // Substitute that value into the condition that 1, |(x,y)-a|, and |(x,y)| form a right triangle
            // (1 is the radius of the Poincare Disk, where gyrolines meet perpinducularly)
            center_x = (MobiusGyrovector::ENormSquared(a) + 1 - 2 * a.y * center_y) / (2 * a.x);
        }
        else if (a.y == b.y) 
        {
            // Since the y values are equal, the perpinducular bisector restraint is simpler
            center_x = (a.x + b.x) / 2;
            
            // Substitute that value into the condition that 1, |(x,y)-a|, and |(x,y)| form a right triangle
            // (1 is the radius of the Poincare Disk, where gyrolines meet perpinducularly)
            center_y = (MobiusGyrovector::ENormSquared(a) + 1 - 2 * a.x * center_x) / (2 * a.y);
        } 
        else 
        {
            // Define A and B such that x = A + By implies (x,y) is on the perpindicular bisector of a and b
            double A = (MobiusGyrovector::ENormSquared(a) - MobiusGyrovector::ENormSquared(b)) / (2 * (a.x - b.x));
            double B = (b.y - a.y) / (a.x - b.x);

            // Substitute x = A + By into the condition that 1, |(x,y)-a|, and |(x,y)| form a right triangle.
            // (1 is the radius of the Poincare Disk, where gyrolines meet perpinducularly)
            center_y = (MobiusGyrovector::ENormSquared(a) - 2 * A * a.x + 1) / (2 * (B * a.x + a.y));
            center_x = A + B * center_y;
        }
        return Complex{ center_x, center_y };
	}
};