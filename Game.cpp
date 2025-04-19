#include "Game.h"
#include <iostream>
#include <random>
#include <cmath>

Game::Game()
    : window(sf::VideoMode::getDesktopMode(), "Booster Landing Game", sf::Style::Fullscreen),
      booster(nullptr), platform(nullptr), ground(nullptr), menuScreen(window, font),
      windowSize(window.getSize()), currentSkyColor(generateRandomSkyColor()),
      explosionBackgroundAlpha(0), isExplosionAnimating(false), explosionAnimationComplete(false), currentExplosionFrame(0),
      explosionAnimationPlaying(false)
{
    // Загрузка шрифта
    if (!font.loadFromFile("HarreeghPoppedCyrillic.ttf")) {
        std::cerr << "Failed to load font! Using default." << std::endl;
        font.loadFromFile("Arial.ttf");
    }

    // Загрузка фоновых текстур
    if (!skyTexture.loadFromFile("sky.png")) {
        std::cerr << "Failed to load sky texture!" << std::endl;
        sf::Image img;
        img.create(8400, 6000, sf::Color(100, 150, 255));
        skyTexture.loadFromImage(img);
    }
    skySprite.setTexture(skyTexture);
    skySprite.setPosition(-4000, -4600);

    if (!starsTexture.loadFromFile("stars.png")) {
        std::cerr << "Failed to load stars texture!" << std::endl;
        sf::Image img;
        img.create(7826, 6000, sf::Color(10, 10, 30));
        starsTexture.loadFromImage(img);
    }
    starsSprite.setTexture(starsTexture);
    starsSprite.setPosition(-4000, -4600);

    // Загрузка текстуры взрыва
    if (!explosionTexture.loadFromFile("explosion.png")) {
        std::cerr << "Failed to load explosion texture!" << std::endl;
        sf::Image img;
        img.create(1, 1, sf::Color(255, 0, 0));
        explosionTexture.loadFromImage(img);
    }
    explosionSprite.setTexture(explosionTexture);

    // Загрузка текстур объектов
    if (!boosterTexture.loadFromFile("booster.png") ||
        !engineTexture.loadFromFile("engine.png") ||
        !platformTexture.loadFromFile("platform.png") ||
        !groundTexture.loadFromFile("ground.png")) {
        std::cerr << "Failed to load object textures!" << std::endl;
    }

    // Загрузка текстур кнопок
    if (!retryTexture.loadFromFile("retry.png")) {
        std::cerr << "Failed to load retry button texture!" << std::endl;
    }
    if (!restartTexture.loadFromFile("restart.png")) {
        std::cerr << "Failed to load restart button texture!" << std::endl;
    }
    if (!quitTexture.loadFromFile("quit.png")) {
        std::cerr << "Failed to load quit button texture!" << std::endl;
    }

    // Загрузка текстур для анимации взрыва
    for (int i = 0; i < 7; ++i) {
        sf::Texture texture;
        std::string filename = "explosion" + std::to_string(i) + ".png";
        if (!texture.loadFromFile(filename)) {
            // Создаем простую текстуру, если загрузка не удалась
            sf::Image img;
            img.create(64, 64, sf::Color(255, 100, 0, 200));
            texture.loadFromImage(img);
        }
        explosionTextures.push_back(texture);
    }

    currentSkyColor = generateRandomSkyColor();

    window.setFramerateLimit(60);
    gameView = window.getDefaultView();
    initExitButtons();
}

