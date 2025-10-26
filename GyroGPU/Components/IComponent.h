#pragma once

#include "ComponentType.h"

struct IComponent {
	virtual ~IComponent() = default;

private:
	// Only Component<T> can inherit from IComponent because only Component<T> can construct it
	template <typename T>
	friend struct Component;
	IComponent() = default;

public:
	virtual ComponentType GetType() const = 0;
};