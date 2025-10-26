#include "TextureManager.h"

#include <memory>
#include <string>

#include "Texture.h"

TextureManager::TextureManager() : 
	activeTextures{} {}

std::shared_ptr<Texture> TextureManager::get(const std::string& path) {
    auto item = activeTextures.find(path);
    if (item == activeTextures.end()) {
        // Texture not found, load and store it
        // First, get the index of the first free texture unit
        int unit;
        if (!freeUnits.empty()) {
            unit = freeUnits.front();
            freeUnits.pop();
        }
        else {
            unit = nextAvailableUnit++;
        }
        std::shared_ptr<Texture> texture = std::make_shared<Texture>(unit, path);
        activeTextures[path] = texture;
        return texture;
    }
    // Texture found, return it
    return item->second;
}

void TextureManager::clearUnused() {
    for (auto item = activeTextures.begin(); item != activeTextures.end(); ) {
        if (item->second.use_count() == 1) {
            // Only TextureManager holds a reference, safe to delete
            item = activeTextures.erase(item);
            // mark this unit as freed up
            freeUnits.push(item->second->getUnit());
        }
        else {
            ++item;
        }
    }
}