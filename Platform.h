#ifndef PLATFORM_H
#define PLATFORM_H

#include <SFML/Graphics.hpp>

// Класс Platform представляет платформу для посадки бустера
class Platform {
public:
    // Конструктор, принимающий координаты платформы
    Platform(float x, float y);
    
    // Отрисовка платформы в окне
    void draw(sf::RenderWindow& window);
    
    // Получение границ платформы (для коллизий)
    sf::FloatRect getBounds() const;
    
    // Установка позиции платформы
    void setPosition(float x, float y);

private:
    sf::RectangleShape shape; // Графический объект платформы (прямоугольник)
};

#endif