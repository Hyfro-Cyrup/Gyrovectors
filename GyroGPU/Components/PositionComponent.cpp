#include "PositionComponent.h"
#include "Math/MobiusGyrovector.h"

void PositionComponent::Translate(const MobiusGyrovector& ds) {
	_pos.Translate(ds);
}
