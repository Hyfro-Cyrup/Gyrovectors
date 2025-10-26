#pragma once
#include <memory>
#include <vector>

#include "GameObject.h"

class PhysicsSystem {

public:
	void Update(std::vector<std::unique_ptr<GameObject>>& scene);
};