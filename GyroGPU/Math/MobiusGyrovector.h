#pragma once
#include <algorithm>
#include <cmath>
#include <complex>
#include <string>

#include "IGyroVector.h"

using Complex = std::complex<double>;

class MobiusGyrovector : public IGyrovector< MobiusGyrovector, double> {
	friend class MobiusGyroline;
	// private fields
private:
	Complex _value; 

	// public fields
public:
	double x = _value.real();
	double y = _value.imag();

	// constructors, destructor
public:
	MobiusGyrovector() : _value{ 0.0 } {};
	MobiusGyrovector(Complex value) : _value{ value } {};
	MobiusGyrovector(double x, double y) : _value{ x, y } {};
	~MobiusGyrovector() noexcept = default;

	//public static fields
public:
	static const MobiusGyrovector Zero;

	//private static fields
private:
	static constexpr double S = 1.0;
	static constexpr double UNIT_LENGTH = 0.5493061443341; // std::atanh(0.5);

#pragma region public methods
public:
	std::string ToString() const {
		return "<" + std::to_string(x) + ", " + std::to_string(y) + ">";
	}
#pragma endregion

#pragma region public static methods
public:
	#pragma region Norms
	static double EInnerProduct(const MobiusGyrovector& a, const MobiusGyrovector& b) {
		return a.x * b.x + a.y * b.y;
	}

	static double ENormSquared(const MobiusGyrovector& a) {
		return EInnerProduct(a, a);
	}

	static double ENorm(const MobiusGyrovector& a) {
		return std::abs(a._value);
	}

	static double Norm(const MobiusGyrovector& a) {
		return std::atanh(ENorm(a) / S) / std::atanh(UNIT_LENGTH / S);
	}

	static double Distance(const MobiusGyrovector& a, const MobiusGyrovector& b) {
		return Norm(-a + b);
	}
	#pragma endregion

	#pragma region Angles
	// true if point b is clockwise of point a, false if b is counterclockwise of a (or they're collinear with the origin)
	static bool IsClockwise(const MobiusGyrovector& a, const MobiusGyrovector& b) {
		// find the k component of the cross product (a x b).
		// If it's positive, a->b is CCW around the origin. If it's negative, a->b is CW around the origin
		return a.x * b.y - a.y * b.x < 0;
	}

	static double Angle(const MobiusGyrovector& a, const MobiusGyrovector& b) {
		// get the abs value of the angle in [0, pi]
		// clamping to [-1, 1] avoids floating point errors
		double dot = EInnerProduct(a, b) / (ENorm(a) * ENorm(b));
		double angle = std::acos(std::min(std::max(dot, -1.0), 1.0));
		return IsClockwise(a, b) ? -angle : angle;
	}

	static double Angle(const MobiusGyrovector& a, const MobiusGyrovector& b, MobiusGyrovector& about) {
		return Angle(-about + a, -about + b);
	}

	static MobiusGyrovector Rotate(const MobiusGyrovector& a, double& angle) {
		if (angle == 0) { return a; }
		return MobiusGyrovector(a._value * std::exp(Complex(0, 1) * angle));
	}

	static MobiusGyrovector RotateAround(const MobiusGyrovector& a, const MobiusGyrovector& center, double& angle) {
		if (angle == 0) { return a; }
		return center + Rotate(-center + a, angle);
	}
	#pragma endregion

#pragma endregion

#pragma region IVector, IGyrovector Overrides
public:
	// Equality operator
	bool operator==(const MobiusGyrovector& other) const {
		return _value == other._value;
	}

	// Addition operator
	MobiusGyrovector operator+(const MobiusGyrovector& other) const {
		const MobiusGyrovector& u = *this;
		const MobiusGyrovector& v = other;
		if constexpr(S == 1.0)
		{
			// In the normal S=1 Poincare Disk we can use a simpler formula. Tested & proven equal to the below
			return MobiusGyrovector((u._value + v._value) / (1.0 + std::conj(u._value) * v._value));
		}
		// This is the general formula that works in the S-ball of any inner product space. 
		double uv = EInnerProduct(u, v);
		double u2 = ENormSquared(u);
		double v2 = ENormSquared(v);
		const double S2 = S * S;
		const double S4 = S2 * S2;
		return MobiusGyrovector(
			((1 + 2.0 / S2 * uv + 1.0 / S2 * v2) * u._value + (1 - 1.0 / S2 * u2) * v._value) /
			(1 + 2.0 / S2 * uv + 1.0 / S4 * u2 * v2));
	}

	// Subtraction operator
	MobiusGyrovector operator-(const MobiusGyrovector& other) const {
		return *this + (-other);
	}

	// Unary negation operator
	MobiusGyrovector operator-() const {
		return MobiusGyrovector{ -_value };
	}

	// Multiplication by scalar
	MobiusGyrovector operator*(const double& scalar) const {
		if (*this == Zero) {
			return Zero;
		}
		double mag = ENorm(*this);
		return MobiusGyrovector(S * std::tanh(scalar * std::atanh(mag / S)) * _value / mag);
	}

	// Division by scalar
	MobiusGyrovector operator/(const double& scalar) const {
		return *this * (1.0 / scalar);
	}

	MobiusGyrovector Gyr(const MobiusGyrovector& b, const MobiusGyrovector& c) const override {
		return MobiusGyrovector{ Gyr(b) * c._value };
	}

	Complex Gyr(const MobiusGyrovector& b) const {
		const MobiusGyrovector& a = *this;
		return (a + b)._value / (b + a)._value;
	}

	// Multiplication by complex
	MobiusGyrovector operator*(const Complex& z) const {
		if (*this == Zero) {
			return Zero;
		}
		return MobiusGyrovector{ z * _value };
	}



#pragma endregion

};

