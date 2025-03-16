#include "Game.h"
#include <iostream>
#include <limits>
#include <ctime>
#include <string>

Game::Game() 
    : booster(nullptr), platform(0, 0), windEnabled(false), gravity(200.0f), engineTiltEnabled(true) {
    // Загрузка шрифта
    if (!font.loadFromFile("Arial.ttf")) {
        std::cerr << "Failed to load font! Text will not be displayed." << std::endl;
    }

    // Ввод параметров
    initializeGame();
}

Game::~Game() {
    delete booster; // Очистка памяти, выделенной для бустера
}

void Game::initializeGame() {
    // Ввод параметров (ветер, наклон двигателей, гравитация)
    std::string input;
    bool validInput = false;

    // Включение/выключение ветра
    while (!validInput) {
        std::cout << "Enable wind? (y/n): ";
        std::getline(std::cin, input);
        if (input.length() == 1 && (input[0] == 'y' || input[0] == 'Y' || input[0] == 'n' || input[0] == 'N')) {
            validInput = true;
            windEnabled = (input[0] == 'y' || input[0] == 'Y');
        } else {
            std::cout << "Invalid input. Please enter 'y' or 'n'." << std::endl;
        }
    }

    // Включение/выключение наклона двигателей
    validInput = false;
    while (!validInput) {
        std::cout << "Enable engine tilt? (y/n): ";
        std::getline(std::cin, input);
        if (input.length() == 1 && (input[0] == 'y' || input[0] == 'Y' || input[0] == 'n' || input[0] == 'N')) {
            validInput = true;
            engineTiltEnabled = (input[0] == 'y' || input[0] == 'Y');
        } else {
            std::cout << "Invalid input. Please enter 'y' or 'n'." << std::endl;
        }
    }

    // Установка гравитации
    validInput = false;
    while (!validInput) {
        std::cout << "Set gravity (recommended: 200): ";
        std::getline(std::cin, input);
        try {
            gravity = std::stof(input);
            if (gravity > 0) {
                validInput = true;
            } else {
                std::cout << "Invalid input. Please enter a positive number." << std::endl;
            }
        } catch (const std::invalid_argument&) {
            std::cout << "Invalid input. Please enter a positive number." << std::endl;
        }
    }

    // Создание окна игры
    window.create(sf::VideoMode::getDesktopMode(), "Booster Landing Game", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    // Инициализация платформы и земли
    platform.setPosition(window.getSize().x / 2 - 100, window.getSize().y - 50);
    ground.setSize(sf::Vector2f(window.getSize().x, 50));
    ground.setFillColor(sf::Color::Green);
    ground.setPosition(0, window.getSize().y - 50);

    // Инициализация бустера
    float initialY = -1000.0f; // Фиксированная начальная высота
    booster = new Booster(window.getSize().x / 2, initialY);
    booster->setEngineTiltEnabled(engineTiltEnabled);
    booster->setGravity(gravity);

    // Установка случайной силы ветра
    if (windEnabled) {
        float windX = (std::rand() % 1000 - 500) / 10.0f; // Случайная сила ветра по X (-5 до 5)
        float windY = (std::rand() % 1000 - 500) / 10.0f; // Случайная сила ветра по Y (-5 до 5)
        booster->setWind(sf::Vector2f(windX, windY));
    }
}

void Game::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
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
    booster->checkLanding(platform.getBounds(), ground.getGlobalBounds());
}

void Game::render() {
    window.clear();
    window.draw(ground); // Отрисовка земли
    booster->draw(window);
    platform.draw(window);
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