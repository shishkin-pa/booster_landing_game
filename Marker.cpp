#include "Marker.h"
#include <cmath>

Marker::Marker() : isVisible(false), distanceToPlatform(0) {
    markerShape.setPointCount(3);
    markerShape.setPoint(0, sf::Vector2f(0, -15));  // Вершина
    markerShape.setPoint(1, sf::Vector2f(-10, 10)); // Левый нижний угол
    markerShape.setPoint(2, sf::Vector2f(10, 10));  // Правый нижний угол
    
    markerShape.setFillColor(sf::Color(200, 100, 255));
    markerShape.setOutlineThickness(2);
    markerShape.setOutlineColor(sf::Color(150, 50, 200));
    markerShape.setOrigin(0, 0); // Центр в вершине треугольника
}

void Marker::update(const Booster& booster, const Platform& platform, const sf::RenderWindow& window) {
    sf::FloatRect platformBounds = platform.getBounds();
    sf::View view = window.getView();
    sf::FloatRect viewArea(
        view.getCenter() - view.getSize()/2.f, 
        view.getSize()
    );

    // Проверяем видимость платформы
    isVisible = viewArea.intersects(platformBounds);
    if (isVisible) return;

    // Фиксированные отступы в пикселях
    const float sideMargin = 50.f;   // Расстояние от боковых краёв
    const float bottomMargin = 40.f; // Расстояние от нижнего края

    // Вычисляем допустимую область для маркера
    float markerMinX = viewArea.left + sideMargin;
    float markerMaxX = viewArea.left + viewArea.width - sideMargin;
    float markerY = viewArea.top + viewArea.height - bottomMargin;

    // Позиция платформы по X
    float platformCenterX = platformBounds.left + platformBounds.width/2;

    // Вычисляем позицию маркера с жёстким ограничением
    float markerX;
    if (platformCenterX < markerMinX) {
        markerX = markerMinX;
    } 
    else if (platformCenterX > markerMaxX) {
        markerX = markerMaxX;
    } 
    else {
        // Если платформа в зоне видимости, маркер точно над ней
        markerX = platformCenterX;
    }

    // Позиция маркера
    sf::Vector2f markerPos(markerX, markerY);

    // Вычисляем направление к платформе
    sf::Vector2f platformCenter(platformCenterX, platformBounds.top + platformBounds.height/2);
    sf::Vector2f direction = platformCenter - markerPos;
    float angle = atan2(direction.y, direction.x) * 180.f / 3.14159265f;
    angle += 90.f; // Корректируем угол для треугольника

    // Масштабирование в зависимости от расстояния
    distanceToPlatform = std::abs(booster.getPosition().y - platformBounds.top);
    float scale = std::min(1.5f, 1000.f/distanceToPlatform);

    // Устанавливаем параметры маркера
    markerShape.setPosition(markerPos);
    markerShape.setRotation(angle);
    markerShape.setScale(scale, scale);
}

void Marker::draw(sf::RenderWindow& window) const {
    if (!isVisible) {
        window.draw(markerShape);
    }
}