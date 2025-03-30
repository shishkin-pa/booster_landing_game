#ifndef BOOSTER_H
#define BOOSTER_H

#include <SFML/Graphics.hpp>
#include "Engine.h"

class Game; // Предварительное объявление

class Booster {
public:
    Booster(float x, float y, Game* game);
    ~Booster();

    void update(float deltaTime);
    void applyThrust(float thrust);
    void rotateEngines(float deltaAngle);
    void rotateGyroscope(float deltaAngle, float deltaTime);
    void checkLanding(const sf::FloatRect& platformBounds, const sf::FloatRect& groundBounds);
    void explode();
    void draw(sf::RenderWindow& window);

    bool isLanded() const;
    bool isExploded() const;
    sf::Vector2f getVelocity() const;
    float getAngle() const;
    float getInitialAngle() const;
    void setWind(const sf::Vector2f& windForce);
    sf::Vector2f getWind() const;
    sf::Vector2f getPosition() const;
    void setPosition(float x, float y);
    void setVelocity(float x, float y);
    void setEngineTiltEnabled(bool enabled);
    void setGravity(float gravity);

private:
    void updateEngines();

    Game* game;
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::RectangleShape shape;
    Engine leftEngine;
    Engine rightEngine;
    float angle;
    float initialAngle;
    float engineAngle;
    float angularVelocity;
    bool landed;
    bool exploded;
    float thrustForce;
    sf::Vector2f windForce;
    bool engineTiltEnabled;
    float gravity;
    bool landingMessageShown;
    bool groundLandingMessageShown;

    const float maxLandingSpeed = 100.0f;
    const float maxHorizontalLandingSpeed = 50.0f;
    const float airResistance = 0.2f;
    const float angularDamping = 0.5f;
    const float maxEngineAngle = 30.0f;
    const float gyroscopeTorque = 20.0f;
};

#endif