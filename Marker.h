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
    sf::CircleShape markerShape;
    bool isVisible;
    float distanceToPlatform;
};

#endif // MARKER_H