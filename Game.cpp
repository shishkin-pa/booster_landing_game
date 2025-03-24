#include "Game.h"
#include <iostream>
#include <cmath>

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
                menuScreen.handleEvent(event);
            }
        }

        if (menuScreen.isMenuActive()) {
            window.clear();
            menuScreen.draw();
            window.display();
        } else {
            // Инициализируем игру, если бустер и платформа ещё не созданы
            if (booster == nullptr || platform == nullptr) {
                initializeGame();
            }
            handleEvents();
            update();
            render();
        }
    }
}

void Game::handleEvents() {
    if (booster == nullptr || platform == nullptr) return;

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
    if (booster == nullptr || platform == nullptr) return;

    float deltaTime = clock.restart().asSeconds();
    booster->update(deltaTime);
    booster->checkLanding(platform->getBounds(), ground.getGlobalBounds());
}

void Game::render() {
    if (booster == nullptr || platform == nullptr) return;

    window.clear();
    window.draw(ground);
    booster->draw(window);
    platform->draw(window);
    drawHUD();
    drawMarker();
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
        // Рассчитываем минимальное расстояние до края экрана
        float distanceToEdge = std::min(
            std::min(position.x < 0 ? -position.x : windowSize.x - position.x,
                    position.y < 0 ? -position.y : windowSize.y - position.y),
            static_cast<float>(std::min(windowSize.x, windowSize.y))
        );

        // Параметры масштабирования
        const float maxEffectDistance = 2250.0f;  // Расстояние влияния на размер
        const float minScale = 0.3f;             // Минимальный размер (при максимальном удалении)
        const float maxScale = 1.5f;             // Максимальный размер (у края экрана)
        
        // Плавное изменение размера - уменьшается при отдалении
        float scale = maxScale - (maxScale - minScale) * 
                     std::min(distanceToEdge / maxEffectDistance, 1.0f);

        // Цвет контура (темно-бордовый)
        sf::Color outlineColor(100, 0, 0);

        if (menuScreen.getMarkerType() == MarkerType::CIRCLE) {
            // Circle marker
            sf::CircleShape marker(8 * scale);  // Уменьшен базовый размер
            marker.setFillColor(sf::Color::Red);
            marker.setOutlineThickness(1.5f * scale);
            marker.setOutlineColor(outlineColor);
            
            float markerX = position.x;
            float markerY = position.y;
            if (position.x < 0) markerX = 10 * scale;
            if (position.x > windowSize.x) markerX = windowSize.x - 10 * scale;
            if (position.y < 0) markerY = 10 * scale;
            if (position.y > windowSize.y) markerY = windowSize.y - 10 * scale;
            
            marker.setPosition(markerX, markerY);
            window.draw(marker);
        } else {
            // Triangle marker с дополнительным смещением вниз
            sf::ConvexShape marker;
            marker.setPointCount(3);
            float baseWidth = 18 * scale;    // Уменьшенная ширина основания
            float sideLength = 25 * scale;   // Уменьшенная длина сторон
            float angle = booster->getAngle();
            
            // Позиция маркера с дополнительным смещением вниз
            float markerX = position.x;
            float markerY = position.y;
            float verticalOffset = 20 * scale;  // Смещение вниз

            if (position.x < 0) markerX = 15 * scale;
            if (position.x > windowSize.x) markerX = windowSize.x - 15 * scale;
            if (position.y < 0) markerY = verticalOffset;
            if (position.y > windowSize.y) markerY = windowSize.y - verticalOffset + 8 * scale;

            marker.setPosition(markerX, markerY);
            marker.setPoint(0, sf::Vector2f(0, -sideLength));       // Вершина
            marker.setPoint(1, sf::Vector2f(-baseWidth/2, 4 * scale)); // Левое основание
            marker.setPoint(2, sf::Vector2f(baseWidth/2, 4 * scale));  // Правое основание
            marker.setRotation(angle);
            marker.setFillColor(sf::Color::Red);
            marker.setOutlineThickness(1.5f * scale);
            marker.setOutlineColor(outlineColor);
            window.draw(marker);
        }
    }
}

void Game::initializeGame() {
    if (booster != nullptr) delete booster;
    if (platform != nullptr) delete platform;

    platform = new Platform(window.getSize().x / 2 - 100, window.getSize().y - 50);
    ground.setSize(sf::Vector2f(window.getSize().x, 50));
    ground.setFillColor(sf::Color::Green);
    ground.setPosition(0, window.getSize().y - 50);

    float initialY = -1000.0f;
    booster = new Booster(window.getSize().x / 2, initialY);
    booster->setEngineTiltEnabled(menuScreen.isEngineTiltEnabled());
    booster->setGravity(menuScreen.getGravity());

    if (menuScreen.isWindEnabled()) {
        booster->setWind(menuScreen.getWindForce());
    }
}