#include "Marker.h"
#include <cmath>

Marker::Marker() : isVisible(false), distanceToPlatform(0) {
    markerShape.setRadius(10);
    markerShape.setFillColor(sf::Color::Blue); // Изменил на синий
    markerShape.setOutlineThickness(2);
    markerShape.setOutlineColor(sf::Color::White);
    markerShape.setOrigin(10, 10);
}

void Marker::update(const Booster& booster, const Platform& platform, const sf::RenderWindow& window) {
    sf::FloatRect platformBounds = platform.getBounds();
    sf::FloatRect viewArea(window.getView().getCenter() - window.getView().getSize()/2.f, 
                         window.getView().getSize());

    // Проверяем видимость платформы
    isVisible = viewArea.intersects(platformBounds);
    if (isVisible) return;

    // Центр платформы
    sf::Vector2f platformCenter(platformBounds.left + platformBounds.width/2, 
                               platformBounds.top + platformBounds.height/2);
    
    // Позиция маркера - всегда у нижнего края экрана
    sf::Vector2f viewCenter = window.getView().getCenter();
    sf::Vector2f viewSize = window.getView().getSize();
    sf::Vector2f markerPos(viewCenter.x, viewCenter.y + viewSize.y/2 - 20); // 20 - отступ от края

    // Смещаем маркер по X к положению платформы
    float platformX = platformCenter.x;
    float viewLeft = viewCenter.x - viewSize.x/2;
    float viewRight = viewCenter.x + viewSize.x/2;
    
    // Ограничиваем положение маркера в пределах экрана
    platformX = std::max(viewLeft + 20, std::min(viewRight - 20, platformX));
    markerPos.x = platformX;

    // Обновляем положение и масштаб маркера
    distanceToPlatform = std::abs(booster.getPosition().y - platformCenter.y);
    float scale = std::min(1.5f, 1000.f/distanceToPlatform);
    
    markerShape.setPosition(markerPos);
    markerShape.setScale(scale, scale);
}

void Marker::draw(sf::RenderWindow& window) const {
    if (!isVisible) {
        window.draw(markerShape);
    }
}