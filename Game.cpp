#include "Game.h"
#include <iostream>
#include <random>
#include <cmath>

Game::Game()
    : window(sf::VideoMode::getDesktopMode(), "Booster Landing Game", sf::Style::Fullscreen),
      booster(nullptr), platform(nullptr), ground(nullptr), menuScreen(window, font),
      windowSize(window.getSize())
{
    if (!font.loadFromFile("Arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
    }

    // Загрузка статичных фоновых текстур
    if (!skyTexture.loadFromFile("sky.png")) {
        std::cerr << "Failed to load sky texture!" << std::endl;
        sf::Image img;
        img.create(6000, 5000, sf::Color(100, 150, 255));
        skyTexture.loadFromImage(img);
    }
    skySprite.setTexture(skyTexture);
    skySprite.setPosition(-3000, -4000); // Центрируем фон

    if (!starsTexture.loadFromFile("stars.png")) {
        std::cerr << "Failed to load stars texture!" << std::endl;
        sf::Image img;
        img.create(6000, 5000, sf::Color(10, 10, 30));
        starsTexture.loadFromImage(img);
    }
    starsSprite.setTexture(starsTexture);
    starsSprite.setPosition(-3000, -4000); // Центрируем фон

    // Загрузка остальных текстур
    if (!boosterTexture.loadFromFile("booster.png") ||
        !engineTexture.loadFromFile("engine.png") ||
        !flameTexture.loadFromFile("flame.png") ||
        !platformTexture.loadFromFile("platform.png") ||
        !groundTexture.loadFromFile("ground.png")) {
        std::cerr << "Failed to load object textures!" << std::endl;
    }

    window.setFramerateLimit(60);
    gameView = window.getDefaultView();
    
    initExitButton();
    initRestartButton();
    initMessageBackground();
}

Game::~Game() {
    delete booster;
    delete platform;
    delete ground;
}

void Game::initExitButton() {
    exitButton.setSize(sf::Vector2f(200, 50));
    exitButton.setFillColor(sf::Color::White);
    exitButton.setOutlineThickness(1);
    exitButton.setOutlineColor(sf::Color::Black);
    
    exitButtonText.setFont(font);
    exitButtonText.setString("Exit Game");
    exitButtonText.setCharacterSize(24);
    exitButtonText.setFillColor(sf::Color::Black);
    
    // Центрирование текста
    sf::FloatRect textRect = exitButtonText.getLocalBounds();
    exitButtonText.setOrigin(textRect.left + textRect.width/2.0f,
                           textRect.top + textRect.height/2.0f);
}

void Game::run() {
    while (window.isOpen()) {
        handleEvents();
        
        if (menuScreen.isMenuActive()) {
            window.clear();
            menuScreen.draw();
            window.display();
        } else {
            if (booster == nullptr || platform == nullptr) {
                initializeGame();
            }
            update();
            render();
        }
    }
}

void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (showExitButtonVisible) {
            handleExitButtonEvent(event);
            handleRestartButtonEvent(event);
        }
        else if (menuScreen.isMenuActive()) {
            menuScreen.handleEvent(event);
        }
    }

    if (booster && !menuScreen.isMenuActive() && !showExitButtonVisible) { // Используем флаг
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            booster->applyThrust(10.0f);
        } else {
            booster->applyThrust(0);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            if (menuScreen.isEngineTiltEnabled()) {
                booster->rotateEngines(-1.5f);
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
}

void Game::handleExitButtonEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos = window.mapPixelToCoords(
            sf::Mouse::getPosition(window));
        
        if (exitButton.getGlobalBounds().contains(mousePos)) {
            window.close();
        }
    }
}

void Game::update() {
    if (booster == nullptr || platform == nullptr || ground == nullptr) return;

    float deltaTime = clock.restart().asSeconds();
    booster->update(deltaTime);
    booster->checkLanding(platform->getBounds(), ground->getBounds());
    marker.update(*booster, *platform, window);
    updateCamera();
}

void Game::updateCamera() {
    if (!booster || !ground) return;

    sf::Vector2f viewCenter = booster->getPosition();
    float viewHeight = gameView.getSize().y;
    float groundBottom = ground->getBounds().top + ground->getBounds().height;

    if (viewCenter.y + viewHeight/2 > groundBottom) {
        viewCenter.y = groundBottom - viewHeight/2;
    }

    gameView.setCenter(viewCenter);
    window.setView(gameView);
}

