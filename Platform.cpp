#include "Platform.h"

Platform::Platform(float x, float y) {
    shape.setSize(sf::Vector2f(200, 20)); // Размер платформы
    shape.setFillColor(sf::Color::Blue); // Цвет платформы
    shape.setPosition(x, y); // Позиция платформы
}

void Platform::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

sf::FloatRect Platform::getBounds() const {
    return shape.getGlobalBounds();
}

void Platform::setPosition(float x, float y) {
    shape.setPosition(x, y);
}