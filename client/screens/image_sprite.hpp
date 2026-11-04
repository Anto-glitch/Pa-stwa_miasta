#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

bool generateScreenSpritesFromFiles(
    const std::vector<std::string>& filenames,
    std::vector<std::unique_ptr<sf::Texture>>& textures,
    std::vector<sf::Sprite>& sprites,
    const sf::Vector2f& position = {0.f, 0.f},
    const sf::Vector2f& size = {100.f, 100.f});

bool generateModuleSpritesFromFiles(
    std::vector<std::unique_ptr<sf::Texture>>& textures,
    std::vector<sf::Sprite>& sprites);
