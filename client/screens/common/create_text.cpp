#include "text.hpp"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

sf::Font createFont() {
    sf::Font font;
    if (!font.openFromFile("../assets/fonts/FORTE.ttf")) {
        std::cerr << "Failed to load font from file: ../../assets/FORTE.ttf" << std::endl;
    }
    return font;
}

sf::Text createText(const std::string& textString, const sf::Font& font, unsigned int characterSize, const sf::Color& color, const sf::Vector2f& position) {
    sf::Text text(font);
    text.setString(textString);
    text.setCharacterSize(characterSize);
    text.setFillColor(color);
    text.setPosition(position);
    return text;
}