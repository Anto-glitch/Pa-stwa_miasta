#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>
#include <string>
#include <vector>

struct Module {
    std::string filename;
    sf::Vector2f position;
    sf::Vector2f size;
};

std::optional<sf::Sprite> generateImageSprite(
    sf::Texture& texture,
    const std::string& filename,
    const sf::Vector2f& position,
    const sf::Vector2f& size);

bool generateScreenSpritesFromFiles(
    const std::vector<std::string>& filenames,
    std::vector<std::unique_ptr<sf::Texture>>& textures,
    std::vector<sf::Sprite>& sprites,
    const sf::Vector2f& position = {0.f, 0.f},
    const sf::Vector2f& size = {100.f, 100.f});

bool generateModuleSpritesFromFiles(
    const std::vector<Module>& moduleConfigs,
    std::vector<std::unique_ptr<sf::Texture>>& textures,
    std::vector<sf::Sprite>& sprites);
