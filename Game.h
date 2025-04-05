#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "Booster.h"
#include "Platform.h"
#include "MenuScreen.h"
#include "Marker.h"

class Game {
public:
    Game();
    ~Game();
    void run();
    void showExitButton(const std::string& message, const sf::Color& color);

    bool isExitButtonVisible() const { return showExitButtonVisible; }

    const sf::Texture& getBoosterTexture() const { return boosterTexture; }
    const sf::Texture& getEngineTexture() const { return engineTexture; }
    const sf::Texture& getFlameTexture() const { return flameTexture; }
    const sf::Texture& getPlatformTexture() const { return platformTexture; }

private:
    void handleEvents();
    void update();
    void render();
    void drawHUD();
    void drawMarker();
    void initializeGame();
    void updateCamera();
    void initExitButton();
    void initRestartButton();
    void initMessageBackground();
    void handleExitButtonEvent(sf::Event& event);
    void handleRestartButtonEvent(sf::Event& event);
    void restartGame();

    sf::RenderWindow window;
    sf::View gameView;
    Booster* booster;
    Platform* platform;
    Platform* ground;
    sf::Clock clock;
    sf::Font font;
    sf::Vector2u windowSize;

    // Фон
    sf::Texture skyTexture;
    sf::Texture starsTexture;
    sf::Sprite skySprite;
    sf::Sprite starsSprite;

    MenuScreen menuScreen;
    Marker marker;
    sf::RectangleShape exitButton;
    sf::RectangleShape restartButton;
    sf::Text exitButtonText;
    sf::Text restartButtonText;
    sf::Text resultMessageText;
    bool showExitButtonVisible = false;
    std::string resultMessage;
    sf::Color messageColor;
    sf::RectangleShape messageBackground;

    sf::Texture boosterTexture;
    sf::Texture engineTexture;
    sf::Texture flameTexture;
    sf::Texture platformTexture;
    sf::Texture groundTexture;
};

#endif // GAME_H