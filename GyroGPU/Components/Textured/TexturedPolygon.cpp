#include "TexturedPolygon.h"
#include "Systems/RenderSystem.h"
#include "Math/MobiusTransformation.h"

void TexturedPolygon::SubmitTo(RenderSystem& sys, MobiusTransformation& model) const
{
	sys.Add(*this, model);
}