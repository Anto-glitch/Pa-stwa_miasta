
#include "cursor.hpp"

#include <SFML/Window.hpp>
#include <vector>

void handleCursorInteractions(
    sf::RenderWindow& window,
    const std::vector<sf::Sprite>& buttons,
    const TextBox& textBox,
    const sf::Cursor& arrowCursor,
    const sf::Cursor& handCursor,
    const sf::Cursor& textCursor,
    CursorType& currentCursor
) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    CursorType nextCursor = CursorType::Arrow;

    for (const auto& button : buttons) {
        if (button.getGlobalBounds().contains(mousePosF)) {
            nextCursor = CursorType::Hand;
            break;
        }
    }

    if (nextCursor == CursorType::Arrow &&
        textBox.getGlobalBounds().contains(mousePosF)) {
        nextCursor = CursorType::Text;
    }

    if (nextCursor == currentCursor) {
        return;
    }

    switch (nextCursor) {
    case CursorType::Arrow:
        window.setMouseCursor(arrowCursor);
        break;
    case CursorType::Hand:
        window.setMouseCursor(handCursor);
        break;
    case CursorType::Text:
        window.setMouseCursor(textCursor);
        break;
    }

    currentCursor = nextCursor;
}


