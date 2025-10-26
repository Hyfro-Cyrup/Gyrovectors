#pragma once
#include <algorithm>
#include <memory> 
#include <vector>

#include <glm/ext/vector_float2.hpp>

#include "Rendering/RenderPrimitive.h"
#include "Rendering/Renderer.h"
#include "Math/MobiusGyrovector.h"
#include "Math/MobiusGyroline.h"
#include "Math/MobiusTransformation.h"
#include "Components/Painted/LineRenderComponent.h"
#include "Components/Textured/TexturedPolygon.h"
#include "GameObject.h"

class RenderSystem {
	std::vector<RenderPrimitive> primitives;
	Renderer renderer;
	MobiusTransformation camera;

public:
	// constructor
	RenderSystem(int width, int height) : renderer{ width, height } {}

	void RenderScene(std::vector<std::unique_ptr<GameObject>>& scene, MobiusTransformation& camera);

	void Resize(int width, int height) {
		renderer.Resize(width, height);
	}

private:
	// push to low-level renderer
	void Render() {
		// Sort so higher z is first
		std::vector<RenderPrimitive> sorted = primitives;
		std::sort(sorted.begin(), sorted.end(),
			[](const auto& a, const auto& b) { return a.z > b.z; });

		renderer.PopulateComputeBuffer(primitives);
		renderer.Render();
	}

	void Clear() {
		primitives.clear();
	}

	void SetCamera(MobiusTransformation& new_camera) {
		camera = new_camera;
	}

public:
	//
	// component conversion
	//

	void Add(const LineRenderComponent& comp, MobiusTransformation& model);

	void Add(const TexturedPolygon& comp, MobiusTransformation& model);

private:
	//
	// factory methods
	//
	RenderPrimitive CreateLinePrimitive(const MobiusGyroline& line, bool isSegment, float z, double weight, bool isGyroWeight);

	RenderPrimitive CreateTrianglePrimitive(
		MobiusGyrovector a, MobiusGyrovector b, MobiusGyrovector c, 
		glm::vec2 uv_a, glm::vec2 uv_b, glm::vec2 uv_c,
		float z, int texUnit
	);

	std::vector<RenderPrimitive> CreatePrimitivesForConvexPolygon(
		std::vector<MobiusGyrovector>& points, std::vector<glm::vec2>& uvs, 
		float z, int texUnit
	);

	//std::vector<RenderPrimitive> CreatePrimitivesForLineStrip(std::vector<MobiusGyrovector>& points, bool closed, float line_weight) {
	//	if (closed) {
	//		points.push_back(points[0]);
	//	}
	//	std::vector<RenderPrimitive> primitives{ points.size() - 1 };
	//	for (int i = 0; i < points.size() - 1; i++) {
	//		primitives[i] = CreateLinePrimitive(MobiusGyroline{ points[i], points[i + 1] }, true, 1.0, line_weight, true);
	//	}
	//	if (closed) {
	//		points.pop_back();
	//	}
	//	return primitives;
	//}

	

};