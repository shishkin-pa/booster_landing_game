#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include <SFML/Graphics.hpp>
#include <vector>

class MenuScreen {
public:
    MenuScreen(sf::RenderWindow& window, sf::Font& font);
    void draw();
    void handleEvent(sf::Event& event);
    bool isMenuActive() const;
    void setMenuActive(bool active);

    bool isEngineTiltEnabled() const;
    bool isWindEnabled() const;
    sf::Vector2f getWindForce() const;
    float getGravity() const;

private:
    void createMenu();

    sf::RenderWindow& window;
    sf::Font& font;

    bool menuActive;
    int menuStep;

    // Элементы для выбора наклона двигателей
    sf::RectangleShape tiltYesButton;
    sf::RectangleShape tiltNoButton;
    sf::Text tiltYesLabel;
    sf::Text tiltNoLabel;

    // Элементы для выбора ветра
    std::vector<sf::RectangleShape> windButtons;
    std::vector<sf::Text> windButtonLabels;

    // Элементы для настройки гравитации
    sf::RectangleShape gravitySlider;
    sf::RectangleShape gravitySliderHandle;
    sf::Text gravityLabel;

    // Общие элементы
    sf::Text menuTitle;
    sf::RectangleShape startButton;
    sf::Text startButtonLabel;

    // Настройки
    bool engineTiltEnabled;
    bool windEnabled;
    sf::Vector2f windForce;
    float gravity;
    bool isDraggingSlider;
};

#endif // MENUSCREEN_H