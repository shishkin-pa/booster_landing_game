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

private:
    sf::RectangleShape shape; // Графический объект двигателя
    sf::RectangleShape flame; // Графический объект огня
    bool flameVisible; // Видимость огня
};

#endif