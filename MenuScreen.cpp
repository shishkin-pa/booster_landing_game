#include "MenuScreen.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

MenuScreen::MenuScreen(sf::RenderWindow& window, sf::Font& font)
    : window(window), font(font), menuActive(true), menuStep(0),
      engineTiltEnabled(true), windEnabled(false), windForce(0.f, 0.f),
      gravity(200.0f), isDraggingSlider(false), showSplash(true) {
    
    // Загрузка текстуры заставки
    if (!splashTexture.loadFromFile("splash.png")) {
        std::cerr << "Failed to load splash.png" << std::endl;
        // Создаем черный экран, если заставка не загрузилась
        sf::Image img;
        img.create(1, 1, sf::Color::Black);
        splashTexture.loadFromImage(img);
    }
    
    splashScreen.setSize(sf::Vector2f(window.getSize()));
    splashScreen.setTexture(&splashTexture);
    
    // Загрузка текстур
    if (!tiltYesTexture.loadFromFile("tilt_yes.png")) {
        std::cerr << "Failed to load tilt_yes.png" << std::endl;
    }
    if (!tiltNoTexture.loadFromFile("tilt_no.png")) {
        std::cerr << "Failed to load tilt_no.png" << std::endl;
    }
    if (!windNoneTexture.loadFromFile("wind_none.png")) {
        std::cerr << "Failed to load wind_none.png" << std::endl;
    }
    if (!windWeakTexture.loadFromFile("wind_weak.png")) {
        std::cerr << "Failed to load wind_weak.png" << std::endl;
    }
    if (!windModerateTexture.loadFromFile("wind_moderate.png")) {
        std::cerr << "Failed to load wind_moderate.png" << std::endl;
    }
    if (!windStrongTexture.loadFromFile("wind_strong.png")) {
        std::cerr << "Failed to load wind_strong.png" << std::endl;
    }
    if (!startGameTexture.loadFromFile("start_game.png")) {
        std::cerr << "Failed to load start_game.png" << std::endl;
    }
    if (!gravityMinTexture.loadFromFile("gravity_min.png")) {
        std::cerr << "Failed to load gravity_min.png" << std::endl;
    }

    createMenu();
}

void MenuScreen::createMenu() {
    // Явно получаем текущие размеры окна
    const sf::Vector2u windowSize = window.getSize();
    const float centerX = windowSize.x / 2.0f;
    const float centerY = windowSize.y / 2.0f;

    // Кнопки наклона двигателя
    tiltYesButton.setSize(sf::Vector2f(312, 312));
    tiltYesButton.setPosition(centerX - 350, centerY - 156);
    tiltYesButton.setTexture(&tiltYesTexture);

    tiltNoButton.setSize(sf::Vector2f(312, 312));
    tiltNoButton.setPosition(centerX + 38, centerY - 156);
    tiltNoButton.setTexture(&tiltNoTexture);

    // Кнопки ветра
    windButtons.clear();
    const std::vector<sf::Texture*> windTextures = {
        &windNoneTexture, &windWeakTexture, &windModerateTexture, &windStrongTexture
    };
    
    const float totalWidth = 4 * 312 + 3 * 40;
    const float startX = centerX - totalWidth / 2;
    
    for (int i = 0; i < 4; ++i) {
        sf::RectangleShape button(sf::Vector2f(312, 312));
        button.setPosition(startX + i * (312 + 40), centerY - 156);
        button.setTexture(windTextures[i]);
        windButtons.push_back(button);
    }

    // Гравитация
    gravityMinButton.setSize(sf::Vector2f(312, 312));
    gravityMinButton.setPosition(centerX - 156, centerY - 364);
    gravityMinButton.setTexture(&gravityMinTexture);

    // Слайдер гравитации (трек)
    gravitySliderTrack.setSize(sf::Vector2f(624, 20));
    gravitySliderTrack.setPosition(centerX - 312, centerY);
    gravitySliderTrack.setFillColor(sf::Color(100, 100, 100));
    gravitySliderTrack.setOutlineThickness(5);  // Чёрный контур по умолчанию
    gravitySliderTrack.setOutlineColor(sf::Color::Black);

    // Слайдер гравитации (заполнение)
    gravitySlider.setSize(sf::Vector2f(624, 20));
    gravitySlider.setPosition(centerX - 312, centerY);
    gravitySlider.setFillColor(sf::Color::White);
    gravitySlider.setOutlineThickness(5);  // Чёрный контур
    gravitySlider.setOutlineColor(sf::Color::Black);

    // Ползунок слайдера
    gravitySliderHandle.setSize(sf::Vector2f(20, 40));
    gravitySliderHandle.setPosition(centerX - 312 + (gravity / 400.0f) * gravitySlider.getSize().x, centerY - 10);
    gravitySliderHandle.setFillColor(sf::Color::Green);
    gravitySliderHandle.setOutlineThickness(3);  // Контур тоньше для ползунка
    gravitySliderHandle.setOutlineColor(sf::Color::Black);

    // Стартовая кнопка
    startButton.setSize(sf::Vector2f(312, 312));
    startButton.setPosition(centerX - 156, centerY + 64);
    startButton.setTexture(&startGameTexture);

    // Сбрасываем состояние всех кнопок
    updateButtonHover();
}

