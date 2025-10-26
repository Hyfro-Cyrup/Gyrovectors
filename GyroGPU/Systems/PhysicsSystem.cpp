#include "PhysicsSystem.h"
#include <memory>
#include <vector>

#include "Components/ComponentType.h"
#include "Components/PositionComponent.h"
#include "Components/VelocityComponent.h"
#include "GameObject.h"

void PhysicsSystem::Update(std::vector<std::unique_ptr<GameObject>>& scene)
{
	for (auto& obj : scene) {
		PositionComponent* pos = obj->getComponent<POSITION>();
		VelocityComponent* vel = obj->getComponent<VELOCITY>();
		if (pos && vel) {
			pos->Translate(vel->GetVelocity());
		}
	}
}
