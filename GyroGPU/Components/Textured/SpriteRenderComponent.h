#pragma once
#include <memory>

#include "TexturedPolygon.h"
#include "Math/MobiusGyrovector.h"
#include "Rendering/Texture/Texture.h"

class RenderSystem;

/// <summary>
/// Render component for "rectangular" sprites with texture.
/// </summary>
struct SpriteRenderComponent : public TexturedPolygon {

	//constructors
	SpriteRenderComponent(MobiusGyrovector pos, double width, double height, std::shared_ptr<Texture> texture, float z = 0.0) :
		TexturedPolygon{
			{ // vertices
				pos + MobiusGyrovector(-width, -height),
				pos + MobiusGyrovector(width, -height),
				pos + MobiusGyrovector(width,  height),
				pos + MobiusGyrovector(-width,  height),
			},
			{ // uvs
				{0.0, 0.0},
				{1.0, 0.0},
				{1.0, 1.0},
				{0.0, 1.0}
			}, texture, z
		} {	}

	SpriteRenderComponent(double half_width, double half_height, std::shared_ptr<Texture> texture, float z = 0.0) :
		SpriteRenderComponent(MobiusGyrovector::Zero, half_width, half_height, texture, z) {}
};