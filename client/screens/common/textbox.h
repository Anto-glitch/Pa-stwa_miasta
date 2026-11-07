#pragma once

#include <SFML/Graphics.hpp>
#include <sstream>
#include <SFML/Window.hpp>

using namespace std;

#define DELETE_KEY 8
#define ENTER_KEY 13
#define ESCAPE_KEY 27

class TextBox {
    private:
        sf::Text text;
        bool isActive = false;
        int CharacterLimit = 20;
        ostringstream response;
    
        void InputLogic(int charTyped) {
            if(charTyped != DELETE_KEY && charTyped!=ESCAPE_KEY && charTyped!=ENTER_KEY) {
                if(response.str().length()<CharacterLimit){
                    response << static_cast<char>(charTyped);
                }
            else if(charTyped == DELETE_KEY){}
            else if(charTyped ==ESCAPE_KEY){
                setActive(false);
            }
            else if(charTyped == ENTER_KEY){
              //  sendResponse(socketId);
                setActive(false);

            }
            text.setString(response.str() + "_");
        }
    }
            
    public:
        TextBox(
            const sf::Font& font,
            unsigned int characterSize,
            const sf::Color& color,
            const sf::Vector2f& position
        )
            : text(font, "", characterSize)
        {
            text.setFillColor(color);
            text.setPosition(position);
        }
        void setLimit(int limit) {
            CharacterLimit = limit;
        }

        void sendResponse(int socketId) {
            //send_message(socketId, response);
        }
         
        void setActive(bool active) {
            isActive = active;
            if(!isActive) {
                text.setString(response.str());
            }
            else text.setString("_");
        }
        
        void draw(sf::RenderWindow& window) {
            window.draw(text);
        }

        void typedOn(const sf::Event::TextEntered& textEvent) {
            if(isActive) {
                const char32_t charTyped = textEvent.unicode;
                if(charTyped < 128) {
                    InputLogic(charTyped);
                }
            }
        }
        sf::FloatRect getGlobalBounds() const {
            return text.getGlobalBounds();
        }
};

