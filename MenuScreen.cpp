#include "MenuScreen.h"
#include <cstdlib>
#include <ctime>

MenuScreen::MenuScreen(sf::RenderWindow& window, sf::Font& font)
    : window(window), font(font), menuActive(true), menuStep(0),
      engineTiltEnabled(true), windEnabled(false), windForce(0.f, 0.f),
      gravity(200.0f), isDraggingSlider(false) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    createMenu();
}

void MenuScreen::createMenu() {
    sf::Vector2u windowSize = window.getSize();
    float centerX = windowSize.x / 2.0f;
    float centerY = windowSize.y / 2.0f;

    // Настройка заголовка меню
    menuTitle.setFont(font);
    menuTitle.setCharacterSize(40);
    menuTitle.setFillColor(sf::Color::White);
    menuTitle.setPosition(centerX - 150, centerY - 200);

    // Кнопки выбора наклона двигателей (Шаг 1)
    tiltYesButton.setSize(sf::Vector2f(200, 50));
    tiltYesButton.setPosition(centerX - 250, centerY - 100);
    tiltYesButton.setFillColor(sf::Color::White);
    tiltYesButton.setOutlineThickness(2);
    tiltYesButton.setOutlineColor(sf::Color::Black);

    tiltNoButton.setSize(sf::Vector2f(200, 50));
    tiltNoButton.setPosition(centerX + 50, centerY - 100);
    tiltNoButton.setFillColor(sf::Color::White);
    tiltNoButton.setOutlineThickness(2);
    tiltNoButton.setOutlineColor(sf::Color::Black);

    tiltYesLabel.setFont(font);
    tiltYesLabel.setString("Yes");
    tiltYesLabel.setCharacterSize(24);
    tiltYesLabel.setFillColor(sf::Color::Black);
    tiltYesLabel.setPosition(centerX - 240, centerY - 90);

    tiltNoLabel.setFont(font);
    tiltNoLabel.setString("No");
    tiltNoLabel.setCharacterSize(24);
    tiltNoLabel.setFillColor(sf::Color::Black);
    tiltNoLabel.setPosition(centerX + 60, centerY - 90);

    // Кнопки выбора ветра (Шаг 2)
    std::vector<std::string> windOptions = {"No Wind", "Weak Wind", "Moderate Wind", "Strong Wind"};
    for (int i = 0; i < windOptions.size(); ++i) {
        sf::RectangleShape button(sf::Vector2f(200, 50));
        button.setPosition(centerX - 100, centerY - 50 + i * 60);
        button.setFillColor(sf::Color::White);
        button.setOutlineThickness(2);
        button.setOutlineColor(sf::Color::Black);
        windButtons.push_back(button);

        sf::Text label;
        label.setFont(font);
        label.setString(windOptions[i]);
        label.setCharacterSize(24);
        label.setFillColor(sf::Color::Black);
        label.setPosition(centerX - 90, centerY - 40 + i * 60);
        windButtonLabels.push_back(label);
    }

    // Слайдер гравитации (Шаг 3)
    gravitySlider.setSize(sf::Vector2f(300, 20));
    gravitySlider.setPosition(centerX - 150, centerY + 150);
    gravitySlider.setFillColor(sf::Color::White);
    gravitySlider.setOutlineThickness(2);
    gravitySlider.setOutlineColor(sf::Color::Black);

    float initialSliderX = centerX - 150 + (gravity / 400.0f) * gravitySlider.getSize().x;
    gravitySliderHandle.setSize(sf::Vector2f(20, 40));
    gravitySliderHandle.setPosition(initialSliderX, centerY + 140);
    gravitySliderHandle.setFillColor(sf::Color::Red);
    gravitySliderHandle.setOutlineThickness(2);
    gravitySliderHandle.setOutlineColor(sf::Color::Black);

    gravityLabel.setFont(font);
    gravityLabel.setString("Gravity: " + std::to_string(static_cast<int>(gravity)));
    gravityLabel.setCharacterSize(24);
    gravityLabel.setFillColor(sf::Color::White);
    gravityLabel.setPosition(centerX - 150, centerY + 180);

    // Кнопка старта игры
    startButton.setSize(sf::Vector2f(200, 50));
    startButton.setPosition(centerX - 100, centerY + 250);
    startButton.setFillColor(sf::Color::Green);
    startButton.setOutlineThickness(2);
    startButton.setOutlineColor(sf::Color::Black);

    startButtonLabel.setFont(font);
    startButtonLabel.setString("Start Game");
    startButtonLabel.setCharacterSize(24);
    startButtonLabel.setFillColor(sf::Color::Black);
    startButtonLabel.setPosition(centerX - 90, centerY + 260);
}

