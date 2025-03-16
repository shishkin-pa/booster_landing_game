#include "Game.h"
#include <iostream>
#include <limits>
#include <ctime>
#include <string>

Game::Game() 
    : booster(nullptr), platform(nullptr), windEnabled(false), gravity(200.0f), engineTiltEnabled(true), menuActive(true), menuStep(0), isDraggingSlider(false) {
    // Загрузка шрифта
    if (!font.loadFromFile("Arial.ttf")) {
        std::cerr << "Failed to load font! Text will not be displayed." << std::endl;
    }

    // Создание окна игры
    window.create(sf::VideoMode::getDesktopMode(), "Booster Landing Game", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    // Инициализация меню
    createMenu();
}

Game::~Game() {
    delete booster; // Очистка памяти, выделенной для бустера
    delete platform; // Очистка памяти, выделенной для платформы
}

void Game::createMenu() {
    // Получаем размеры окна
    sf::Vector2u windowSize = window.getSize();
    float centerX = windowSize.x / 2.0f;
    float centerY = windowSize.y / 2.0f;

    // Заголовок меню
    menuTitle.setFont(font);
    menuTitle.setCharacterSize(40);
    menuTitle.setFillColor(sf::Color::White);
    menuTitle.setPosition(centerX - 150, centerY - 200); // Центрируем заголовок

    // Кнопки для выбора наклона двигателей
    tiltYesButton.setSize(sf::Vector2f(200, 50));
    tiltYesButton.setPosition(centerX - 250, centerY - 100); // Центрируем кнопки
    tiltYesButton.setFillColor(sf::Color::White);

    tiltNoButton.setSize(sf::Vector2f(200, 50));
    tiltNoButton.setPosition(centerX + 50, centerY - 100); // Центрируем кнопки
    tiltNoButton.setFillColor(sf::Color::White);

    tiltYesLabel.setFont(font);
    tiltYesLabel.setString("Yes");
    tiltYesLabel.setCharacterSize(24);
    tiltYesLabel.setFillColor(sf::Color::Black);
    tiltYesLabel.setPosition(centerX - 240, centerY - 90); // Центрируем текст

    tiltNoLabel.setFont(font);
    tiltNoLabel.setString("No");
    tiltNoLabel.setCharacterSize(24);
    tiltNoLabel.setFillColor(sf::Color::Black);
    tiltNoLabel.setPosition(centerX + 60, centerY - 90); // Центрируем текст

    // Кнопки для выбора силы ветра
    std::vector<std::string> windOptions = {"No Wind", "Weak Wind", "Moderate Wind", "Strong Wind"};
    for (int i = 0; i < windOptions.size(); ++i) {
        sf::RectangleShape button(sf::Vector2f(200, 50));
        button.setPosition(centerX - 100, centerY - 50 + i * 60); // Центрируем кнопки
        button.setFillColor(sf::Color::White);
        windButtons.push_back(button);

        sf::Text label;
        label.setFont(font);
        label.setString(windOptions[i]);
        label.setCharacterSize(24);
        label.setFillColor(sf::Color::Black);
        label.setPosition(centerX - 90, centerY - 40 + i * 60); // Центрируем текст
        windButtonLabels.push_back(label);
    }

    // Слайдер для гравитации
    gravitySlider.setSize(sf::Vector2f(300, 20));
    gravitySlider.setPosition(centerX - 150, centerY + 150); // Центрируем слайдер
    gravitySlider.setFillColor(sf::Color::White);

    // Начальная позиция ползунка слайдера, соответствующая значению гравитации (200)
    float initialSliderX = centerX - 150 + (gravity / 400.0f) * gravitySlider.getSize().x;
    gravitySliderHandle.setSize(sf::Vector2f(20, 40));
    gravitySliderHandle.setPosition(initialSliderX, centerY + 140); // Центрируем ползунок
    gravitySliderHandle.setFillColor(sf::Color::Red);

    gravityLabel.setFont(font);
    gravityLabel.setString("Gravity: " + std::to_string(static_cast<int>(gravity)));
    gravityLabel.setCharacterSize(24);
    gravityLabel.setFillColor(sf::Color::White);
    gravityLabel.setPosition(centerX - 150, centerY + 180); // Центрируем текст

    // Кнопка для запуска игры
    startButton.setSize(sf::Vector2f(200, 50));
    startButton.setPosition(centerX - 100, centerY + 250); // Центрируем кнопку
    startButton.setFillColor(sf::Color::Green);

    startButtonLabel.setFont(font);
    startButtonLabel.setString("Start Game");
    startButtonLabel.setCharacterSize(24);
    startButtonLabel.setFillColor(sf::Color::Black);
    startButtonLabel.setPosition(centerX - 90, centerY + 260); // Центрируем текст
}

void Game::handleMenuEvents(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        if (menuStep == 0) { // Шаг 1: Выбор наклона двигателей
            if (tiltYesButton.getGlobalBounds().contains(mousePos)) {
                engineTiltEnabled = true;
                menuStep = 1; // Переход к следующему шагу
            }
            if (tiltNoButton.getGlobalBounds().contains(mousePos)) {
                engineTiltEnabled = false;
                menuStep = 1; // Переход к следующему шагу
            }
        } else if (menuStep == 1) { // Шаг 2: Выбор силы ветра
            for (int i = 0; i < windButtons.size(); ++i) {
                if (windButtons[i].getGlobalBounds().contains(mousePos)) {
                    switch (i) {
                        case 0: // No Wind
                            windEnabled = false;
                            windForce = sf::Vector2f(0, 0); // Ветер отключен
                            break;
                        case 1: // Weak Wind
                            windEnabled = true;
                            {
                                float windY = (std::rand() % 10 + 1); // Вертикальный ветер от 1 до 10
                                float windX = windY * 2; // Горизонтальный ветер в 2 раза больше
                                windForce = sf::Vector2f(windX, windY); // Сохраняем силу ветра
                            }
                            break;
                        case 2: // Moderate Wind
                            windEnabled = true;
                            {
                                float windY = (std::rand() % 15 + 5); // Вертикальный ветер от 5 до 20
                                float windX = windY * 2; // Горизонтальный ветер в 2 раза больше
                                windForce = sf::Vector2f(windX, windY); // Сохраняем силу ветра
                            }
                            break;
                        case 3: // Strong Wind
                            windEnabled = true;
                            {
                                float windY = (std::rand() % 20 + 10); // Вертикальный ветер от 10 до 30
                                float windX = windY * 2; // Горизонтальный ветер в 2 раза больше
                                windForce = sf::Vector2f(windX, windY); // Сохраняем силу ветра
                            }
                            break;
                    }
                    menuStep = 2; // Переход к следующему шагу
                }
            }
        } else if (menuStep == 2) { // Шаг 3: Выбор гравитации
            if (gravitySliderHandle.getGlobalBounds().contains(mousePos)) {
                isDraggingSlider = true; // Начинаем перетаскивание ползунка
            }
            if (startButton.getGlobalBounds().contains(mousePos)) {
                initializeGame(); // Инициализация игровых объектов
                menuActive = false; // Завершение меню и запуск игры
            }
        }
    } else if (event.type == sf::Event::MouseButtonReleased) {
        if (isDraggingSlider) {
            isDraggingSlider = false; // Завершаем перетаскивание ползунка
        }
    } else if (event.type == sf::Event::MouseMoved) {
        if (isDraggingSlider) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            // Перемещение ползунка слайдера
            float newX = mousePos.x;
            if (newX < gravitySlider.getPosition().x) newX = gravitySlider.getPosition().x; // Ограничение слева
            if (newX > gravitySlider.getPosition().x + gravitySlider.getSize().x) newX = gravitySlider.getPosition().x + gravitySlider.getSize().x; // Ограничение справа
            gravitySliderHandle.setPosition(newX, gravitySliderHandle.getPosition().y);

            // Обновление значения гравитации
            gravity = (newX - gravitySlider.getPosition().x) / gravitySlider.getSize().x * 400.0f; // Масштабирование от 0 до 400
            gravityLabel.setString("Gravity: " + std::to_string(static_cast<int>(gravity)));
        }
    }
}

