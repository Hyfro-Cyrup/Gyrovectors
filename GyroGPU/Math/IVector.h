#pragma once

template <typename V, typename S>
class IVector {
public:
    virtual ~IVector() = default;

    // Equality operator
    virtual bool operator==(const V& other) const = 0;

    // Addition operator
    virtual V operator+(const V& other) const = 0;

    // Subtraction operator
    virtual V operator-(const V& other) const = 0;

    // Unary negation operator
    virtual V operator-() const = 0;

    // Multiplication by scalar
    virtual V operator*(const S& scalar) const = 0;

    // Division by scalar
    virtual V operator/(const S& scalar) const = 0;
};