Game::~Game() {
    delete booster;
    delete platform;
    delete ground;
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
            handleRetryButtonEvent(event);
            handleRestartButtonEvent(event);
            handleQuitButtonEvent(event);
        }
        else if (menuScreen.isMenuActive()) {
            menuScreen.handleEvent(event);
        }
    }

    // Обработка наведения на кнопки
    sf::Color hoverColor(110, 160, 210); // Светло-голубой

    if (showExitButtonVisible) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        
        retryButton.setOutlineThickness(retryButton.getGlobalBounds().contains(mousePos) ? 5 : 0);
        retryButton.setOutlineColor(hoverColor);
    
        restartButton.setOutlineThickness(restartButton.getGlobalBounds().contains(mousePos) ? 5 : 0);
        restartButton.setOutlineColor(hoverColor);
    
        quitButton.setOutlineThickness(quitButton.getGlobalBounds().contains(mousePos) ? 5 : 0);
        quitButton.setOutlineColor(hoverColor);
    }

    if (booster && !menuScreen.isMenuActive() && !showExitButtonVisible) {
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

void Game::update() {
    if (booster == nullptr || platform == nullptr || ground == nullptr) return;

    float deltaTime = clock.restart().asSeconds();
    if(booster) {
        booster->update(deltaTime);
    }
    
    booster->checkLanding(platform->getBounds(), ground->getBounds());
    marker.update(*booster, *platform, window);
    updateCamera();
}

void Game::render() {
    window.clear();

    // Если анимация взрыва не активна, полностью сбрасываем прозрачность
    if (!isExplosionAnimating) {
        explosionBackgroundAlpha = 0;
    }

    // Рассчитываем прозрачность для обычного фона
    float heightRatio = (-booster->getPosition().y) / 3000.0f;
    heightRatio = std::max(0.0f, std::min(1.0f, heightRatio));

    // Если идет анимация взрыва, плавно увеличиваем прозрачность фона взрыва
    if (isExplosionAnimating) {
        float animTime = explosionAnimClock.getElapsedTime().asSeconds();
        explosionBackgroundAlpha = static_cast<sf::Uint8>(std::min(255.0f, animTime * 50)); // Плавное увеличение
    }

    // Рисуем обычный фон (небо и звезды)
    sf::Color dynamicSkyColor = currentSkyColor;
    dynamicSkyColor.a = static_cast<sf::Uint8>(255 * (1.0f - heightRatio));
    
    sf::Color starsColor(255, 255, 255, static_cast<sf::Uint8>(255 * heightRatio));
    
    skySprite.setColor(dynamicSkyColor);
    starsSprite.setColor(starsColor);

    window.draw(starsSprite);
    window.draw(skySprite);

    // Если есть прозрачность фона взрыва, рисуем его поверх обычного фона
    if (explosionBackgroundAlpha > 0) {
        explosionSprite.setPosition(gameView.getCenter() - gameView.getSize()/2.f);
        explosionSprite.setScale(
            window.getSize().x / explosionSprite.getLocalBounds().width,
            window.getSize().y / explosionSprite.getLocalBounds().height
        );
        explosionSprite.setColor(sf::Color(255, 255, 255, explosionBackgroundAlpha));
        window.draw(explosionSprite);
    }

    // Рисуем землю (если в зоне видимости)
    if (ground->getBounds().top < gameView.getCenter().y + gameView.getSize().y/2) {
        ground->setColor(currentSkyColor);
        ground->draw(window);
    }

    // Рисуем платформу
    platform->setColor(currentSkyColor);

    platform->draw(window);

     // Анимация взрыва
    if (booster && booster->isExploded()) {
        if (!explosionAnimationPlaying && !explosionAnimationComplete) {
            // Начинаем анимацию только если она еще не игралась и не завершена
            explosionAnimationPlaying = true;
            currentExplosionFrame = 0;
            explosionAnimClock.restart();
        }

        if (explosionAnimationPlaying) {
            float frameTime = explosionAnimClock.getElapsedTime().asSeconds();
            
            // Переход к следующему кадру
            if (frameTime > 0.1f && currentExplosionFrame < explosionTextures.size() - 1) {
                currentExplosionFrame++;
                explosionAnimClock.restart();
            }
            // Завершение анимации
            else if (currentExplosionFrame >= explosionTextures.size() - 1 && frameTime > 0.1f) {
                explosionAnimationPlaying = false;
                explosionAnimationComplete = true;
            }

            // Рисуем текущий кадр
            sf::Sprite explosionSprite;
            explosionSprite.setTexture(explosionTextures[currentExplosionFrame]);
            explosionSprite.setOrigin(explosionTextures[currentExplosionFrame].getSize().x / 2, 
                                   explosionTextures[currentExplosionFrame].getSize().y / 2);
            explosionSprite.setPosition(booster->getPosition());
            explosionSprite.setScale(2.0f, 2.0f);
            window.draw(explosionSprite);
        }
    }
    else if (booster) {
        booster->draw(window);
    }

    drawMarker();
    
    if (showExitButtonVisible) {
        sf::Vector2f viewCenter = gameView.getCenter();
        
        // Определяем смещение для взрыва
        float yOffset = (resultMessage.find("BOOM!") != std::string::npos) ? 100.f : 0.f;
        
        // Настраиваем сообщение
        resultMessageText.setFont(font);
        resultMessageText.setString(resultMessage);
        resultMessageText.setFillColor(messageColor);
        resultMessageText.setCharacterSize(36);
        resultMessageText.setOutlineColor(sf::Color::Black);
        resultMessageText.setOutlineThickness(3);
        
        sf::FloatRect textBounds = resultMessageText.getLocalBounds();
        resultMessageText.setOrigin(textBounds.left + textBounds.width/2.0f,
                                  textBounds.top + textBounds.height/2.0f);
        resultMessageText.setPosition(viewCenter.x, viewCenter.y - 200 + yOffset);
        
        // Настраиваем кнопки
        retryButton.setOrigin(retryButton.getSize().x/2, retryButton.getSize().y/2);
        retryButton.setPosition(viewCenter.x - 350, viewCenter.y + yOffset);
        
        restartButton.setOrigin(restartButton.getSize().x/2, restartButton.getSize().y/2);
        restartButton.setPosition(viewCenter.x, viewCenter.y + yOffset);
        
        quitButton.setOrigin(quitButton.getSize().x/2, quitButton.getSize().y/2);
        quitButton.setPosition(viewCenter.x + 350, viewCenter.y + yOffset);
        
        // Рендерим элементы
        window.draw(resultMessageText);
        window.draw(retryButton);
        window.draw(restartButton);
        window.draw(quitButton);
    }
    
    window.display();
}

void Game::initExitButtons() {
    retryButton.setSize(sf::Vector2f(312, 312));
    retryButton.setTexture(&retryTexture);
    retryButton.setOutlineThickness(0);
    retryButton.setOutlineColor(sf::Color::White);
    
    restartButton.setSize(sf::Vector2f(312, 312));
    restartButton.setTexture(&restartTexture);
    restartButton.setOutlineThickness(0);
    restartButton.setOutlineColor(sf::Color::White);
    
    quitButton.setSize(sf::Vector2f(312, 312));
    quitButton.setTexture(&quitTexture);
    quitButton.setOutlineThickness(0);
    quitButton.setOutlineColor(sf::Color::White);
}

void Game::handleRetryButtonEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        if (retryButton.getGlobalBounds().contains(mousePos)) {
            retryGame();
        }
    }
}

