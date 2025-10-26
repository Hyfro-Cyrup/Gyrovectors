#pragma once

#include "ComponentType.h"
#include "Component.h"
#include "Math/MobiusGyrovector.h"


/// <summary>
/// Stores the velocity of a GameObject
/// </summary>
class VelocityComponent : public Component<VelocityComponent> {
	MobiusGyrovector _vel;

public:
	VelocityComponent(MobiusGyrovector initial_velocity) : _vel{ initial_velocity } {}

	void Accelerate(const MobiusGyrovector& dv);

	MobiusGyrovector GetVelocity() { return _vel; }
};

using VELOCITY = VelocityComponent;
