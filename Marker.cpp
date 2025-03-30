#include "Marker.h"

Marker::Marker(MarkerType type) : type(type), scale(maxScale), distanceToEdge(0) {
    createCircleMarker();
    createTriangleMarker();
}

void Marker::update(const Booster& booster, const sf::RenderWindow& window) {
    sf::Vector2f boosterPos = booster.getPosition();
    sf::Vector2u windowSize = window.getSize();
    
    distanceToEdge = 0.0f;
    if (boosterPos.x < 0) distanceToEdge = -boosterPos.x;
    else if (boosterPos.x > windowSize.x) distanceToEdge = boosterPos.x - windowSize.x;
    else if (boosterPos.y < 0) distanceToEdge = -boosterPos.y;
    else if (boosterPos.y > windowSize.y) distanceToEdge = boosterPos.y - windowSize.y;
    
    updateScale(distanceToEdge);
    updatePosition(boosterPos, windowSize);
    triangleMarker.setRotation(booster.getAngle());
}

void Marker::draw(sf::RenderWindow& window) const {
    if (distanceToEdge > 0) {
        if (type == MarkerType::CIRCLE) {
            window.draw(circleMarker);
        } else {
            window.draw(triangleMarker);
        }
    }
}

void Marker::setType(MarkerType type) {
    this->type = type;
}

MarkerType Marker::getType() const {
    return type;
}

void Marker::updatePosition(const sf::Vector2f& boosterPos, const sf::Vector2u& windowSize) {
    float x = boosterPos.x;
    float y = boosterPos.y;
    
    if (type == MarkerType::CIRCLE) {
        if (boosterPos.x < 0) x = 10 * scale;
        else if (boosterPos.x > windowSize.x) x = windowSize.x - 10 * scale;
        if (boosterPos.y < 0) y = 10 * scale;
        else if (boosterPos.y > windowSize.y) y = windowSize.y - 10 * scale;
        
        circleMarker.setPosition(x, y);
    } else {
        float verticalOffset = 20 * scale;
        
        if (boosterPos.x < 0) x = 15 * scale;
        else if (boosterPos.x > windowSize.x) x = windowSize.x - 15 * scale;
        if (boosterPos.y < 0) y = verticalOffset;
        else if (boosterPos.y > windowSize.y) y = windowSize.y - verticalOffset + 8 * scale;
        
        triangleMarker.setPosition(x, y);
    }
}

void Marker::updateScale(float distance) {
    scale = maxScale - (maxScale - minScale) * std::min(distance / maxDistance, 1.0f);
    
    circleMarker.setRadius(8 * scale);
    circleMarker.setOutlineThickness(1.5f * scale);
    
    float baseWidth = 18 * scale;
    float sideLength = 25 * scale;
    
    triangleMarker.setPoint(0, sf::Vector2f(0, -sideLength));
    triangleMarker.setPoint(1, sf::Vector2f(-baseWidth/2, 4 * scale));
    triangleMarker.setPoint(2, sf::Vector2f(baseWidth/2, 4 * scale));
    triangleMarker.setOutlineThickness(1.5f * scale);
}

void Marker::createCircleMarker() {
    circleMarker.setRadius(8);
    circleMarker.setFillColor(sf::Color::Red);
    circleMarker.setOutlineThickness(1.5f);
    circleMarker.setOutlineColor(outlineColor);
    circleMarker.setOrigin(8, 8);
}

void Marker::createTriangleMarker() {
    triangleMarker.setPointCount(3);
    triangleMarker.setFillColor(sf::Color::Red);
    triangleMarker.setOutlineColor(outlineColor);
    triangleMarker.setOutlineThickness(1.5f);
}