#pragma once

#include <SFML/Graphics.hpp>

bool handleExitButton(
    const sf::Event& event,
    sf::RenderWindow& window,
    const sf::Sprite& exitButton,
    int& socketId,
    bool isStartScreen);
