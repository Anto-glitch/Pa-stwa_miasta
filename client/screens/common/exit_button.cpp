#include "exit_button.hpp"

#include "../../systems/socket_connection.hpp"

bool handleExitButton(
    const sf::Event& event,
    sf::RenderWindow& window,
    const sf::Sprite& exitButton,
    int& socketId,
    bool isStartScreen)
{
    const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
    if (mouseEvent == nullptr || mouseEvent->button != sf::Mouse::Button::Left) {
        return false;
    }

    const sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    if (!exitButton.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
        return false;
    }

    if (isStartScreen) {
        window.close();
    } else {
        disconnect_connection(socketId);
    }

    return true;
}
