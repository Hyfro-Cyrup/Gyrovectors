#pragma once
#include <vector>
#include <list>

#include "MobiusTransformation.h"
#include "MobiusGyrovector.h"

struct Gyrotile {
	MobiusTransformation pos;	// stores the transformation necessary to go from the center tile to this one
	std::vector<Gyrotile> neighbors;
	int depth;

	Gyrotile() : pos{}, neighbors{}, depth{ -1 } {};
	Gyrotile(int num_sides) : Gyrotile{ num_sides, -1 } {} // depth -1 means tile was default constructed
	Gyrotile(int num_sides, int depth) : pos{}, neighbors( num_sides ), depth{ depth } {}
	Gyrotile(MobiusTransformation pos, int num_sides, int depth) : pos{pos}, neighbors( num_sides ), depth{ depth } {}

	bool isDefault() const { return depth == -1; }
};

/// <summary>
/// Constructs a grid of tiles as a tree starting from (0, 0). 
/// Each tile stores the MobiusTransformation (`pos`) that maps from the `base_ngon` to that tile. 
/// </summary>
class Gyrogrid {
	const double radius;
	
	const double ComputeRadius(int num_sides, int tiles_per_vertex);

	std::vector<MobiusGyrovector> ngon_base;
	Gyrotile root;

public:
	Gyrogrid(int num_sides, int tiles_per_vertex, int max_depth, double max_distance);
	Gyrogrid(int num_sides, int tiles_per_vertex, int max_depth);
	Gyrogrid(int num_sides, int tiles_per_vertex, double max_distance);

private:
	void GenerateGrid(int num_sides, int max_depth, double max_distance);

public:
	std::list<Gyrotile*> getAll();

	inline double getRadius() const { return radius; }
	inline std::vector<MobiusGyrovector> getBasePoints() const { return ngon_base; }

	std::vector<MobiusGyrovector> getPoints(Gyrotile* tile);
};


