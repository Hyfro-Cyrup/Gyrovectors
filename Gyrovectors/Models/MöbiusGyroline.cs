using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace Gyrovectors.Models;

public readonly struct MöbiusGyroline
{
    public readonly MöbiusGyrovector a, b;
    public readonly Complex center;
    public readonly double radius;

    public MöbiusGyroline(MöbiusGyrovector a, MöbiusGyrovector b) {
        this.a = a;
        this.b = b;
        double center_x, center_y;
        if (a.x != b.x)
        {
            // Define A and B such that x = A + By implies (x,y) is on the perpindicular bisector of a and b
            double A = (MöbiusGyrovector.ENormSquared(a) - MöbiusGyrovector.ENormSquared(b)) / (2 * (a.x - b.x));
            double B = (b.y - a.y) / (a.x - b.x);

            // Substitute x = A + By into the condition that 1, |(x,y)-a|, and |(x,y)| form a right triangle.
            // (1 is the radius of the Poincare Disk, where gyrolines meet perpinducularly)
            center_y = (MöbiusGyrovector.ENormSquared(a) - 2 * A * a.x + 1) / (2 * (B * a.x + a.y));
            center_x = A + B * center_y;
        } 
        else
        {
            if (a.y == b.y)
            {
                throw new ArgumentException($"Gyrovectors a={a} and b={b} cannot be the same.");
            }
            // Since the x values are equal, the perpinducular bisector restraint is simpler
            center_y = (a.y + b.y) / 2;

            // Substitute that value into the condition that 1, |(x,y)-a|, and |(x,y)| form a right triangle
            // (1 is the radius of the Poincare Disk, where gyrolines meet perpinducularly)
            center_x = (MöbiusGyrovector.ENormSquared(a) + 1 - 2 * a.y * center_y) / (2 * a.x);
            
        }

            center = new Complex(center_x, center_y);
        radius = new Complex(center_x - a.x, center_y - a.y).Magnitude;
    }

}
