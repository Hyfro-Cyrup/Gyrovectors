#include "VelocityComponent.h"
#include "Math/MobiusGyrovector.h"

void VelocityComponent::Accelerate(const MobiusGyrovector& dv) {
	_vel = dv + _vel;
}
