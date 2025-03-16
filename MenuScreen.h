#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include <SFML/Graphics.hpp>
#include <vector>

class MenuScreen {
public:
    MenuScreen(sf::RenderWindow& window, sf::Font& font);

    void draw(); // Отрисовка меню
    void handleEvent(sf::Event& event); // Обработка событий меню
    bool isMenuActive() const; // Возвращает, активно ли меню
    void setMenuActive(bool active); // Устанавливает активность меню

    // Геттеры для параметров игры
    bool isEngineTiltEnabled() const;
    bool isWindEnabled() const;
    sf::Vector2f getWindForce() const;
    float getGravity() const;

private:
    void createMenu(); // Создание элементов меню

    sf::RenderWindow& window; // Ссылка на окно игры
    sf::Font& font; // Ссылка на шрифт

    // Параметры меню
    bool menuActive; // Активно ли меню
    int menuStep; // Текущий шаг меню (0: наклон двигателей, 1: ветер, 2: гравитация)

    // Элементы меню
    std::vector<sf::RectangleShape> windButtons;
    std::vector<sf::Text> windButtonLabels;
    sf::RectangleShape tiltYesButton;
    sf::RectangleShape tiltNoButton;
    sf::Text tiltYesLabel;
    sf::Text tiltNoLabel;
    sf::RectangleShape gravitySlider;
    sf::RectangleShape gravitySliderHandle;
    sf::Text gravityLabel;
    sf::Text menuTitle;
    sf::RectangleShape startButton;
    sf::Text startButtonLabel;

    // Параметры игры
    bool engineTiltEnabled; // Включён ли наклон двигателей
    bool windEnabled; // Включён ли ветер
    sf::Vector2f windForce; // Сила ветра
    float gravity; // Гравитация

    // Флаг для перетаскивания слайдера
    bool isDraggingSlider;
};

#endif