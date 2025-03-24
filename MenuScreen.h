#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include <SFML/Graphics.hpp>
#include <vector>

enum class MarkerType {
    CIRCLE,
    TRIANGLE
};

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
    MarkerType getMarkerType() const;

private:
    void createMenu();

    sf::RenderWindow& window;
    sf::Font& font;

    bool menuActive;
    int menuStep;
    MarkerType markerType;

    std::vector<sf::RectangleShape> markerButtons;
    std::vector<sf::Text> markerButtonLabels;
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

    bool engineTiltEnabled;
    bool windEnabled;
    sf::Vector2f windForce;
    float gravity;
    bool isDraggingSlider;
};

#endif