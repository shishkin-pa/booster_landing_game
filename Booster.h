#ifndef BOOSTER_H
#define BOOSTER_H

#include <SFML/Graphics.hpp>
#include "Engine.h"

class Booster {
public:
    Booster(float x, float y);
    ~Booster(); // Деструктор для очистки памяти

    void update(float deltaTime);
    void applyThrust(float thrust);
    void rotateEngines(float deltaAngle);
    void checkLanding(const sf::FloatRect& platformBounds, const sf::FloatRect& groundBounds);
    void explode();
    void draw(sf::RenderWindow& window);

    bool isLanded() const;
    bool isExploded() const;

    void rotateGyroscope(float deltaAngle, float deltaTime);

    sf::Vector2f getVelocity() const;
    float getAngle() const;
    float getInitialAngle() const;

    void setWind(const sf::Vector2f& windForce);
    sf::Vector2f getWind() const;

    sf::Vector2f getPosition() const;
    void setPosition(float x, float y);

    void setEngineTiltEnabled(bool enabled);
    void setGravity(float gravity);

private:
    void updateEngines();

    sf::Vector2f position; // Позиция центра масс бустера
    sf::Vector2f velocity; // Скорость бустера
    sf::RectangleShape shape; // Графический объект бустера
    Engine leftEngine; // Левый двигатель
    Engine rightEngine; // Правый двигатель
    float angle; // Угол поворота бустера
    float initialAngle; // Начальный угол наклона бустера
    float engineAngle; // Угол наклона двигателей
    float angularVelocity; // Угловая скорость
    bool landed; // Флаг приземления
    bool exploded; // Флаг взрыва
    float thrustForce; // Сила тяги
    sf::Vector2f windForce; // Сила ветра
    bool engineTiltEnabled; // Включен ли наклон двигателей
    float gravity; // Гравитация
    bool landingMessageShown; // Флаг для отображения сообщения о посадке
    bool groundLandingMessageShown; // Флаг для отображения сообщения о посадке на землю

    // Константы
    const float maxLandingSpeed = 100.0f; // Максимальная скорость для успешного приземления
    const float maxHorizontalLandingSpeed = 50.0f; // Максимальная горизонтальная скорость для успешного приземления
    const float airResistance = 0.2f; // Сопротивление воздуха
    const float angularDamping = 0.5f; // Затухание вращения
    const float maxEngineAngle = 30.0f; // Максимальный угол наклона двигателей
    const float gyroscopeTorque = 20.0f; // Момент силы гироскопа
};

#endif