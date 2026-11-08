#include "generate_sprite.hpp"

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

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

bool generateScreenSpritesFromFiles(
    const std::vector<std::string>& filenames,
    std::vector<std::unique_ptr<sf::Texture>>& textures,
    std::vector<sf::Sprite>& sprites,
    const sf::Vector2f& position,
    const sf::Vector2f& size)
{
    textures.clear();
    sprites.clear();
    textures.reserve(filenames.size());
    sprites.reserve(filenames.size());

    for (const auto& filename : filenames) {
        std::unique_ptr<sf::Texture> texture = std::make_unique<sf::Texture>();
        auto spriteOpt = generateImageSprite(*texture, filename, position, size);
        textures.push_back(std::move(texture));
        if (!spriteOpt) {
            std::cerr << "Failed to generate sprite from file: " << filename << '\n';
            return false;
        }
        sprites.push_back(std::move(spriteOpt.value()));
    }

    return true;
}

bool generateModuleSpritesFromFiles(
    std::vector<std::unique_ptr<sf::Texture>>& textures,
    std::vector<sf::Sprite>& sprites)
{
    textures.clear();
    sprites.clear();
        std::unique_ptr<sf::Texture> texture = std::make_unique<sf::Texture>();
        auto spriteOpt = generateImageSprite(*texture, "../assets/graphics/button.png", {300.f, 495.f}, {180.f, 80.f});
        if(!spriteOpt) {
            std::cerr << "Failed to generate sprite from texture\n";
            return false;
        }
        textures.push_back(std::move(texture));
        sprites.push_back(std::move(spriteOpt.value()));

        texture = std::make_unique<sf::Texture>();
        spriteOpt = generateImageSprite(*texture, "../assets/graphics/exitbutton.png", {660.f, 10.f}, {130.f, 65.f});
        if(!spriteOpt) {
            std::cerr << "Failed to generate sprite from texture\n";
            return false;
        }
        textures.push_back(std::move(texture));
        sprites.push_back(std::move(spriteOpt.value()));

        texture = std::make_unique<sf::Texture>();
        spriteOpt = generateImageSprite(*texture, "../assets/graphics/textplaceholder.png", {280.f, 470.f}, {220.f, 5.f});
        if(!spriteOpt) {
            std::cerr << "Failed to generate sprite from texture\n";
            return false;
        }
        textures.push_back(std::move(texture));
        sprites.push_back(std::move(spriteOpt.value()));
        

    return true;
}
