#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "Booster.h"
#include "Platform.h"

class Game {
public:
    Game();
    ~Game(); // Деструктор для очистки памяти
    void run();

private:
    void handleEvents();
    void update();
    void render();
    void drawHUD();
    void drawMarker();
    void initializeGame();

    sf::RenderWindow window; // Окно игры
    Booster* booster; // Указатель на бустер
    Platform platform; // Платформа
    sf::RectangleShape ground; // Земля
    sf::Clock clock; // Таймер для управления временем
    sf::Font font; // Шрифт для HUD
    bool windEnabled; // Включён ли ветер
    float gravity; // Гравитация
    bool engineTiltEnabled; // Включён ли наклон двигателей
};

#endif