void Game::run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (menuActive) {
                handleMenuEvents(event);
            }
        }

        if (menuActive) {
            window.clear();
            if (menuStep == 0) { // Шаг 1: Выбор наклона двигателей
                menuTitle.setString("Enable Engine Tilt?");
                window.draw(menuTitle);
                window.draw(tiltYesButton);
                window.draw(tiltYesLabel);
                window.draw(tiltNoButton);
                window.draw(tiltNoLabel);
            } else if (menuStep == 1) { // Шаг 2: Выбор силы ветра
                menuTitle.setString("Select Wind Strength");
                window.draw(menuTitle);
                for (int i = 0; i < windButtons.size(); ++i) {
                    window.draw(windButtons[i]);
                    window.draw(windButtonLabels[i]);
                }
            } else if (menuStep == 2) { // Шаг 3: Выбор гравитации
                menuTitle.setString("Set Gravity");
                window.draw(menuTitle);
                window.draw(gravitySlider);
                window.draw(gravitySliderHandle);
                window.draw(gravityLabel);
                window.draw(startButton);
                window.draw(startButtonLabel);
            }
            window.display();
        } else {
            handleEvents();
            update();
            render();
        }
    }
}

void Game::initializeGame() {
    // Инициализация платформы и земли
    platform = new Platform(window.getSize().x / 2 - 100, window.getSize().y - 50);
    ground.setSize(sf::Vector2f(window.getSize().x, 50));
    ground.setFillColor(sf::Color::Green);
    ground.setPosition(0, window.getSize().y - 50);

    // Инициализация бустера
    float initialY = -1000.0f; // Фиксированная начальная высота
    booster = new Booster(window.getSize().x / 2, initialY);
    booster->setEngineTiltEnabled(engineTiltEnabled);
    booster->setGravity(gravity);

    // Установка силы ветра (если ветер включен)
    if (windEnabled) {
        booster->setWind(windForce); // Применяем сохранённую силу ветра
    }
}

