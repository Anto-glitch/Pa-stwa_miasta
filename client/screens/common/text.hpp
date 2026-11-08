#include "SFML/Graphics.hpp"
#include <string>

sf::Font createFont();
sf::Text createText(const std::string& textString, const sf::Font& font, unsigned int characterSize, const sf::Color& color, const sf::Vector2f& position);