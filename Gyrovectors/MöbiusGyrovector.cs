using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace Gyrovectors;

public readonly struct MöbiusGyrovector : IGyroVector<MöbiusGyrovector, double>
{
    readonly Complex _value;

    public MöbiusGyrovector(Complex value)
    {
        //if (value.Magnitude >= 1) throw new Exception($"Val was {value}, mag {value.Magnitude}");
        _value = value;
    }

    public MöbiusGyrovector(double x, double y) : this(new Complex(x, y)) { }

    public static MöbiusGyrovector Zero = new(0);
    public static MöbiusGyrovector AdditiveIdentity => Zero;

    public double x => _value.Real;
    public double y => _value.Imaginary;

    public static MöbiusGyrovector Gyr(MöbiusGyrovector a, MöbiusGyrovector b, MöbiusGyrovector c)
        => new MöbiusGyrovector(((a + b)._value / (b + a)._value) * c._value);

    public static double InnerProduct(MöbiusGyrovector a, MöbiusGyrovector b)
        => a.x * b.x + a.y * b.y;

    public static double NormSquared(MöbiusGyrovector a)
        => InnerProduct(a, a);

    public static double Norm(MöbiusGyrovector a)
        => Math.Sqrt(NormSquared(a));

    public static double Distance(MöbiusGyrovector a, MöbiusGyrovector b)
        => Norm(-a + b);

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

    public static MöbiusGyrovector operator +(MöbiusGyrovector left, MöbiusGyrovector right) 
        => new MöbiusGyrovector((left._value + right._value) / (1 + Complex.Conjugate(left._value) * right._value));


    public static MöbiusGyrovector operator -(MöbiusGyrovector value) 
        => new MöbiusGyrovector(-value._value);

    public static MöbiusGyrovector operator -(MöbiusGyrovector left, MöbiusGyrovector right)
        => left + (-right);

    public static MöbiusGyrovector operator *(MöbiusGyrovector vector, double scalar)
    {
        double mag = vector._value.Magnitude;
        if (mag == 0) { return Zero; }

        Complex a = Math.Pow(1 + mag, scalar);
        Complex b = Math.Pow(1 - mag, scalar);
        return new MöbiusGyrovector((a - b) / (a + b) * vector._value / mag);

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
