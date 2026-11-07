#pragma once

#include <SFML/Graphics.hpp>
#include <sstream>
#include <string>
#include <SFML/Window.hpp>

using namespace std;

#define DELETE_KEY 8
#define ENTER_KEY 13
#define ESCAPE_KEY 27

class TextBox {
private:
    sf::Text text;
    sf::Vector2f boxPosition;
    sf::Vector2f boxSize;
    bool isActive = false;
    int CharacterLimit = 20;
    ostringstream response;

    void deleteLastChar() {
        string current = response.str();
        if (!current.empty()) {
            current.pop_back();
            response.str("");
            response.clear();
            response << current;
        }
    }

    void InputLogic(int charTyped) {
        if (charTyped == DELETE_KEY || charTyped == 127) {
            deleteLastChar();
        } else if (charTyped == ESCAPE_KEY) {
            setActive(false);
            return;
        } else if (charTyped == ENTER_KEY) {
            // sendResponse(socketId);
            setActive(false);
            return;
        } else if (charTyped >= 32) {
            if (response.str().length() < static_cast<size_t>(CharacterLimit)) {
                response << static_cast<char>(charTyped);
            }
        }
        text.setString(response.str() + "_");
    }

public:
    TextBox(
        const sf::Font& font,
        unsigned int characterSize,
        const sf::Color& color,
        const sf::Vector2f& position,
        const sf::Vector2f& size = {220.f, 35.f}
    )
        : text(font, "", characterSize),
          boxPosition(position),
          boxSize(size)
    {
        text.setFillColor(color);
        text.setPosition(position);
    }

    void setLimit(int limit) {
        CharacterLimit = limit;
    }

    string getText() const {
        return response.str();
    }

    void sendResponse(int socketId) {
        // send_message(socketId, response);
    }

    void setActive(bool active) {
        isActive = active;
        if (isActive) {
            text.setString(response.str() + "_");
        } else {
            text.setString(response.str());
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(text);
    }

    void typedOn(const sf::Event::TextEntered& textEvent) {
        if (isActive) {
            const char32_t charTyped = textEvent.unicode;
            if (charTyped < 128) {
                InputLogic(charTyped);
            }
        }
    }

    sf::FloatRect getGlobalBounds() const {
        return sf::FloatRect(boxPosition, boxSize);
    }
};
