#include "Game.h"
#include <iostream>

Game::Game()
    : window(sf::VideoMode::getDesktopMode(), "Booster Landing Game", sf::Style::Fullscreen),
      booster(nullptr), platform(nullptr), menuScreen(window, font),
      marker(), windowSize(window.getSize()) // Инициализация windowSize
{
    if (!font.loadFromFile("Arial.ttf")) {
        std::cerr << "Failed to load font! Text will not be displayed." << std::endl;
    }
    window.setFramerateLimit(60);
    gameView = window.getDefaultView();
    
    // Инициализация groundVertices
    groundVertices.setPrimitiveType(sf::Quads);
    groundVertices.resize(4);
}

Game::~Game() {
    delete booster;
    delete platform;
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
            if (booster == nullptr || platform == nullptr) {
                initializeGame();
            }
            handleEvents();
            update();
            updateCamera();
            render();
        }
    }
}

void Game::updateCamera() {
    if (!booster) return;

    sf::Vector2f viewCenter = booster->getPosition();
    float groundBottom = ground.getPosition().y + ground.getSize().y;
    float viewHeight = gameView.getSize().y;

    // Если земля в зоне видимости, фиксируем её у нижнего края
    if (viewCenter.y + viewHeight/2 > groundBottom) {
        viewCenter.y = groundBottom - viewHeight/2;
    }

    gameView.setCenter(viewCenter);
    window.setView(gameView);
}

void Game::handleEvents() {
    if (!booster) return;

    // Тяга
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        booster->applyThrust(10.0f);
    } else {
        booster->applyThrust(0);
    }

    // Оригинальное управление:
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        if (menuScreen.isEngineTiltEnabled()) {
            booster->rotateEngines(-1.5f); // Наклон двигателей
        }
        booster->rotateGyroscope(-1.0f, clock.getElapsedTime().asSeconds());
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        if (menuScreen.isEngineTiltEnabled()) {
            booster->rotateEngines(1.5f);
        }
        booster->rotateGyroscope(1.0f, clock.getElapsedTime().asSeconds());
    }
}

void Game::update() {
    if (booster == nullptr || platform == nullptr) return;

    float deltaTime = clock.restart().asSeconds();
    booster->update(deltaTime);
    booster->checkLanding(platform->getBounds(), ground.getGlobalBounds());
    marker.update(*booster, *platform, window);
}

void Game::render() {
    window.clear();
    
    // Рендеринг земли
    sf::Vector2f viewCenter = gameView.getCenter();
    sf::Vector2f viewSize = gameView.getSize();
    float groundLevel = windowSize.y - 50; // Уровень земли
    
    groundVertices[0].position = sf::Vector2f(viewCenter.x - viewSize.x*2, groundLevel);
    groundVertices[1].position = sf::Vector2f(viewCenter.x + viewSize.x*2, groundLevel);
    groundVertices[2].position = sf::Vector2f(viewCenter.x + viewSize.x*2, groundLevel + 1000.f);
    groundVertices[3].position = sf::Vector2f(viewCenter.x - viewSize.x*2, groundLevel + 1000.f);
    
    float texScale = 0.1f;
    groundVertices[0].texCoords = sf::Vector2f(0, 0);
    groundVertices[1].texCoords = sf::Vector2f(viewSize.x*4*texScale, 0);
    groundVertices[2].texCoords = sf::Vector2f(viewSize.x*4*texScale, 1000.f*texScale);
    groundVertices[3].texCoords = sf::Vector2f(0, 1000.f*texScale);
    
    window.draw(groundVertices, &groundTexture);
    
    // Остальной рендеринг
    if (platform) platform->draw(window);
    if (booster) booster->draw(window);
    drawHUD();
    drawMarker();
    
    window.display();
}

void Game::drawHUD() {
    if (font.loadFromFile("Arial.ttf") && booster) {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);

        sf::Vector2f viewTopLeft = gameView.getCenter() - gameView.getSize()/2.f;

        sf::Vector2f velocity = booster->getVelocity();
        text.setString("Velocity: (" + std::to_string(velocity.x) + ", " + std::to_string(velocity.y) + ")");
        text.setPosition(viewTopLeft.x + 10, viewTopLeft.y + 10);
        window.draw(text);

        text.setString("Angle: " + std::to_string(booster->getAngle()));
        text.setPosition(viewTopLeft.x + 10, viewTopLeft.y + 40);
        window.draw(text);

        text.setString("Initial Angle: " + std::to_string(booster->getInitialAngle()));
        text.setPosition(viewTopLeft.x + 10, viewTopLeft.y + 70);
        window.draw(text);

        float heightAboveGround = ground.getPosition().y - (booster->getPosition().y + 30);
        text.setString("Height: " + std::to_string(heightAboveGround));
        text.setPosition(viewTopLeft.x + 10, viewTopLeft.y + 100);
        window.draw(text);

        if (menuScreen.isWindEnabled()) {
            sf::Vector2f wind = booster->getWind();
            text.setString("Wind: (" + std::to_string(wind.x) + ", " + std::to_string(wind.y) + ")");
            text.setPosition(viewTopLeft.x + 10, viewTopLeft.y + 130);
            window.draw(text);
        }
    }
}

void Game::drawMarker() {
    marker.draw(window);
}

void Game::initializeGame() {
    windowSize = window.getSize(); // Обновляем размер окна
    
    // Загрузка текстуры земли
    if (!groundTexture.loadFromFile("ground_texture.jpg")) {
        // Создаем простую зеленую текстуру если загрузка не удалась
        sf::Image img;
        img.create(32, 32, sf::Color::Green);
        groundTexture.loadFromImage(img);
    }

    // Физическая модель земли
    ground.setSize(sf::Vector2f(100000.f, 50.f));
    ground.setPosition(-50000.f, windowSize.y - 50);
    ground.setFillColor(sf::Color::Transparent);

    // Инициализация других объектов
    platform = new Platform(windowSize.x / 2 - 100, windowSize.y - 50);
    float initialY = -1000.0f;
    booster = new Booster(windowSize.x / 2, initialY);
    booster->setEngineTiltEnabled(menuScreen.isEngineTiltEnabled());
    booster->setGravity(menuScreen.getGravity());

    if (menuScreen.isWindEnabled()) {
        booster->setWind(menuScreen.getWindForce());
    }
}