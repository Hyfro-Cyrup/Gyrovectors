#define _USE_MATH_DEFINES
#include <cmath>
#include "RegularTexturedPolygon.h"
#include <vector>
#include <glm/ext/vector_float2.hpp>


std::vector<glm::vec2> RegularTexturedPolygon::GetUVs(int num_sides) {
	if (num_sides == 4) {
		// cheap stopgap to maximize area used for rectangular textures. 
		// TODO: Maximize areas correctly
		return {
			{0.0, 0.0},
			{1.0, 0.0},
			{1.0, 1.0},
			{0.0, 1.0}
		};
	}
	std::vector<glm::vec2> uvs(num_sides);
	double coeff{ 2 * M_PI / num_sides };
	double theta;
	for (int i = 0; i < num_sides; i++) {
		theta = coeff * i;
		uvs[i].x = (float)(0.5 * std::cos(theta) + 0.5);
		uvs[i].y = (float)(0.5 * std::sin(theta) + 0.5);
	}
	return uvs;
}


