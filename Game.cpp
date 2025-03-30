#include "Game.h"
#include <iostream>

Game::Game()
    : window(sf::VideoMode::getDesktopMode(), "Booster Landing Game", sf::Style::Fullscreen),
      booster(nullptr), platform(nullptr), menuScreen(window, font),
      marker(MarkerType::CIRCLE) {
    
    if (!font.loadFromFile("Arial.ttf")) {
        std::cerr << "Failed to load font! Text will not be displayed." << std::endl;
    }
    window.setFramerateLimit(60);
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
                marker.setType(menuScreen.getMarkerType());
            }
            handleEvents();
            update();
            render();
        }
    }
}

void Game::handleEvents() {
    if (booster == nullptr) return;

    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        booster->applyThrust(10.0f);
    } else {
        booster->applyThrust(0);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        booster->rotateGyroscope(-1.0f, clock.getElapsedTime().asSeconds());
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        booster->rotateGyroscope(1.0f, clock.getElapsedTime().asSeconds());
    }
}

void Game::update() {
    if (booster == nullptr || platform == nullptr) return;

    float deltaTime = clock.restart().asSeconds();
    booster->update(deltaTime);
    booster->checkLanding(platform->getBounds(), ground.getGlobalBounds());
}

void Game::render() {
    window.clear();
    window.draw(ground);
    if (booster) booster->draw(window);
    if (platform) platform->draw(window);
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

        sf::Vector2f velocity = booster->getVelocity();
        text.setString("Velocity: (" + std::to_string(velocity.x) + ", " + std::to_string(velocity.y) + ")");
        text.setPosition(10, 10);
        window.draw(text);

        text.setString("Angle: " + std::to_string(booster->getAngle()));
        text.setPosition(10, 40);
        window.draw(text);

        text.setString("Initial Angle: " + std::to_string(booster->getInitialAngle()));
        text.setPosition(10, 70);
        window.draw(text);

        float heightAboveGround = ground.getPosition().y - (booster->getPosition().y + 30);
        text.setString("Height: " + std::to_string(heightAboveGround));
        text.setPosition(10, 100);
        window.draw(text);

        if (menuScreen.isWindEnabled()) {
            sf::Vector2f wind = booster->getWind();
            text.setString("Wind: (" + std::to_string(wind.x) + ", " + std::to_string(wind.y) + ")");
            text.setPosition(10, 130);
            window.draw(text);
        }
    }
}

void Game::drawMarker() {
    if (booster) {
        marker.update(*booster, window);
        marker.draw(window);
    }
}

void Game::initializeGame() {
    if (platform) delete platform;
    if (booster) delete booster;

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