void MenuScreen::draw() {
    if (!menuActive) return;

    window.clear(sf::Color(50, 50, 50));

    if (menuStep == 0) {
        menuTitle.setString("Enable Engine Tilt?");
        window.draw(menuTitle);
        window.draw(tiltYesButton);
        window.draw(tiltYesLabel);
        window.draw(tiltNoButton);
        window.draw(tiltNoLabel);
    }
    else if (menuStep == 1) {
        menuTitle.setString("Select Wind Strength");
        window.draw(menuTitle);
        for (size_t i = 0; i < windButtons.size(); ++i) {
            window.draw(windButtons[i]);
            window.draw(windButtonLabels[i]);
        }
    }
    else if (menuStep == 2) {
        menuTitle.setString("Set Gravity");
        window.draw(menuTitle);
        window.draw(gravitySlider);
        window.draw(gravitySliderHandle);
        window.draw(gravityLabel);
        window.draw(startButton);
        window.draw(startButtonLabel);
    }

    window.display();
}

void MenuScreen::handleEvent(sf::Event& event) {
    if (!menuActive) return;

    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        if (menuStep == 0) {
            if (tiltYesButton.getGlobalBounds().contains(mousePos)) {
                engineTiltEnabled = true;
                tiltYesButton.setFillColor(sf::Color::Green); // Визуальная обратная связь
                tiltNoButton.setFillColor(sf::Color::White);
                menuStep = 1;
            }
            else if (tiltNoButton.getGlobalBounds().contains(mousePos)) {
                engineTiltEnabled = false;
                tiltNoButton.setFillColor(sf::Color::Red);
                tiltYesButton.setFillColor(sf::Color::White);
                menuStep = 1;
            }
        }
        else if (menuStep == 1) {
            for (size_t i = 0; i < windButtons.size(); ++i) {
                if (windButtons[i].getGlobalBounds().contains(mousePos)) {
                    switch (i) {
                        case 0: 
                            windEnabled = false; 
                            windForce = sf::Vector2f(0.f, 0.f); 
                            break;
                        case 1: 
                            windEnabled = true; 
                            windForce = sf::Vector2f(
                                static_cast<float>((std::rand()%10+1)*2), 
                                static_cast<float>(std::rand()%10+1)
                            ); 
                            break;
                        case 2: 
                            windEnabled = true; 
                            windForce = sf::Vector2f(
                                static_cast<float>((std::rand()%15+5)*2), 
                                static_cast<float>(std::rand()%15+5)
                            ); 
                            break;
                        case 3: 
                            windEnabled = true; 
                            windForce = sf::Vector2f(
                                static_cast<float>((std::rand()%20+10)*2), 
                                static_cast<float>(std::rand()%20+10)
                            ); 
                            break;
                    }
                    menuStep = 2;
                    break;
                }
            }
        }
        else if (menuStep == 2) {
            if (gravitySliderHandle.getGlobalBounds().contains(mousePos)) {
                isDraggingSlider = true;
            }
            else if (startButton.getGlobalBounds().contains(mousePos)) {
                menuActive = false;
            }
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased) {
        isDraggingSlider = false;
    }
    else if (event.type == sf::Event::MouseMoved && isDraggingSlider) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        float newX = std::max(gravitySlider.getPosition().x,
                            std::min(mousePos.x, 
                                    gravitySlider.getPosition().x + gravitySlider.getSize().x));
        gravitySliderHandle.setPosition(newX, gravitySliderHandle.getPosition().y);
        gravity = (newX - gravitySlider.getPosition().x) / gravitySlider.getSize().x * 400.0f;
        gravityLabel.setString("Gravity: " + std::to_string(static_cast<int>(gravity)));
    }
}

bool MenuScreen::isMenuActive() const {
    return menuActive;
}

void MenuScreen::setMenuActive(bool active) {
    menuActive = active;
}

bool MenuScreen::isEngineTiltEnabled() const {
    return engineTiltEnabled;
}

bool MenuScreen::isWindEnabled() const {
    return windEnabled;
}

sf::Vector2f MenuScreen::getWindForce() const {
    return windForce;
}

float MenuScreen::getGravity() const {
    return gravity;
}