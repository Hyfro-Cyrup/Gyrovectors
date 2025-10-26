#pragma once

/// <summary>
/// Encapsulation/Abstraction of ComponentType generation. Unlikely to change, but good to have separate anyway. 
/// </summary>

using ComponentType = size_t;

inline ComponentType generateTypeId() {
    static ComponentType counter = 0;
    return counter++;
}