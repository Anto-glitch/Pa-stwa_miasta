#include "image_sprite.hpp"

#include <iostream>

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
