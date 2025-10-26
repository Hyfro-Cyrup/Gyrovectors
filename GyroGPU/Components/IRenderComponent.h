#pragma once

#include "Component.h"
#include "Math/MobiusTransformation.h"

class RenderSystem;

struct IRenderComponent : public Component<IRenderComponent> {
	virtual ~IRenderComponent() = default;

	float z{ 0.0 };

	// double dispatch
	virtual void SubmitTo(RenderSystem& sys, MobiusTransformation& model) const = 0;

};

using RENDER = IRenderComponent;