void Game::render() {
    window.clear();

    // Рассчитываем прозрачности
    float heightRatio = (-booster->getPosition().y) / 5000.0f;
    heightRatio = std::max(0.0f, std::min(1.0f, heightRatio));

    // Устанавливаем прозрачности
    sf::Color skyColor(255, 255, 255, static_cast<sf::Uint8>(255 * (1.0f - heightRatio)));
    sf::Color starsColor(255, 255, 255, static_cast<sf::Uint8>(255 * heightRatio));
    
    skySprite.setColor(skyColor);
    starsSprite.setColor(starsColor);

    // Рисуем фон (неподвижный)
    window.draw(starsSprite);
    window.draw(skySprite);

    // Рисуем землю (если в зоне видимости)
    if (ground->getBounds().top < gameView.getCenter().y + gameView.getSize().y/2) {
        ground->draw(window);
    }

    // Рисуем остальные объекты
    platform->draw(window);
    booster->draw(window);

    drawHUD();
    drawMarker();
    
    if (showExitButtonVisible) {
        sf::Vector2f viewCenter = gameView.getCenter();
        
        // Настраиваем сообщение
        resultMessageText.setString(resultMessage);
        resultMessageText.setFillColor(messageColor);
        sf::Color outlineColor(
            messageColor.r * 0.5f,
            messageColor.g * 0.5f,
            messageColor.b * 0.5f
        );
        resultMessageText.setOutlineColor(outlineColor);
        resultMessageText.setOutlineThickness(3);
        
        sf::FloatRect textBounds = resultMessageText.getLocalBounds();
        resultMessageText.setOrigin(textBounds.left + textBounds.width/2.0f,
                                  textBounds.top + textBounds.height/2.0f);
        resultMessageText.setPosition(viewCenter.x, viewCenter.y - 120);
        
        // Настраиваем кнопку рестарта
        restartButton.setSize(sf::Vector2f(200, 50));
        restartButton.setFillColor(sf::Color::White);
        restartButton.setOutlineThickness(1);
        restartButton.setOutlineColor(sf::Color(100, 100, 100));
        restartButton.setOrigin(restartButton.getSize().x/2, restartButton.getSize().y/2);
        restartButton.setPosition(viewCenter.x, viewCenter.y - 20);
        
        restartButtonText.setString("Restart game");
        restartButtonText.setFont(font);
        restartButtonText.setCharacterSize(24);
        restartButtonText.setFillColor(sf::Color::Black);
        restartButtonText.setOutlineThickness(0);
        sf::FloatRect restartTextBounds = restartButtonText.getLocalBounds();
        restartButtonText.setOrigin(restartTextBounds.left + restartTextBounds.width/2.0f,
                                 restartTextBounds.top + restartTextBounds.height/2.0f);
        restartButtonText.setPosition(restartButton.getPosition());
        
        // Настраиваем кнопку выхода
        exitButton.setSize(sf::Vector2f(200, 50));
        exitButton.setFillColor(sf::Color::White);
        exitButton.setOutlineThickness(1);
        exitButton.setOutlineColor(sf::Color(100, 100, 100));
        exitButton.setOrigin(exitButton.getSize().x/2, exitButton.getSize().y/2);
        exitButton.setPosition(viewCenter.x, viewCenter.y + 50);
        
        exitButtonText.setString("Exit game");
        exitButtonText.setFont(font);
        exitButtonText.setCharacterSize(24);
        exitButtonText.setFillColor(sf::Color::Black);
        exitButtonText.setOutlineThickness(0);
        sf::FloatRect exitTextBounds = exitButtonText.getLocalBounds();
        exitButtonText.setOrigin(exitTextBounds.left + exitTextBounds.width/2.0f,
                               exitTextBounds.top + exitTextBounds.height/2.0f);
        exitButtonText.setPosition(exitButton.getPosition());
        
        // Рендерим элементы
        window.draw(resultMessageText);
        window.draw(restartButton);
        window.draw(restartButtonText);
        window.draw(exitButton);
        window.draw(exitButtonText);
    }
    
    window.display();
}

