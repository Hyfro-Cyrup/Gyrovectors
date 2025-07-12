using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Linq.Expressions;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;
using System.Transactions;
using System.Windows.Media.Imaging;
using System.Windows.Media.Media3D;
using System.Windows.Shapes;

namespace Gyrovectors.Models;

public readonly struct MöbiusGyrovector : IGyroVector<MöbiusGyrovector, double>
{
    public const double S = 1; // Gyrovectors live in the S-ball of the complex plane
    public static readonly double UNIT_LENGTH = Math.Tanh(1.0 / 2.0); // Used in the Norm formula; asserts that MöbiusGyrovector(UNIT_LENGTH, 0) has norm 1. Would be const if Math.Tanh was a constexpr

    readonly Complex _value;

    public MöbiusGyrovector(Complex value)
    {
        //if (value.Magnitude >= 1) throw new Exception($"Val was {value}, mag {value.Magnitude}");
        _value = value;
    }

    public MöbiusGyrovector(double x, double y) : this(new Complex(x, y)) { }

    public static readonly MöbiusGyrovector Zero = new(0);
    public static MöbiusGyrovector AdditiveIdentity => Zero;

    public double x => _value.Real;
    public double y => _value.Imaginary;

    public static MöbiusGyrovector Gyr(MöbiusGyrovector a, MöbiusGyrovector b, MöbiusGyrovector c)
        => new MöbiusGyrovector((a + b)._value / (b + a)._value * c._value);

    // This is very sadly not related to any norm in hyperbolic geometry
    public static double EInnerProduct(MöbiusGyrovector a, MöbiusGyrovector b)
        => a.x * b.x + a.y * b.y;
    public static double ENormSquared(MöbiusGyrovector a)
        => EInnerProduct(a, a);

    public static double ENorm(MöbiusGyrovector a)
        => a._value.Magnitude;

    public static double Norm(MöbiusGyrovector a)
        => Math.Atanh(a._value.Magnitude / S) / Math.Atanh(UNIT_LENGTH / S);

    public static double Distance(MöbiusGyrovector a, MöbiusGyrovector b)
        => Norm(-a + b);

    // true if point b is clockwise of point a, false if b is counterclockwise of a (or they're collinear with the origin)
    public static bool isClockwise(MöbiusGyrovector a, MöbiusGyrovector b)
    {
        // find the k component of the cross product (a x b).
        // If it's positive, a->b is CCW around the origin. If it's negative, a->b is CW around the origin
        return a.x * b.y - a.y * b.x < 0;
    }
    // returns the signed angle from `a` counterclockwise to `b` (about the origin) in the range (-pi, pi]
    public static double Angle(MöbiusGyrovector a, MöbiusGyrovector b)
    {
        // get the abs value of the angle in [0, pi]
        // Clamp(x, -1, 1) avoids floating point errors
        double angle = Math.Acos(Math.Clamp(EInnerProduct(a, b) / (ENorm(a) * ENorm(b)), -1, 1));
        return isClockwise(a, b) ? -angle : angle;
    }

    // returns the signed angle from `a` counterclockwise to `b` (about `about`)
    public static double Angle(MöbiusGyrovector a, MöbiusGyrovector b, MöbiusGyrovector about)
        => Angle(-about + a, -about + b);

    public static MöbiusGyrovector NearestPointOnLine(MöbiusGyrovector point, MöbiusGyroline line)
    {
        if (S != 1) { throw new NotImplementedException("Distance from point to line relies on assumptions about standard Poincare Disk"); }

        // Get the intersection between segment point--center and the circle (center, radius)
        Complex delta = point._value - line.center;
        return new MöbiusGyrovector(line.center + line.radius * delta / delta.Magnitude);

    }

    // angle in radians
    public static MöbiusGyrovector Rotate(MöbiusGyrovector a, double angle)
    {
        if (angle == 0) { return a; }
        return new MöbiusGyrovector(a._value * Complex.Exp(Complex.ImaginaryOne * angle));
    }

    public static MöbiusGyrovector RotateAround(MöbiusGyrovector a, MöbiusGyrovector center, double angle)
        => center + Rotate(-center + a, angle);

    #region Equality

    public bool Equals(MöbiusGyrovector? other)
    {
        if (!other.HasValue) return false;
        return _value.Equals(other.Value);
    }

    public bool Equals(MöbiusGyrovector other) => _value.Equals(other._value);

    public override bool Equals(object obj)
    {
        return obj is MöbiusGyrovector && Equals((MöbiusGyrovector)obj);
    }

    public static bool operator ==(MöbiusGyrovector? left, MöbiusGyrovector? right)
    {
        if (left.HasValue && right.HasValue) return left.Value._value == right.Value._value;
        return false;
    }

    public static bool operator ==(MöbiusGyrovector left, MöbiusGyrovector right)
        => left._value == right._value;

    public static bool operator !=(MöbiusGyrovector? left, MöbiusGyrovector? right)
    {
        if (left.HasValue && right.HasValue) return left.Value._value != right.Value._value;
        return false;
    }

    public static bool operator !=(MöbiusGyrovector left, MöbiusGyrovector right)
        => left._value != right._value;

    #endregion

    #region Operators

    public static MöbiusGyrovector operator +(MöbiusGyrovector u, MöbiusGyrovector v)
    {
        if (S == 1)
        {
            // In the normal S=1 Poincare Disk we can use a simpler formula. Tested & proven equal to the below
            return new MöbiusGyrovector((u._value + v._value) / (1 + Complex.Conjugate(u._value) * v._value));
        }
        // This is the general formula that works in the S-ball of any inner product space. 
        double uv = EInnerProduct(u, v);
        double u2 = ENormSquared(u);
        double v2 = ENormSquared(v);
        const double S2 = S * S;
        const double S4 = S2 * S2;
        return new MöbiusGyrovector(
            ((1 + 2.0 / S2 * uv + 1.0 / S2 * v2) * u._value + (1 - 1.0 / S2 * u2) * v._value) /
            (1 + 2.0 / S2 * uv + 1.0 / S4 * u2 * v2));
    }

    public static MöbiusGyrovector operator -(MöbiusGyrovector value) 
        => new MöbiusGyrovector(-value._value);

    public static MöbiusGyrovector operator -(MöbiusGyrovector left, MöbiusGyrovector right)
        => left + -right;

    public static MöbiusGyrovector CoAddition(MöbiusGyrovector a, MöbiusGyrovector b)
        => a + Gyr(a, -b, b);

    public static MöbiusGyrovector CoSubtraction(MöbiusGyrovector a, MöbiusGyrovector b)
        => CoAddition(a, -b);

    public MöbiusGyrovector BoxPlus(MöbiusGyrovector other)
        => CoAddition(this, other);

    public MöbiusGyrovector BoxMinus(MöbiusGyrovector other)
        => CoSubtraction(this, other);

    public static MöbiusGyrovector operator *(MöbiusGyrovector vector, double scalar)
    {
        double mag = vector._value.Magnitude;
        if (mag == 0) { return Zero; }
        return new MöbiusGyrovector(S * Math.Tanh(scalar * Math.Atanh(mag / S)) * vector._value / mag);
    }

    public static MöbiusGyrovector operator *(double scalar, MöbiusGyrovector vector)
        => vector * scalar;

    public static MöbiusGyrovector operator /(MöbiusGyrovector left, double right)
        => new MöbiusGyrovector(left._value * (1 / right));
    #endregion

    public override string ToString()
    {
        return _value.ToString();
    }
}
