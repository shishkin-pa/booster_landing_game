#ifndef MARKER_H
#define MARKER_H

#include <SFML/Graphics.hpp>
#include "Booster.h"

enum class MarkerType {
    CIRCLE,
    TRIANGLE
};

class Marker {
public:
    Marker(MarkerType type);
    
    void update(const Booster& booster, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window) const;
    
    void setType(MarkerType type);
    MarkerType getType() const;

private:
    void updatePosition(const sf::Vector2f& boosterPos, const sf::Vector2u& windowSize);
    void updateScale(float distanceToEdge);
    void createCircleMarker();
    void createTriangleMarker();

    MarkerType type;
    sf::Vector2f position;
    float scale;
    float distanceToEdge;
    
    sf::CircleShape circleMarker;
    sf::ConvexShape triangleMarker;
    
    const float maxDistance = 1500.0f;
    const float minScale = 0.4f;
    const float maxScale = 1.5f;
    const sf::Color outlineColor = sf::Color(100, 0, 0);
};

#endif // MARKER_H