void Game::drawHUD() {
    if (font.loadFromFile("Arial.ttf") && booster && ground) {
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

        // Исправляем обращение к ground через ->
        float heightAboveGround = ground->getPosition().y - (booster->getPosition().y + 30);
        text.setString("Height: " + std::to_string(heightAboveGround));
        text.setPosition(viewTopLeft.x + 10, viewTopLeft.y + 100);
        window.draw(text);

        float distanceFromPlatform = booster->getPosition().x;
        text.setString("Distance to platform " + std::to_string(distanceFromPlatform));
        text.setPosition(viewTopLeft.x + 10, viewTopLeft.y + 130);
        window.draw(text);

        if (menuScreen.isWindEnabled()) {
            sf::Vector2f wind = booster->getWind();
            text.setString("Wind: (" + std::to_string(wind.x) + ", " + std::to_string(wind.y) + ")");
            text.setPosition(viewTopLeft.x + 10, viewTopLeft.y + 160);
            window.draw(text);
        }
    }
}

void Game::drawMarker() {
    marker.draw(window);
}

void Game::initializeGame() {
    delete booster;
    delete platform;
    delete ground;

    windowSize = window.getSize();
    
    // Создаем платформу
    platform = new Platform(-100, windowSize.y - 50);
    platform->setTexture(platformTexture);
    
    // Создаем землю как физический объект
    ground = new Platform(-50000.f, windowSize.y - 50);
    ground->setSize(sf::Vector2f(100000.f, 50.f));
    ground->setTexture(groundTexture);

    // Инициализация бустера
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> xDist(-1500, 1500);
    std::uniform_int_distribution<> yDist(-2000, -1000);
    std::uniform_int_distribution<> velXDist(-200, 200);
    std::uniform_int_distribution<> velYDist(0, 200);
    
    float randomX = static_cast<float>(xDist(gen));
    float randomY = static_cast<float>(yDist(gen));
    float randomVelX = static_cast<float>(velXDist(gen));
    float randomVelY = static_cast<float>(velYDist(gen));
    
    booster = new Booster(randomX, randomY, this);
    booster->setVelocity(randomVelX, randomVelY);
    booster->setEngineTiltEnabled(menuScreen.isEngineTiltEnabled());
    booster->setGravity(menuScreen.getGravity());

    if (menuScreen.isWindEnabled()) {
        booster->setWind(menuScreen.getWindForce());
    }
    
    showExitButtonVisible = false;
}

void Game::showExitButton(const std::string& message, const sf::Color& color) {
    showExitButtonVisible = true;
    resultMessage = message;
    messageColor = color;
    
    // Выключаем огонь двигателей при завершении игры
    if (booster) {
        booster->applyThrust(0);
    }
}

void Game::initRestartButton() {
    restartButton.setSize(sf::Vector2f(200, 50));
    restartButton.setFillColor(sf::Color::White);
    restartButton.setOutlineThickness(1);
    restartButton.setOutlineColor(sf::Color::Black);
    
    restartButtonText.setFont(font);
    restartButtonText.setString("Restart Game");
    restartButtonText.setCharacterSize(24);
    restartButtonText.setFillColor(sf::Color::Black);
    
    sf::FloatRect textRect = restartButtonText.getLocalBounds();
    restartButtonText.setOrigin(textRect.left + textRect.width/2.0f,
                             textRect.top + textRect.height/2.0f);
}

void Game::handleRestartButtonEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos = window.mapPixelToCoords(
            sf::Mouse::getPosition(window));
        
        if (restartButton.getGlobalBounds().contains(mousePos)) {
            restartGame();
        }
    }
}

void Game::restartGame() {
    delete booster;
    delete platform;
    booster = nullptr;
    platform = nullptr;
    showExitButtonVisible = false;
    initializeGame();
}

void Game::initMessageBackground() {
    messageBackground.setFillColor(sf::Color::Transparent);
    messageBackground.setOutlineThickness(2);
    messageBackground.setOutlineColor(sf::Color::White);
    messageBackground.setSize(sf::Vector2f(400, 100));
    messageBackground.setOrigin(
        messageBackground.getSize().x / 2, 
        messageBackground.getSize().y / 2
    );
}