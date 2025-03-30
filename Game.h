#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "Booster.h"
#include "Platform.h"
#include "MenuScreen.h"
#include "Marker.h"

// Класс Game - основной класс игры, управляющий всеми процессами
class Game {
public:
    Game();  // Конструктор
    ~Game(); // Деструктор
    void run(); // Главный цикл игры

private:
    void handleEvents(); // Обработка событий (ввод пользователя)
    void update();       // Обновление состояния игры
    void render();       // Отрисовка игровых объектов
    void drawHUD();      // Отрисовка интерфейса (HUD)
    void drawMarker();   // Отрисовка маркера за краем экрана
    void initializeGame(); // Инициализация игровых объектов

    // Основное окно игры
    sf::RenderWindow window;
    
    // Основные игровые объекты
    Booster* booster;    // Управляемый бустер
    Platform* platform;  // Посадочная платформа
    sf::RectangleShape ground; // Земля (нижняя граница)
    sf::Clock clock;     // Игровые часы для измерения времени между кадрами
    sf::Font font;       // Шрифт для текста

    // Дополнительные элементы
    MenuScreen menuScreen; // Экран меню
    Marker marker;       // Маркер для отображения положения бустера за краем экрана
};

#endif // GAME_H