#include "Game.h"
#include <iostream>

Game::Game()
    : window(sf::VideoMode::getDesktopMode(), "Booster Landing Game", sf::Style::Fullscreen),
      booster(nullptr), platform(nullptr), menuScreen(window, font) {
    // Загрузка шрифта
    if (!font.loadFromFile("Arial.ttf")) {
        std::cerr << "Failed to load font! Text will not be displayed." << std::endl;
    }

    // Настройка окна
    window.setFramerateLimit(60);
}

Game::~Game() {
    delete booster; // Очистка памяти, выделенной для бустера
    delete platform; // Очистка памяти, выделенной для платформы
}

void Game::run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (menuScreen.isMenuActive()) {
                menuScreen.handleEvent(event); // Обработка событий меню
            }
        }

        if (menuScreen.isMenuActive()) {
            window.clear();
            menuScreen.draw(); // Отрисовка меню
            window.display();
        } else {
            handleEvents(); // Обработка событий игры
            update(); // Обновление игры
            render(); // Отрисовка игры
        }
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
    booster->update(deltaTime); // Обновление состояния бустера
    booster->checkLanding(platform->getBounds(), ground.getGlobalBounds()); // Проверка посадки
}

void Game::render() {
    window.clear();
    window.draw(ground); // Отрисовка земли
    booster->draw(window); // Отрисовка бустера
    platform->draw(window); // Отрисовка платформы
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
        if (menuScreen.isWindEnabled()) {
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

void Game::initializeGame() {
    // Инициализация платформы и земли
    platform = new Platform(window.getSize().x / 2 - 100, window.getSize().y - 50);
    ground.setSize(sf::Vector2f(window.getSize().x, 50));
    ground.setFillColor(sf::Color::Green);
    ground.setPosition(0, window.getSize().y - 50);

    // Инициализация бустера
    float initialY = -1000.0f; // Фиксированная начальная высота
    booster = new Booster(window.getSize().x / 2, initialY);
    booster->setEngineTiltEnabled(menuScreen.isEngineTiltEnabled());
    booster->setGravity(menuScreen.getGravity());

    // Установка силы ветра (если ветер включен)
    if (menuScreen.isWindEnabled()) {
        booster->setWind(menuScreen.getWindForce());
    }
}