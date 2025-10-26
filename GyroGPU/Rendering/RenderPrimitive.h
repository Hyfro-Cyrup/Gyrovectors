#pragma once

#include <glm/ext/vector_float2.hpp>

enum RenderPrimitiveType {
	TRIANGLE, 
	LINE,
	LINE_SEGMENT,
};

struct RenderPrimitive {
	int type{ 0 };
	int texUnit{ 0 };	   // texture unit number
	float z{ 0 };           // z height
	float _pad{ 0 };	// padding. ints and floats must appear in 8 byte chunks. 
	glm::vec2 a{ 0 }, b{ 0 }, c{ 0 }; // Points on the triangle. c unused if type 1
	glm::vec2 uv_a{ 0 }, uv_b{ 0 }, uv_c{ 0 }; // uv coords. unused by type 1
	glm::vec2 center_ab{ 0 }, center_bc{ 0 }, center_ca{ 0 }; // centers of each line as a Euclidean circle. bc and ca unused if type 1
	double r_ab{ 0 }, r_bc{ 0 }, r_ca{ 0 }; // radii of each line as a Euclidean circle. bc and ca unused if type 1
};



