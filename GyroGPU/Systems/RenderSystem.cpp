#include "RenderSystem.h"

#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <vector>

#include "Components/ComponentType.h"
#include "Components/IRenderComponent.h"
#include "Components/Painted/LineRenderComponent.h"
#include "Components/PositionComponent.h"
#include "Components/Textured/TexturedPolygon.h"
#include "GameObject.h"
#include "Math/MobiusGyroline.h"
#include "Math/MobiusGyrovector.h"
#include "Math/MobiusTransformation.h"
#include "Rendering/RenderPrimitive.h"
#include <iostream>

void RenderSystem::RenderScene(std::vector<std::unique_ptr<GameObject>>& scene, MobiusTransformation& camera) {
	Clear();
	SetCamera(camera);
	for (auto& obj : scene) {
		IRenderComponent* comp = obj->getComponent<RENDER>();
		if (comp) {
			PositionComponent* pos = obj->getComponent<POSITION>();
			MobiusTransformation model = pos ? pos->GetTransform() : MobiusTransformation::Identity();
			comp->SubmitTo(*this, model);
		}
	}
	Render();
}

void RenderSystem::Add(const LineRenderComponent& comp, MobiusTransformation& model) {
	MobiusGyrovector a = camera.Transform(model.Transform(comp.line.a));
	MobiusGyrovector b = camera.Transform(model.Transform(comp.line.b));
	primitives.emplace_back(CreateLinePrimitive({a, b}, comp.isSegment, comp.z, comp.weight, comp.isGyroWeight));
}

void RenderSystem::Add(const TexturedPolygon& comp, MobiusTransformation& model) {
	std::vector<MobiusGyrovector> points{ comp.vertices };
	for (int i = 0; i < points.size(); i++) {
		points[i] = model.Transform(points[i]);
		points[i] = camera.Transform(points[i]);
	}
	std::vector<glm::vec2> uvs{ comp.uvs };
	std::vector<RenderPrimitive> prims = CreatePrimitivesForConvexPolygon(points, uvs, comp.z, comp.texture->getUnit());

	primitives.insert(primitives.end(), prims.begin(), prims.end()); // the ugliest concat syntax known to man
}

RenderPrimitive RenderSystem::CreateLinePrimitive(const MobiusGyroline& line, bool isSegment, float z, double weight, bool isGyroWeight) {
	glm::vec2 empty{ 0.0, 0.0 };
	return RenderPrimitive{
		isSegment ? LINE_SEGMENT : LINE, // type
		0, // texUnit
		z,	// z
		0.0, // padding
		glm::vec2(line.a.x, line.a.y), // a
		glm::vec2(line.b.x, line.b.y), // b
		empty, // c
		empty, empty, empty, // uvs
		glm::vec2(line.center.real(), line.center.imag()), // center_ab
		empty, empty, // bc and ca
		line.radius, // r_ab
		weight, // line weight
		isGyroWeight ? -1.0 : 1.0 // whether to use euclidean (1.0) or hyperbolic (-1.0) distance in line weight
	};
}

RenderPrimitive RenderSystem::CreateTrianglePrimitive(
	MobiusGyrovector a, MobiusGyrovector b, MobiusGyrovector c, 
	glm::vec2 uv_a, glm::vec2 uv_b, glm::vec2 uv_c,
	float z, int texUnit
) {
	MobiusGyroline ab{ a, b }, bc{ b, c }, ca{ c, a };
	return RenderPrimitive{
		TRIANGLE,	// type=triangle
		texUnit,
		z, // z
		0.0, // padding
		glm::vec2(a.x, a.y), glm::vec2(b.x, b.y), glm::vec2(c.x, c.y), // points a, b, c
		uv_a, uv_b, uv_c,
		glm::vec2(ab.center.real(), ab.center.imag()), // center_ab
		glm::vec2(bc.center.real(), bc.center.imag()), // center_bc
		glm::vec2(ca.center.real(), ca.center.imag()), // center_ca
		(float)ab.radius, (float)bc.radius, (float)ca.radius
	};
}

std::vector<RenderPrimitive> RenderSystem::CreatePrimitivesForConvexPolygon(
	std::vector<MobiusGyrovector>& points, std::vector<glm::vec2>& uvs, 
	float z, int texUnit
) {
	points.push_back(points[0]);
	uvs.push_back(uvs[0]);
	std::vector<RenderPrimitive> primitives{ points.size() - 3 };
	for (int i = 0; i < points.size() - 3; i++) {
		primitives[i] = CreateTrianglePrimitive(points[0], points[i + 1], points[i + 2], uvs[0], uvs[i + 1], uvs[i + 2], z, texUnit); // Like a GL_FAN
	}
	points.pop_back();
	uvs.pop_back();
	return primitives;
}
