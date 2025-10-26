#pragma once
#include <unordered_map>
#include <string>
#include <memory>   //shared_ptr
#include <queue>

#include "Texture.h"

/// <summary>
/// Manages resources surrounding textures, ensuring unnecessary textures don't eat away at GPU memory
/// 
/// call `get("GUY")` to get a shared pointer to the texture for Guy, creating it if it does not exist. 
/// call `clearUnused()` periodically or between scenes to clear up memory from textures that are no longer in use. 
/// 
/// </summary>
class TextureManager
{
    std::unordered_map<std::string, std::shared_ptr<Texture>> activeTextures;
    std::queue<int> freeUnits;
    int nextAvailableUnit = 1;  // we might be able to start at 0. Either that or the compute shader output is bound there. 

public:
    TextureManager();

    std::shared_ptr<Texture> get(const std::string&);

    void clearUnused();
};

