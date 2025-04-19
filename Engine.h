#ifndef ENGINE_H
#define ENGINE_H

#include <SFML/Graphics.hpp>
#include <vector>

class Engine {
public:
    Engine();
    void setPosition(const sf::Vector2f& position);
    void setRotation(float angle);
    void draw(sf::RenderWindow& window, bool isExploded);
    void setFlameVisible(bool visible);
    void setTexture(const sf::Texture& texture);
    void setFlameTextures(const std::vector<const sf::Texture*>& textures); // Исправлено на const
    void update(float deltaTime);

private:
    sf::RectangleShape shape;
    sf::RectangleShape flame;
    bool flameVisible;
    std::vector<const sf::Texture*> flameTextures;
    int currentFlameFrame;
    float flameAnimationTime;
    float flameFrameDuration;
    bool isStarting;
};

#endif // ENGINE_H