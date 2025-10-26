#pragma once

#include "ComponentType.h"
#include "Component.h"
#include "Math/MobiusGyrovector.h"
#include "Math/MobiusTransformation.h"

/// <summary>
/// Stores the position (and rotation) of a GameObject
/// </summary>
class PositionComponent : public Component<PositionComponent> {
	MobiusTransformation _pos;

public:
	PositionComponent(MobiusTransformation initial_position_and_rotation) : _pos{ initial_position_and_rotation } {}
	PositionComponent(MobiusGyrovector initial_position) : _pos{ initial_position } {}
	PositionComponent() : _pos{ MobiusTransformation::Identity() } {}

	void Translate(const MobiusGyrovector& ds);

	MobiusTransformation GetTransform() { 
		return _pos; 
	}

};

using POSITION = PositionComponent;