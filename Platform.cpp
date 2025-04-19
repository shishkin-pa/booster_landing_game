#include "Platform.h"

Platform::Platform(float x, float y) {
    shape.setSize(sf::Vector2f(600, 40));
    shape.setPosition(x, y);
    shape.setFillColor(sf::Color::White);
}

void Platform::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

void Platform::setColor(const sf::Color& color) {
    shape.setFillColor(color);
}

sf::FloatRect Platform::getBounds() const {
    return shape.getGlobalBounds();
}

void Platform::setPosition(float x, float y) {
    shape.setPosition(x, y);
}

sf::Vector2f Platform::getPosition() const {
    return shape.getPosition();
}

void Platform::setTexture(const sf::Texture& texture) {
    shape.setTexture(&texture);
}

void Platform::setSize(const sf::Vector2f& size) {
    shape.setSize(size);
}

sf::Vector2f Platform::getSize() const {
    return shape.getSize();
}