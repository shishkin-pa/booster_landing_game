#ifndef ENGINE_H
#define ENGINE_H

#include <SFML/Graphics.hpp>

class Engine {
public:
    Engine();
    void setPosition(const sf::Vector2f& position);
    void setRotation(float angle);
    void draw(sf::RenderWindow& window, bool isExploded);
    void setFlameVisible(bool visible);
    void setTexture(const sf::Texture& texture);
    void setFlameTexture(const sf::Texture& texture);

private:
    sf::RectangleShape shape;
    sf::RectangleShape flame;
    bool flameVisible;
};

#endif // ENGINE_H