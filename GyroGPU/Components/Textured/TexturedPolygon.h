#pragma once
#include <memory>
#include <vector>

#include <glm/ext/vector_float2.hpp>

#include "../IRenderComponent.h"
#include "Math/MobiusGyrovector.h"
#include "Math/MobiusTransformation.h"
#include "Rendering/Texture/Texture.h"

class RenderSystem;

/// <summary>
/// Render component for regular polygon sprites with texture.
/// </summary>
struct TexturedPolygon : public IRenderComponent {
	std::vector<MobiusGyrovector> vertices;
	std::vector<glm::vec2> uvs;
	std::shared_ptr<Texture> texture;

	//constructors
	TexturedPolygon(std::vector<MobiusGyrovector> vertices, std::vector<glm::vec2> uvs, std::shared_ptr<Texture> texture, float z = 0.0) :
		vertices{ vertices }, uvs{ uvs },
		texture{ texture } {
		this->z = z;
	}

	// double dispatch
	void SubmitTo(RenderSystem& sys, MobiusTransformation& model) const override;
};