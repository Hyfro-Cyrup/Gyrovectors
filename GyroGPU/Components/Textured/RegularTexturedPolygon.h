#pragma once
#include <memory>
#include <vector>

#include <glm/ext/vector_float2.hpp>

#include "TexturedPolygon.h"
#include "Math/MobiusGyrovector.h"
#include "Rendering/Texture/Texture.h"

class RenderSystem;

/// <summary>
/// Render component for regular polygon sprites with texture.
/// </summary>
struct RegularTexturedPolygon : public TexturedPolygon {

	// constructors
	RegularTexturedPolygon(std::vector<MobiusGyrovector> vertices, std::shared_ptr<Texture> texture, float z = 0.0) :
		TexturedPolygon{ vertices, GetUVs((int)vertices.size()), texture, z } { }

private:
	static std::vector<glm::vec2> GetUVs(int num_sides);

};