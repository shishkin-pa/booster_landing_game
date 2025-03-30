#include "Platform.h"

// Конструктор платформы
Platform::Platform(float x, float y) {
    shape.setSize(sf::Vector2f(200, 20)); // Установка размера платформы (ширина 200, высота 20)
    shape.setFillColor(sf::Color::Blue);  // Установка синего цвета платформы
    shape.setPosition(x, y);              // Установка позиции платформы
}

// Отрисовка платформы в окне
void Platform::draw(sf::RenderWindow& window) {
    window.draw(shape); // Отрисовка прямоугольника платформы
}

// Получение границ платформы для проверки коллизий
sf::FloatRect Platform::getBounds() const {
    return shape.getGlobalBounds(); // Возвращает глобальные границы прямоугольника
}

// Установка новой позиции платформы
void Platform::setPosition(float x, float y) {
    shape.setPosition(x, y); // Устанавливает новую позицию прямоугольника
}