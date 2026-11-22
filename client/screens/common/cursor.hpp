#pragma once

#include "textbox.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>

enum class CursorType {
    Arrow,
    Hand,
    Text
};

void handleCursorInteractions(
    sf::RenderWindow& window,
    const std::vector<const sf::Sprite*>& buttons,
    const TextBox* textBox,
    const sf::Cursor& arrowCursor,
    const sf::Cursor& handCursor,
    const sf::Cursor& textCursor,
    CursorType& currentCursor
);
