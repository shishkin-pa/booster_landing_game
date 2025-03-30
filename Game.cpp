#include "Game.h"
#include <iostream>
#include <random>

Game::Game()
    : window(sf::VideoMode::getDesktopMode(), "Booster Landing Game", sf::Style::Fullscreen),
      booster(nullptr), platform(nullptr), menuScreen(window, font),
      marker(), windowSize(window.getSize()) 
{
    if (!font.loadFromFile("Arial.ttf")) {
        std::cerr << "Failed to load font! Text will not be displayed." << std::endl;
    }
    window.setFramerateLimit(60);
    gameView = window.getDefaultView();
    
    groundVertices.setPrimitiveType(sf::Quads);
    groundVertices.resize(4);
    
    // Настройка текста результата
    resultMessageText.setFont(font);
    resultMessageText.setCharacterSize(36); // Увеличили размер
    resultMessageText.setStyle(sf::Text::Bold);
}

Game::~Game() {
    delete booster;
    delete platform;
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

        if (showExitButtonVisible) { // Используем флаг видимости
            handleExitButtonEvent(event);
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
    if (booster == nullptr || platform == nullptr) return;

    float deltaTime = clock.restart().asSeconds();
    booster->update(deltaTime);
    booster->checkLanding(platform->getBounds(), ground.getGlobalBounds());
    marker.update(*booster, *platform, window);
    updateCamera();
}

void Game::updateCamera() {
    if (!booster) return;

    sf::Vector2f viewCenter = booster->getPosition();
    float groundBottom = ground.getPosition().y + ground.getSize().y;
    float viewHeight = gameView.getSize().y;

    if (viewCenter.y + viewHeight/2 > groundBottom) {
        viewCenter.y = groundBottom - viewHeight/2;
    }

    gameView.setCenter(viewCenter);
    window.setView(gameView);
}

void Game::render() {
    window.clear();
    
    // Рендеринг земли
    sf::Vector2f viewCenter = gameView.getCenter();
    sf::Vector2f viewSize = gameView.getSize();
    float groundLevel = windowSize.y - 50;
    
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
    
    if (platform) platform->draw(window);
    if (booster) booster->draw(window);
    drawHUD();
    drawMarker();
    
    if (showExitButtonVisible) {
        sf::Vector2f viewCenter = gameView.getCenter();
        
        // Настраиваем сообщение
        resultMessageText.setString(resultMessage);
        resultMessageText.setFillColor(messageColor);
        
        // Вычисляем цвет окантовки (средний между цветом текста и чёрным)
        sf::Color outlineColor(
            messageColor.r * 0.5f,
            messageColor.g * 0.5f,
            messageColor.b * 0.5f
        );
        resultMessageText.setOutlineColor(outlineColor);
        resultMessageText.setOutlineThickness(3);
        
        // Центрируем сообщение
        sf::FloatRect textBounds = resultMessageText.getLocalBounds();
        resultMessageText.setOrigin(textBounds.left + textBounds.width/2.0f,
                                  textBounds.top + textBounds.height/2.0f);
        resultMessageText.setPosition(viewCenter.x, viewCenter.y - 80);
        
        // Настраиваем кнопку
        exitButton.setSize(sf::Vector2f(200, 50));
        exitButton.setFillColor(sf::Color::White);
        exitButton.setOutlineThickness(1);
        exitButton.setOutlineColor(sf::Color(100, 100, 100)); // Серый контур
        exitButton.setOrigin(exitButton.getSize().x/2, exitButton.getSize().y/2);
        exitButton.setPosition(viewCenter.x, viewCenter.y + 20);
        
        // Настраиваем текст кнопки
        exitButtonText.setString("Exit game");
        exitButtonText.setFont(font);
        exitButtonText.setCharacterSize(24);
        exitButtonText.setFillColor(sf::Color::Black);
        exitButtonText.setOutlineThickness(0);
        sf::FloatRect buttonTextBounds = exitButtonText.getLocalBounds();
        exitButtonText.setOrigin(buttonTextBounds.left + buttonTextBounds.width/2.0f,
                               buttonTextBounds.top + buttonTextBounds.height/2.0f);
        exitButtonText.setPosition(exitButton.getPosition());
        
        // Рендерим элементы
        window.draw(resultMessageText);
        window.draw(exitButton);
        window.draw(exitButtonText);
    }
    
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
    if (platform) delete platform;
    if (booster) delete booster;

    windowSize = window.getSize();
    
    if (!groundTexture.loadFromFile("ground_texture.jpg")) {
        sf::Image img;
        img.create(32, 32, sf::Color::Green);
        groundTexture.loadFromImage(img);
    }

    platform = new Platform(-100, windowSize.y - 50);
    ground.setSize(sf::Vector2f(100000.f, 50.f));
    ground.setPosition(-50000.f, windowSize.y - 50);
    ground.setFillColor(sf::Color::Transparent);

    // Генерация случайных координат и скоростей
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> xDist(-1500, 1500);
    std::uniform_int_distribution<> yDist(-2000, -1000);
    std::uniform_int_distribution<> velXDist(-200, 200);
    std::uniform_int_distribution<> velYDist(0, 200); // Положительная вертикальная скорость
    
    float randomX = static_cast<float>(xDist(gen));
    float randomY = static_cast<float>(yDist(gen));
    float randomVelX = static_cast<float>(velXDist(gen));
    float randomVelY = static_cast<float>(velYDist(gen));
    
    booster = new Booster(randomX, randomY, this);
    booster->setVelocity(randomVelX, randomVelY); // Передаем скорости как два отдельных аргумента
    booster->setEngineTiltEnabled(menuScreen.isEngineTiltEnabled());
    booster->setGravity(menuScreen.getGravity());

    if (menuScreen.isWindEnabled()) {
        booster->setWind(menuScreen.getWindForce());
    }
    
    showExitButtonVisible = false;
}

void Game::initMessageBackground() {
    messageBackground.setFillColor(sf::Color::Transparent);
    messageBackground.setOutlineThickness(2);
    messageBackground.setOutlineColor(sf::Color::White);
}

void Game::initResultMessage() {
    resultMessageText.setFont(font);
    resultMessageText.setCharacterSize(30);
    resultMessageText.setStyle(sf::Text::Bold);
    resultMessageText.setOutlineThickness(2);
    resultMessageText.setOutlineColor(sf::Color::Black);
}

void Game::showExitButton(const std::string& message, const sf::Color& color) {
    showExitButtonVisible = true;
    resultMessage = message;
    messageColor = color;
}
