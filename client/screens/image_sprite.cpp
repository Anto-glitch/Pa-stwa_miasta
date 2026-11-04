#include "image_sprite.hpp"

#include <iostream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

std::optional<sf::Sprite> generateImageSprite(
    sf::Texture& texture,
    const std::string& filename,
    const sf::Vector2f& position,
    const sf::Vector2f& size)
{
    if (!texture.loadFromFile(filename)) {
        std::cerr << "Failed to load image: " << filename << '\n';
        return std::nullopt;
    }

    sf::Sprite sprite(texture);
    sprite.setPosition(position);

    const auto textureSize = texture.getSize();
    sprite.setScale({
        size.x / static_cast<float>(textureSize.x),
        size.y / static_cast<float>(textureSize.y)
    });

    return sprite;
}


std::vector<std::string> getAssetsFilenames(const std::string& directoryPath) {
    std::vector<std::string> filenames;
    for (const auto &entry : fs::directory_iterator(directoryPath)){
        if (entry.is_regular_file()) {
            filenames.push_back(entry.path().string());
        }
    }
    return filenames;
}

bool generateScreenSpritesFromFiles(
    const std::vector<std::string>& filenames,
    std::vector<sf::Texture>& textures,
    std::vector<sf::Sprite>& sprites,
    const sf::Vector2f& position,
    const sf::Vector2f& size)
{
    textures.clear();
    sprites.clear();
    textures.reserve(filenames.size());
    sprites.reserve(filenames.size());

    for (const auto& filename : filenames) {
        textures.emplace_back();
        sf::Texture& texture = textures.back();

        if (!texture.loadFromFile(filename)) {
            std::cerr << "Failed to load image: " << filename << '\n';
            return false;
        }

        sf::Sprite sprite(texture);
        sprite.setPosition(position);
        const auto textureSize = texture.getSize();
        sprite.setScale({
            size.x / static_cast<float>(textureSize.x),
            size.y / static_cast<float>(textureSize.y)
        });
        sprites.push_back(std::move(sprite));
    }

    return true;
}

bool generateOtherSpritesFromFiles(
    std::vector<sf::Texture>& textures,
    std::vector<sf::Sprite>& sprites)
{
    textures.clear();
    sprites.clear();
    textures.emplace_back();
        std::optional<sf::Sprite> spriteOpt = generateImageSprite(textures.back(), "../assets/button.png", {300.f, 495.f}, {180.f, 80.f});
        if(!spriteOpt) {
            std::cerr << "Failed to generate sprite from texture\n";
            return false;
        }
        sprites.push_back(std::move(spriteOpt.value()));

    return true;
}
