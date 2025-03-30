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

private:
    void handleEvents();
    void update();
    void render();
    void drawHUD();
    void drawMarker();
    void initializeGame();
    void updateCamera();

    sf::RenderWindow window;
    sf::View gameView;
    Booster* booster;
    Platform* platform;
    sf::RectangleShape ground;
    sf::Clock clock;
    sf::Font font;
    
    // Добавленные поля
    sf::Texture groundTexture;
    sf::VertexArray groundVertices;
    sf::Vector2u windowSize; // Для хранения размера окна

    MenuScreen menuScreen;
    Marker marker;
};

#endif // GAME_H