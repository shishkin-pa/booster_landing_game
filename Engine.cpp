#include "Engine.h"
#include <cmath>

Engine::Engine() : flameVisible(false) {
    shape.setSize(sf::Vector2f(180, 180));
    shape.setOrigin(shape.getSize().x / 2, shape.getSize().y);
    flame.setSize(sf::Vector2f(15, 30));
    flame.setOrigin(flame.getSize().x / 2, flame.getSize().y);
}

void Engine::setTexture(const sf::Texture& texture) {
    shape.setTexture(&texture);
    shape.setFillColor(sf::Color::White); // Белый цвет для правильного отображения текстуры
}

void Engine::setFlameTexture(const sf::Texture& texture) {
    flame.setTexture(&texture);
    flame.setFillColor(sf::Color::White);
}

void Engine::setPosition(const sf::Vector2f& position) {
    shape.setPosition(position);

    // Позиция огня рассчитывается относительно нижней грани двигателя
    float radians = shape.getRotation() * 3.14159265f / 180.0f; // Угол в радианах
    float offsetX = 0; // Смещение по X
    float offsetY = shape.getSize().y; // Смещение по Y (нижняя грань двигателя)

    // Новые координаты огня с учётом угла наклона
    float flameX = position.x + offsetX * cos(radians) - offsetY * sin(radians);
    float flameY = position.y + offsetX * sin(radians) + offsetY * cos(radians);

    flame.setPosition(flameX, flameY);
}

void Engine::setRotation(float angle) {
    shape.setRotation(angle);
    flame.setRotation(angle);

    // При изменении угла наклона двигателя нужно обновить позицию огня
    setPosition(shape.getPosition());
}

void Engine::draw(sf::RenderWindow& window, bool isExploded) {
    if (!isExploded) { // Отрисовываем двигатели только если бустер не взорвался
        window.draw(shape); // Отрисовка двигателя
        if (flameVisible) {
            window.draw(flame); // Отрисовка огня
        }
    }
}

void Engine::setFlameVisible(bool visible) {
    flameVisible = visible;
}