#define _USE_MATH_DEFINES
#include "Gyrotile.h"
#include <algorithm>
#include <cmath>
#include <complex>
#include <limits>
#include <list>
#include <queue>
#include <vector>

#include "MobiusGyrovector.h"
#include "MobiusTransformation.h"
#include <iostream>

const double Gyrogrid::ComputeRadius(int num_sides, int tiles_per_vertex) {
	// Let O be the origin, D be the center of the euclidean circle defining the gyroline
	// Assume WLOG D lies on the real axis. Let A be a vertex of the n-gon such that its conjugate A'
	// is a neighboring point on the n-gon. 
	// The angle A'OA is 2pi/num_sides. DOA is half that. 
	// The angle DAO is intersected by the circle at D. Because DA is a radius of that, this divides
	// the angle into pi/2 + pi/tiles_per_vertex (DA-Circle + half of an interior angle of the n-gon)
	//
	// Note that the circle centered at D passing through A is orthogonal to the unit circle. 
	// Say it meets at C. DCO is a right triangle.
	// From there, law of cosines and pythagorean=> OA^2 + DA^2 - 2(OA)(DA)cos(DAO) = OD^2 = 1 + DA^2
	// Law of sines=> DA = OAsin(DOA)/sin(pi-DOA-DAO)
	// solve for OA
	double DOA = M_PI / num_sides;
	double DAO = M_PI / 2 + M_PI / tiles_per_vertex;
	return std::sqrt(
		1 /
		(1 - (
			2 * std::sin(DOA) * std::cos(DAO) /
			std::sin(M_PI - DOA - DAO)
			))
	);
}

Gyrogrid::Gyrogrid(int num_sides, int tiles_per_vertex, int max_depth, double max_distance) :
	radius{ ComputeRadius(num_sides, tiles_per_vertex) },
	ngon_base(num_sides),
	root{ num_sides, 0 } {
	Complex exponent = Complex(0, 1) * 2.0 * M_PI / (double)num_sides;
	for (int i = 0; i < num_sides; i++) {
		ngon_base[i] = MobiusGyrovector{ radius * std::exp(exponent * (double)(i)) };
	}
	GenerateGrid(num_sides, max_depth, max_distance);
}

Gyrogrid::Gyrogrid(int num_sides, int tiles_per_vertex, int max_depth) :
	Gyrogrid{ num_sides, tiles_per_vertex, max_depth, std::numeric_limits<double>::infinity() } {
}

Gyrogrid::Gyrogrid(int num_sides, int tiles_per_vertex, double max_distance) :
	Gyrogrid{ num_sides, tiles_per_vertex, -1, max_distance} { }

void Gyrogrid::GenerateGrid(int num_sides, int max_depth, double max_distance) {
	std::list<Gyrotile*> seen;
	std::queue<Gyrotile*> leaves;
	leaves.push(&root);
	seen.push_back(&root);

	std::vector<MobiusGyrovector> directions(num_sides);
	for (int i = 0; i < num_sides; i++) {
		// Get the midpoint of a side. That's the center of the neighboring tile
		MobiusGyrovector A = ngon_base[i];
		MobiusGyrovector B = ngon_base[i == num_sides - 1 ? 0 : i + 1];
		directions[i] = (A + (-A + B) * 0.5);
	}

	bool debug = true;

	while (!leaves.empty()) {
		Gyrotile* active = leaves.front();
		leaves.pop();

		if (active->depth == max_depth || MobiusGyrovector::Norm(active->pos.translation) > max_distance) {
			continue;
		}

		for (int i = 0; i < num_sides; i++) {
			MobiusGyrovector direction = directions[i] * 2.0;
			MobiusTransformation move_in_direction{ direction };
			MobiusTransformation childpos = MobiusTransformation{ active->pos }.Rotate(M_PI).Translate(active->pos(directions[i]) * 2.0);
			if (num_sides % 2 == 0) {
				childpos.rotation *= -1;
			}
			if (std::any_of(seen.begin(), seen.end(), [&](Gyrotile* node) -> bool {
				return MobiusGyrovector::ENormSquared(-node->pos.translation + childpos.translation) < radius * radius*0.1;
				})) {
				// we've seen this tile or a rotated version before; ignore it
				continue;
			}
			active->neighbors[i] = Gyrotile{ childpos, num_sides , active->depth + 1 };
			leaves.push(&(active->neighbors[i]));
			seen.push_back(&(active->neighbors[i]));
			
		}

	}
}

std::list<Gyrotile*> Gyrogrid::getAll() {
	MobiusTransformation zero{};
	std::list<Gyrotile*> tiles;
	std::queue<Gyrotile*> leaves;
	leaves.push(&root);

	while (!leaves.empty()) {
		Gyrotile* active = leaves.front();
		tiles.push_back(active);
		for (auto& neighbor : active->neighbors) {
			if (neighbor.isDefault()) {
				continue;
			}
			leaves.push(&neighbor);
		}
		leaves.pop();
	}
	return tiles;
}

std::vector<MobiusGyrovector> Gyrogrid::getPoints(Gyrotile* tile) {
	std::vector<MobiusGyrovector> points;
	for (auto& z : ngon_base) {
		points.push_back(tile->pos.Transform(z));
	}
	return points;
}