void MenuScreen::draw() {
    if (!menuActive) return;

    window.clear(sf::Color(50, 50, 50));

    if (showSplash) {
        window.draw(splashScreen);
    }
    else {
        // Отрисовка обычного меню
        if (menuStep == 0) {
            window.draw(tiltYesButton);
            window.draw(tiltNoButton);
        }
        else if (menuStep == 1) {
            for (auto& button : windButtons) {
                window.draw(button);
            }
        }
        else if (menuStep == 2) {
            window.draw(gravityMinButton);
            window.draw(gravitySliderTrack);
            window.draw(gravitySlider);
            window.draw(gravitySliderHandle);
            window.draw(startButton);
        }
    }

    window.display();
}

void MenuScreen::handleEvent(sf::Event& event) {
    if (!menuActive) return;

    if (showSplash) {
        if (event.type == sf::Event::KeyPressed || 
            event.type == sf::Event::MouseButtonPressed) {
            showSplash = false;
        }
        return;
    }

    updateButtonHover();

    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        if (menuStep == 0) {
            if (tiltYesButton.getGlobalBounds().contains(mousePos)) {
                engineTiltEnabled = true;
                menuStep = 1;
            }
            else if (tiltNoButton.getGlobalBounds().contains(mousePos)) {
                engineTiltEnabled = false;
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
            // Обработка клика по ползунку или треку слайдера
            if (gravitySliderHandle.getGlobalBounds().contains(mousePos)) {
                isDraggingSlider = true;
            }
            else if (gravitySliderTrack.getGlobalBounds().contains(mousePos)) {
                // Новый функционал: клик по треку мгновенно перемещает ползунок
                float relativeX = mousePos.x - gravitySliderTrack.getPosition().x;
                float percent = std::max(0.f, std::min(1.f, relativeX / gravitySliderTrack.getSize().x));
                
                gravity = percent * 400.0f;
                gravitySliderHandle.setPosition(
                    gravitySliderTrack.getPosition().x + percent * gravitySliderTrack.getSize().x - 10, // -10 для центровки
                    gravitySliderHandle.getPosition().y
                );
                
                // Обновляем цвет
                gravitySliderHandle.setFillColor(
                    (gravity < 190 || gravity > 210) ? sf::Color::Red : sf::Color::Green
                );
                
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
        float newX = std::max(gravitySliderTrack.getPosition().x,
                            std::min(mousePos.x, 
                                    gravitySliderTrack.getPosition().x + gravitySliderTrack.getSize().x));
        gravitySliderHandle.setPosition(newX - 10, gravitySliderHandle.getPosition().y); // -10 для центровки
        gravity = (newX - gravitySliderTrack.getPosition().x) / gravitySliderTrack.getSize().x * 400.0f;
        
        // Обновляем цвет
        gravitySliderHandle.setFillColor(
            (gravity < 190 || gravity > 210) ? sf::Color::Red : sf::Color::Green
        );
    }
}

void MenuScreen::updateButtonHover() {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    const sf::Color hoverColor(50, 100, 150); // Тёмно-голубой цвет контура

    if (menuStep == 0) {
        tiltYesButton.setOutlineThickness(tiltYesButton.getGlobalBounds().contains(mousePos) ? 5 : 0);
        tiltYesButton.setOutlineColor(hoverColor); // Устанавливаем цвет
        
        tiltNoButton.setOutlineThickness(tiltNoButton.getGlobalBounds().contains(mousePos) ? 5 : 0);
        tiltNoButton.setOutlineColor(hoverColor);
    }
    else if (menuStep == 1) {
        for (auto& button : windButtons) {
            button.setOutlineThickness(button.getGlobalBounds().contains(mousePos) ? 5 : 0);
            button.setOutlineColor(hoverColor);
        }
    }
    else if (menuStep == 2) {
        startButton.setOutlineThickness(startButton.getGlobalBounds().contains(mousePos) ? 5 : 0);
        startButton.setOutlineColor(hoverColor);
        
        gravityMinButton.setOutlineThickness(gravityMinButton.getGlobalBounds().contains(mousePos) ? 5 : 0);
        gravityMinButton.setOutlineColor(hoverColor);
    }
}

bool MenuScreen::isMenuActive() const {
    return menuActive;
}

void MenuScreen::setMenuActive(bool active) {
    menuActive = active;
    if (active) {
        menuStep = 0;
    }
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

void MenuScreen::resetMenu() {
    // 1. Сбрасываем состояние
    menuActive = true;
    menuStep = 0;
    showSplash = true;
    isDraggingSlider = false;
    
    // 2. Пересоздаем элементы меню
    createMenu();
    
    // 3. Сбрасываем настройки к значениям по умолчанию
    engineTiltEnabled = true;
    windEnabled = false;
    windForce = sf::Vector2f(0.f, 0.f);
    gravity = 200.0f;
}