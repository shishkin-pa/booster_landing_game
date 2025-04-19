#ifndef PLATFORM_H
#define PLATFORM_H

#include <SFML/Graphics.hpp>

class Platform {
public:
    Platform(float x, float y);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    void setPosition(float x, float y);
    sf::Vector2f getPosition() const;  // Добавленный метод
    void setTexture(const sf::Texture& texture);
    void setSize(const sf::Vector2f& size);
    sf::Vector2f getSize() const;      // Добавленный метод
    void setColor(const sf::Color& color);

private:
    sf::RectangleShape shape;
};

#endif