#pragma once
#include <type_traits>

#include "IComponent.h"
#include "ComponentType.h"

template <typename Derived>
struct Component : IComponent {
    // enforce CRTP in constructor
    Component() {
        static_assert(std::is_base_of_v<Component<Derived>, Derived>, "CRTP requires T to inherit from Base<T>");
    }

    static ComponentType GetStaticType() {
        static const ComponentType id = generateTypeId();
        return id;
    }

    ComponentType GetType() const override {
        return GetStaticType();
    }

};