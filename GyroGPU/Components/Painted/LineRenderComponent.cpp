#include "LineRenderComponent.h"

#include "Math/MobiusGyroline.h"
#include "Math/MobiusTransformation.h"
#include "Systems/RenderSystem.h"

void LineRenderComponent::SubmitTo(RenderSystem& sys, MobiusTransformation& model) const {
	sys.Add(*this, model);
}
