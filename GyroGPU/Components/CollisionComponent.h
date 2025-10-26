#pragma once
#include <memory>
#include <type_traits>
#include <vector>

#include "ComponentType.h"
#include "Component.h"
#include "Shapes/IGeometry.h"
#include "Math/MobiusTransformation.h"

class CollisionComponent : public Component<CollisionComponent> {
	std::vector<std::unique_ptr<collision::IGeometry>> colliders;
public:
	// Geoms should be IGeometry subclasses
	template<typename... Geoms>
	CollisionComponent(Geoms&&... geoms) {
		(colliders.emplace_back(std::make_unique<std::decay_t<Geoms>>(std::forward<Geoms>(geoms))), ...);
	}

	bool Intersects(CollisionComponent* other) {
		for (auto& a : colliders) {
			for (auto& b : other->colliders) {
				if (a->Intersects(b.get())) {
					return true;
				}
			}
		}
		return false;
	}

};

