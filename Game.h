#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "Booster.h"
#include "Platform.h"
#include "MenuScreen.h"
#include "Marker.h"

class Game {
public:
    Game();
    ~Game();
    
    void run();
    void showExitButton(const std::string& message, const sf::Color& color);
    
    bool isExitButtonVisible() const { return showExitButtonVisible; }
    const sf::Texture& getBoosterTexture() const { return boosterTexture; }
    const sf::Texture& getEngineTexture() const { return engineTexture; }
    const sf::Texture& getPlatformTexture() const { return platformTexture; }
    const sf::Texture& getFlameTexture(int index) const;

    float YOffset;
private:
    void handleEvents();
    void update();
    void render();
    void drawHUD();
    void drawMarker();
    void initializeGame();
    void updateCamera();
    void initExitButtons();
    void handleExitButtonEvent(sf::Event& event);
    void handleRetryButtonEvent(sf::Event& event);
    void handleRestartButtonEvent(sf::Event& event);
    void handleQuitButtonEvent(sf::Event& event);
    void retryGame();
    void restartGame();

    sf::RenderWindow window;
    sf::View gameView;
    Booster* booster;
    Platform* platform;
    Platform* ground;
    sf::Clock clock;
    sf::Font font;
    sf::Vector2u windowSize;

    // Фон
    sf::Texture skyTexture;
    sf::Texture starsTexture;
    sf::Texture explosionTexture;
    sf::Sprite skySprite;
    sf::Sprite starsSprite;
    sf::Sprite explosionSprite;

    MenuScreen menuScreen;
    Marker marker;
    
    // Кнопки на конечном экране
    sf::RectangleShape retryButton;
    sf::RectangleShape restartButton;
    sf::RectangleShape quitButton;
    sf::Texture retryTexture;
    sf::Texture restartTexture;
    sf::Texture quitTexture;
    
    bool showExitButtonVisible = false;
    std::string resultMessage;
    sf::Color messageColor;
    sf::Text resultMessageText;

    sf::Texture boosterTexture;
    sf::Texture engineTexture;
    sf::Texture flameTexture;
    sf::Texture platformTexture;
    sf::Texture groundTexture;

    sf::Color generateRandomSkyColor(); // Новый метод
    sf::Color currentSkyColor; // Текущий цвет неба

    std::vector<sf::Texture> explosionTextures; // Для анимации взрыва

    float explosionBackgroundAlpha; // Прозрачность фона взрыва (0-255)
    bool isExplosionAnimating;      // Флаг анимации взрыва
    sf::Clock explosionAnimClock;   // Таймер анимации
    bool explosionAnimationComplete; // Флаг завершения анимации взрыва
    int currentExplosionFrame; // Текущий кадр анимации взрыва
    bool explosionAnimationPlaying; // Флаг проигрывания анимации
};

#endif // GAME_H