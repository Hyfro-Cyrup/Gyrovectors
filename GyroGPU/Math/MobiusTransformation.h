#pragma once
#include <complex>

#include "MobiusGyrovector.h"

/// <summary>
/// Gyroassociates for you, turning two gyrotranslations into one rotation and then gyrotranslation
/// </summary>
struct MobiusTransformation {
	MobiusGyrovector translation;
	Complex rotation;

	MobiusTransformation() : translation{ MobiusGyrovector::Zero }, rotation{ 1.0 } {}
	MobiusTransformation(MobiusGyrovector translation, Complex rotation) : translation{ translation }, rotation{ rotation } {}
	MobiusTransformation(MobiusGyrovector translation) : MobiusTransformation{ translation, 1.0 } {}

	inline static MobiusTransformation Identity() { return { MobiusGyrovector::Zero, 1.0 }; }

	MobiusGyrovector Transform(const MobiusGyrovector& z) const {
		return translation + (z * rotation);
	}

	MobiusGyrovector operator()(const MobiusGyrovector& z) const {
		return Transform(z);
	}

	// returns reference for method chaining
	MobiusTransformation& Translate(MobiusGyrovector ds) {
		// a + (t + uz) = (a + t) + gyr[a, t](uz) = (a + t) + (gyr[a, t]*u)z
		if (ds == -translation) {
			// left cancellation
			return *this;
		}
		rotation *= ds.Gyr(translation);
		translation = ds + translation;
		return *this;
	}

	// returns reference for method chaining
	MobiusTransformation& Rotate(double angle) {
		// for complex multiplication by a unit w, we have w(t + uz) = wt + wuz from the definition of gyroaddition
		Complex w = std::exp(Complex(0, 1) * angle);
		translation = translation * w;
		rotation *= w;
		return *this;
	}

	// Create a new Transformation based on this one plus a translation
	MobiusTransformation TranslateConst(MobiusGyrovector ds) const {
		MobiusTransformation copy = *this;
		copy.Translate(ds);
		return copy;
	}

	MobiusTransformation PrecomposeConst(MobiusTransformation transform) const {
		transform.translation = transform.translation * this->rotation;
		transform.rotation *= this->rotation;
		return transform.TranslateConst(this->translation);
	}


};