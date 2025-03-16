#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "Booster.h"
#include "Platform.h"
#include <vector>

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
    void createMenu();
    void handleMenuEvents(sf::Event& event);

    sf::RenderWindow window; // Окно игры
    Booster* booster; // Указатель на бустер
    Platform* platform; // Указатель на платформу
    sf::RectangleShape ground; // Земля
    sf::Clock clock; // Таймер для управления временем
    sf::Font font; // Шрифт для HUD
    bool windEnabled; // Включён ли ветер
    float gravity; // Гравитация
    bool engineTiltEnabled; // Включён ли наклон двигателей

    // Меню
    std::vector<sf::RectangleShape> windButtons;
    std::vector<sf::Text> windButtonLabels;
    sf::RectangleShape tiltYesButton;
    sf::RectangleShape tiltNoButton;
    sf::Text tiltYesLabel;
    sf::Text tiltNoLabel;
    sf::RectangleShape gravitySlider;
    sf::RectangleShape gravitySliderHandle;
    sf::Text gravityLabel;
    sf::Text menuTitle; // Заголовок меню
    sf::RectangleShape startButton; // Кнопка для запуска игры
    sf::Text startButtonLabel; // Текст кнопки запуска
    bool menuActive; // Активно ли меню
    int menuStep; // Текущий шаг меню (0: наклон двигателей, 1: ветер, 2: гравитация)

    // Сила ветра
    sf::Vector2f windForce; // Сохраняем силу ветра для последующего применения

    // Флаг для перетаскивания слайдера
    bool isDraggingSlider; // Флаг для отслеживания перетаскивания слайдера
};

#endif