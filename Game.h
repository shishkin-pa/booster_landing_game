#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "Booster.h" // Добавлено включение заголовочного файла Booster.h
#include "Platform.h" // Добавлено включение заголовочного файла Platform.h
#include "MenuScreen.h" // Добавлено включение заголовочного файла MenuScreen.h

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

    sf::RenderWindow window;
    Booster* booster; // Теперь компилятор знает, что такое Booster
    Platform* platform; // Теперь компилятор знает, что такое Platform
    sf::RectangleShape ground;
    sf::Clock clock;
    sf::Font font;

    MenuScreen menuScreen; // Объект меню
};

#endif