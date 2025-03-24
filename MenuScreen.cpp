#include "MenuScreen.h"
#include <iostream>

MenuScreen::MenuScreen(sf::RenderWindow& window, sf::Font& font)
    : window(window), font(font), menuActive(true), menuStep(0), engineTiltEnabled(true),
      windEnabled(false), windForce(0, 0), gravity(200.0f), isDraggingSlider(false),
      markerType(MarkerType::CIRCLE) {
    createMenu();
}

void MenuScreen::createMenu() {
    sf::Vector2u windowSize = window.getSize();
    float centerX = windowSize.x / 2.0f;
    float centerY = windowSize.y / 2.0f;

    // Marker type selection
    menuTitle.setFont(font);
    menuTitle.setCharacterSize(40);
    menuTitle.setFillColor(sf::Color::White);
    menuTitle.setPosition(centerX - 150, centerY - 200);

    std::vector<std::string> markerOptions = {"Circle", "Triangle"};
    for (int i = 0; i < markerOptions.size(); ++i) {
        sf::RectangleShape button(sf::Vector2f(200, 50));
        button.setPosition(centerX - 250 + i * 300, centerY - 100); // Горизонтальное расположение
        button.setFillColor(sf::Color::White);
        button.setOutlineThickness(2);
        button.setOutlineColor(sf::Color::Black);
        markerButtons.push_back(button);

        sf::Text label;
        label.setFont(font);
        label.setString(markerOptions[i]);
        label.setCharacterSize(24);
        label.setFillColor(sf::Color::Black);
        label.setPosition(centerX - 240 + i * 300, centerY - 90);
        markerButtonLabels.push_back(label);
    }

    // Engine tilt selection
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

    // Wind selection
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

    // Gravity slider
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

    // Start button
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
    if (menuActive) {
        if (menuStep == 0) {
            menuTitle.setString("Select Marker Type");
            window.draw(menuTitle);
            for (int i = 0; i < markerButtons.size(); ++i) {
                window.draw(markerButtons[i]);
                window.draw(markerButtonLabels[i]);
            }
        } else if (menuStep == 1) {
            menuTitle.setString("Enable Engine Tilt?");
            window.draw(menuTitle);
            window.draw(tiltYesButton);
            window.draw(tiltYesLabel);
            window.draw(tiltNoButton);
            window.draw(tiltNoLabel);
        } else if (menuStep == 2) {
            menuTitle.setString("Select Wind Strength");
            window.draw(menuTitle);
            for (int i = 0; i < windButtons.size(); ++i) {
                window.draw(windButtons[i]);
                window.draw(windButtonLabels[i]);
            }
        } else if (menuStep == 3) {
            menuTitle.setString("Set Gravity");
            window.draw(menuTitle);
            window.draw(gravitySlider);
            window.draw(gravitySliderHandle);
            window.draw(gravityLabel);
            window.draw(startButton);
            window.draw(startButtonLabel);
        }
    }
}

void MenuScreen::handleEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        if (menuStep == 0) {
            for (int i = 0; i < markerButtons.size(); ++i) {
                if (markerButtons[i].getGlobalBounds().contains(mousePos)) {
                    markerType = static_cast<MarkerType>(i);
                    menuStep = 1;
                }
            }
        } else if (menuStep == 1) {
            if (tiltYesButton.getGlobalBounds().contains(mousePos)) {
                engineTiltEnabled = true;
                menuStep = 2;
            }
            if (tiltNoButton.getGlobalBounds().contains(mousePos)) {
                engineTiltEnabled = false;
                menuStep = 2;
            }
        } else if (menuStep == 2) {
            for (int i = 0; i < windButtons.size(); ++i) {
                if (windButtons[i].getGlobalBounds().contains(mousePos)) {
                    switch (i) {
                        case 0: windEnabled = false; windForce = sf::Vector2f(0, 0); break;
                        case 1: windEnabled = true; windForce = sf::Vector2f((std::rand() % 10 + 1) * 2, std::rand() % 10 + 1); break;
                        case 2: windEnabled = true; windForce = sf::Vector2f((std::rand() % 15 + 5) * 2, std::rand() % 15 + 5); break;
                        case 3: windEnabled = true; windForce = sf::Vector2f((std::rand() % 20 + 10) * 2, std::rand() % 20 + 10); break;
                    }
                    menuStep = 3;
                }
            }
        } else if (menuStep == 3) {
            if (gravitySliderHandle.getGlobalBounds().contains(mousePos)) {
                isDraggingSlider = true;
            }
            if (startButton.getGlobalBounds().contains(mousePos)) {
                menuActive = false;
            }
        }
    } else if (event.type == sf::Event::MouseButtonReleased) {
        isDraggingSlider = false;
    } else if (event.type == sf::Event::MouseMoved) {
        if (isDraggingSlider) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            float newX = std::max(gravitySlider.getPosition().x, std::min(mousePos.x, gravitySlider.getPosition().x + gravitySlider.getSize().x));
            gravitySliderHandle.setPosition(newX, gravitySliderHandle.getPosition().y);
            gravity = (newX - gravitySlider.getPosition().x) / gravitySlider.getSize().x * 400.0f;
            gravityLabel.setString("Gravity: " + std::to_string(static_cast<int>(gravity)));
        }
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

MarkerType MenuScreen::getMarkerType() const {
    return markerType;
}