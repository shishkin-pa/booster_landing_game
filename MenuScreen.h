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
    void resetMenu();  // Добавляем новый метод для сброса меню

    bool isEngineTiltEnabled() const;
    bool isWindEnabled() const;
    sf::Vector2f getWindForce() const;
    float getGravity() const;

private:
    void createMenu();
    void updateButtonHover();

    sf::RenderWindow& window;
    sf::Font& font;

    bool menuActive;
    int menuStep;

    // Текстуры кнопок
    sf::Texture tiltYesTexture;
    sf::Texture tiltNoTexture;
    sf::Texture windNoneTexture;
    sf::Texture windWeakTexture;
    sf::Texture windModerateTexture;
    sf::Texture windStrongTexture;
    sf::Texture startGameTexture;
    sf::Texture gravityMinTexture;

    // Кнопки
    sf::RectangleShape tiltYesButton;
    sf::RectangleShape tiltNoButton;
    std::vector<sf::RectangleShape> windButtons;
    sf::RectangleShape startButton;
    sf::RectangleShape gravityMinButton;

    // Элементы слайдера
    sf::RectangleShape gravitySlider;
    sf::RectangleShape gravitySliderTrack;
    sf::RectangleShape gravitySliderHandle;

    // Настройки
    bool engineTiltEnabled;
    bool windEnabled;
    sf::Vector2f windForce;
    float gravity;
    bool isDraggingSlider;

    sf::Texture splashTexture;
    sf::RectangleShape splashScreen;
    bool showSplash;
};

#endif // MENUSCREEN_H