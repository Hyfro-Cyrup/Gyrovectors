#pragma once
#include <map>
#include <memory>
#include <type_traits>
#include <utility>

#include "Components/ComponentType.h"
#include "Components/IComponent.h"
#include "Math/MobiusTransformation.h"

class GameObject {
	std::map<ComponentType, std::unique_ptr<IComponent>> Components;

public:
	// constructor
	GameObject() : Components{} {}

	// enforce move constructors over copy. Can't copy a std::unique_ptr
	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;
	GameObject(GameObject&&) = default;
	GameObject& operator=(GameObject&&) = default;

	void addComponent(std::unique_ptr<IComponent> component) {
		Components[component->GetType()] = std::move(component);
	}

	template <typename T, typename ...Args>
	void addComponent(Args&&... args) {
		addComponent(std::make_unique<T>(std::forward<Args>(args)...));
	}

	template <typename T>
	T* getComponent() {
		auto item = Components.find(T::GetStaticType());
		if (item == Components.end()) {
			return nullptr;
		}
		return static_cast<T*>(item->second.get());
	}

	void Transform(const MobiusTransformation& transform) {
		// for each ITranslatable ComponentType, try to call translate on it
		//if (auto comp = getComponent<RENDER>()) comp->Transform(transform);
		//if (auto comp = getComponent<PHYSICS>()) comp->Transform(transform);
		//if (auto comp = getComponent<COLLISION>()) comp->Transform(transform);
	}
};