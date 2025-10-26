#pragma once

#include "../IRenderComponent.h"
#include "Math/MobiusGyroline.h"
#include "Math/MobiusTransformation.h"

class RenderSystem;

struct LineRenderComponent : public IRenderComponent {
	MobiusGyroline line;
	double weight;
	bool isSegment;
	bool isGyroWeight;

	// constructors
	LineRenderComponent(MobiusGyroline line, double weight, bool isSegment, bool isGyroWeight, float z) :
		line{ line }, weight{ weight }, isSegment{ isSegment }, isGyroWeight{ isGyroWeight } {
		this->z = z;
	}
	LineRenderComponent(MobiusGyroline line, double weight) : 
		LineRenderComponent(line, weight, false, true, 1.0) {}

	// double dispatch
	void SubmitTo(RenderSystem& sys, MobiusTransformation& model) const override;
};