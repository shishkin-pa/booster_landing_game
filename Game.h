#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <string>
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
    void initResultMessage();
    void handleExitButtonEvent(sf::Event& event);
    void handleRestartButtonEvent(sf::Event& event);
    void restartGame();

    sf::RenderWindow window;
    sf::View gameView;
    Booster* booster;
    Platform* platform;
    sf::RectangleShape ground;
    sf::Clock clock;
    sf::Font font;
    sf::Texture groundTexture;
    sf::VertexArray groundVertices;
    sf::Vector2u windowSize;

    MenuScreen menuScreen;
    Marker marker;

    // Кнопки и сообщения
    sf::RectangleShape exitButton;
    sf::RectangleShape restartButton;
    sf::Text exitButtonText;
    sf::Text restartButtonText;
    sf::Text resultMessageText;
    bool showExitButtonVisible = false;
    std::string resultMessage;
    sf::Color messageColor;
    sf::RectangleShape messageBackground;
    void initMessageBackground();
};

#endif // GAME_H