void Game::handleRestartButtonEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        if (restartButton.getGlobalBounds().contains(mousePos)) {
            restartGame();
        }
    }
}

void Game::handleQuitButtonEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        if (quitButton.getGlobalBounds().contains(mousePos)) {
            window.close();
        }
    }
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

void Game::drawHUD() {
    if (booster && ground) {
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
    ground = new Platform(-60000.f, windowSize.y - 50);
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

    if (booster) {
        booster->applyThrust(0);
    }

    if (message.find("BOOM!") != std::string::npos) {
        isExplosionAnimating = true;
        explosionBackgroundAlpha = 0;
        explosionAnimClock.restart();
        // Не сбрасываем здесь анимацию, она запустится автоматически в render()
    }
}


void Game::restartGame() {
    delete booster;
    delete platform;
    booster = nullptr;
    platform = nullptr;
    showExitButtonVisible = false;
    
    // Полный сброс состояния анимации
    explosionAnimationPlaying = false;
    explosionAnimationComplete = false;
    currentExplosionFrame = 0;
    isExplosionAnimating = false;
    explosionBackgroundAlpha = 0;
    
    menuScreen.resetMenu();
    currentSkyColor = generateRandomSkyColor();
    
    windowSize = window.getSize();
    gameView.setSize(windowSize.x, windowSize.y);
    gameView.setCenter(windowSize.x/2, windowSize.y/2);
    window.setView(gameView);
}

void Game::retryGame() {
    delete booster;
    delete platform;
    booster = nullptr;
    platform = nullptr;
    showExitButtonVisible = false;
    
    // Полный сброс состояния анимации
    explosionAnimationPlaying = false;
    explosionAnimationComplete = false;
    currentExplosionFrame = 0;
    isExplosionAnimating = false;
    explosionBackgroundAlpha = 0;
    
    initializeGame();
}

const sf::Texture& Game::getFlameTexture(int index) const {
    static std::vector<sf::Texture> flameTextures;
    if (flameTextures.empty()) {
        flameTextures.resize(6);
        for (int i = 0; i < 6; ++i) {
            std::string filename = "flame" + std::to_string(i) + ".png";
            if (!flameTextures[i].loadFromFile(filename)) {
                // Создаем прозрачную текстуру, если загрузка не удалась
                sf::Image img;
                img.create(32, 64, sf::Color::Transparent);
                flameTextures[i].loadFromImage(img);
            }
        }
    }
    return flameTextures.at(index % 6);
}

sf::Color Game::generateRandomSkyColor() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> colorDist(50, 255); // Не слишком тёмные цвета
    
    // Генерация приятных пастельных тонов
    return sf::Color(
        colorDist(gen), // R
        colorDist(gen), // G
        colorDist(gen), // B
        255             // Alpha (будет регулироваться отдельно)
    );
}