using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace Gyrovectors;

public class MöbiusGyroline
{
    public readonly MöbiusGyrovector a, b;
    public readonly Complex center;
    public readonly double radius;

    public MöbiusGyroline(MöbiusGyrovector a, MöbiusGyrovector b) {
        this.a = a;
        this.b = b;
        double A = (MöbiusGyrovector.ENormSquared(a) - MöbiusGyrovector.ENormSquared(b)) / (2 * (a.x - b.x));
        double B = (b.y - a.y) / (a.x - b.x);
        double center_y = (MöbiusGyrovector.ENormSquared(a) - 2 * A * a.x + 1) / (2 * (B * a.x + a.y));
        double center_x = A + B * center_y;

        center = new Complex(center_x, center_y);
        radius = (new Complex(center_x - a.x, center_y - a.y)).Magnitude;
    }

}
