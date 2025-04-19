#ifndef MARKER_H
#define MARKER_H

#include <SFML/Graphics.hpp>
#include "Booster.h"
#include "Platform.h"

class Marker {
public:
    Marker();
    void update(const Booster& booster, const Platform& platform, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window) const;

private:
    sf::ConvexShape markerShape;
    bool isVisible;
    float distanceToPlatform;
    sf::Vector2f platformPosition;
    sf::Vector2f lastValidPosition; // Добавляем для хранения последней корректной позиции
};

#endif // MARKER_H