void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }

    // Управление тягой
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        booster->applyThrust(10.0f); // Тяга уменьшена до 10
    } else {
        booster->applyThrust(0); // Отключение тяги
    }

    // Управление гироскопом (стрелки влево/вправо)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        booster->rotateGyroscope(-1.0f, clock.getElapsedTime().asSeconds());
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        booster->rotateGyroscope(1.0f, clock.getElapsedTime().asSeconds());
    }

    // Управление наклоном двигателей (клавиши A и D)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        booster->rotateEngines(-1.0f); // Наклон влево
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        booster->rotateEngines(1.0f); // Наклон вправо
    }
}

void Game::update() {
    float deltaTime = clock.restart().asSeconds();
    booster->update(deltaTime);
    booster->checkLanding(platform->getBounds(), ground.getGlobalBounds());
}

void Game::render() {
    window.clear();
    window.draw(ground); // Отрисовка земли
    booster->draw(window);
    platform->draw(window);
    drawHUD(); // Отрисовка HUD
    drawMarker(); // Отрисовка маркера
    window.display();
}

void Game::drawHUD() {
    if (font.loadFromFile("Arial.ttf")) {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);

        // Скорость
        sf::Vector2f velocity = booster->getVelocity();
        text.setString("Velocity: (" + std::to_string(velocity.x) + ", " + std::to_string(velocity.y) + ")");
        text.setPosition(10, 10);
        window.draw(text);

        // Угол
        text.setString("Angle: " + std::to_string(booster->getAngle()));
        text.setPosition(10, 40);
        window.draw(text);

        // Начальный угол
        text.setString("Initial Angle: " + std::to_string(booster->getInitialAngle()));
        text.setPosition(10, 70);
        window.draw(text);

        // Высота над землёй
        float heightAboveGround = ground.getPosition().y - (booster->getPosition().y + 30);
        text.setString("Height: " + std::to_string(heightAboveGround));
        text.setPosition(10, 100);
        window.draw(text);

        // Ветер
        if (windEnabled) {
            sf::Vector2f wind = booster->getWind();
            text.setString("Wind: (" + std::to_string(wind.x) + ", " + std::to_string(wind.y) + ")");
            text.setPosition(10, 130);
            window.draw(text);
        }
    }
}

void Game::drawMarker() {
    sf::Vector2f position = booster->getPosition();
    sf::Vector2u windowSize = window.getSize();

    if (position.x < 0 || position.x > windowSize.x || position.y < 0 || position.y > windowSize.y) {
        sf::CircleShape marker(10);
        marker.setFillColor(sf::Color::Red);

        // Позиция маркера
        float markerX = position.x;
        float markerY = position.y;

        if (position.x < 0) markerX = 10;
        if (position.x > windowSize.x) markerX = windowSize.x - 10;
        if (position.y < 0) markerY = 10;
        if (position.y > windowSize.y) markerY = windowSize.y - 10;

        marker.setPosition(markerX, markerY);
        window.draw(marker);
    }
}