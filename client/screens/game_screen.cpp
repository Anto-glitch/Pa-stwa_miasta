#include "game_screen.hpp"

#include "common/text.hpp"

#include <string>

GameScreen::GameScreen(const sf::Font &font)
    : backgroundTexture(std::make_unique<sf::Texture>()),
      answerTextBox(font, 36, FONTCOLOR, {280.f, 425.f}, {250.f, 35.f}),
      letterAnnouncementText(createText("Wylosowana litera: ", font, 22,
                                        FONTCOLOR, {30.f, 110.f})),
      letterText(createText("", font, 48, FONTCOLOR, {30.f, 140.f})),
      promptText(createText("", font, 24, FONTCOLOR, {30.f, 200.f})) {
  backgroundSprite = generateImageSprite(
      *backgroundTexture, "../assets/graphics/gameScreen.png", {0.f, 0.f},
      {800.f, 600.f});
  loaded = backgroundSprite.has_value();
}

void GameScreen::syncTexts(const AppContext &ctx) {
  if (ctx.gameLetter != displayedLetter) {
    displayedLetter = ctx.gameLetter;
    letterText.setString(
        displayedLetter != '\0' ? std::string(1, displayedLetter) : "");
  }

  if (ctx.currentPrompt != displayedPrompt) {
    displayedPrompt = ctx.currentPrompt;
    promptText.setString(displayedPrompt);
    answerTextBox.setActive(true);
  }
}

void GameScreen::onEnter(AppContext &ctx) {
  answerTextBox.clear();
  answerTextBox.setActive(true);
  syncTexts(ctx);
}

void GameScreen::handleEvent(const sf::Event &event, AppContext &ctx) {
  if (const auto *mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
    if (mouseEvent->button == sf::Mouse::Button::Left) {
      sf::Vector2i mousePos = sf::Mouse::getPosition(ctx.window);
      sf::Vector2f mousePosF(mousePos);
      if (answerTextBox.getGlobalBounds().contains(mousePosF)) {
        answerTextBox.setActive(true);
      } else {
        answerTextBox.setActive(false);
      }
    }
  } else if (const auto *textEvent = event.getIf<sf::Event::TextEntered>()) {
    if (textEvent->unicode != 13 && textEvent->unicode != 10) {
      answerTextBox.typedOn(*textEvent);
    }
  } else if (const auto *keyEvent = event.getIf<sf::Event::KeyReleased>()) {
    if (keyEvent->code == sf::Keyboard::Key::Enter) {
      std::string answer = answerTextBox.getText();
      if (!answer.empty()) {
        ctx.netClient.sendLine(answer);
        answerTextBox.clear();
        answerTextBox.setActive(true);
      }
    }
  }
}

void GameScreen::update(AppContext &ctx) { syncTexts(ctx); }

void GameScreen::draw(sf::RenderWindow &window) {
  if (backgroundSprite) {
    window.draw(*backgroundSprite);
  }
  if (displayedLetter != '\0') {
    window.draw(letterAnnouncementText);
    window.draw(letterText);
  }
  window.draw(promptText);
  answerTextBox.draw(window);
}

const TextBox *GameScreen::getActiveTextBox(const AppContext &ctx) const {
  (void)ctx;
  return &answerTextBox;
}
