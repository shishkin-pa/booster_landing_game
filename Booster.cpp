#include "Booster.h"
#include "Game.h"
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <ctime>

// Функция для нормализации угла в диапазон [-180, 180]
float normalizeAngle(float angle) {
    while (angle > 180.0f) angle -= 360.0f;
    while (angle < -180.0f) angle += 360.0f;
    return angle;
}

Booster::Booster(float x, float y, Game* game)
    : position(x, y), velocity(0, 0), angle(0), engineAngle(0), landed(false), exploded(false),
      thrustForce(0), angularVelocity(0), windForce(0, 0), engineTiltEnabled(true),
      gravity(0), landingMessageShown(false), groundLandingMessageShown(false), game(game) {
    shape.setSize(sf::Vector2f(40, 60)); // Размер бустера
    shape.setFillColor(sf::Color::Red); // Цвет бустера
    shape.setOrigin(shape.getSize().x / 2, shape.getSize().y / 2); // Центр вращения в центре масс
    shape.setPosition(position);

    // Случайная начальная скорость (вниз)
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    velocity.y = (std::rand() % 80 + 10); // Случайная вертикальная скорость (100 до 900)
    velocity.x = 0; // Начальная горизонтальная скорость равна 0

    // Случайный начальный угол наклона (от -60 до 60 градусов)
    initialAngle = (std::rand() % 120 - 60); // Случайный угол от -60 до 60 градусов
    angle = initialAngle;
    shape.setRotation(angle);

    // Инициализация двигателей
    updateEngines();
}

Booster::~Booster() {
    // Деструктор для очистки памяти
}

void Booster::update(float deltaTime) {
    if (!landed && !exploded) {
        // Гравитация
        velocity.y += gravity * deltaTime;

        // Ветер
        velocity += windForce * deltaTime;

        // Сопротивление воздуха
        velocity.x -= airResistance * velocity.x * deltaTime;
        velocity.y -= airResistance * velocity.y * deltaTime;

        // Обновление позиции
        position += velocity * deltaTime;

        // Проверка высоты: если бустер выше 3500, возвращаем его на высоту 3500
        const float maxHeight = 4000.0f;
        if (position.y < -maxHeight) {
            position.y = -maxHeight;
            // Обнуляем вертикальную скорость, если она направлена вверх (отрицательная в SFML)
            if (velocity.y < 0) {
                velocity.y = 0;
            }
        }

        const float maxLength = 3000.0f;
        if (position.x < -maxLength) {
            position.x = maxLength;
        }

        if (position.x > maxLength) {
            position.x = -maxLength;
        }

        shape.setPosition(position);

        // Момент силы от двигателей
        float torque = thrustForce * (shape.getSize().x / 2) * std::sin(engineAngle * 3.14159265f / 180.0f);
        angularVelocity += torque * deltaTime;
        angularVelocity -= angularDamping * angularVelocity * deltaTime; // Затухание вращения
        angle += angularVelocity * deltaTime;

        // Нормализация угла
        angle = normalizeAngle(angle);

        shape.setRotation(angle);

        // Обновление позиции и угла двигателей
        updateEngines();
    }
}

void Booster::applyThrust(float thrust) {
    if (!landed && !exploded) {
        thrustForce = thrust;
        
        // Направление тяги зависит от угла наклона двигателей
        float radians = (angle + engineAngle) * 3.14159265f / 180.0f;
        velocity.x += thrust * std::sin(radians);
        velocity.y -= thrust * std::cos(radians);
        
        leftEngine.setFlameVisible(thrust > 0);
        rightEngine.setFlameVisible(thrust > 0);

    } else {
        // Выключаем огонь на двигателях
        leftEngine.setFlameVisible(false);
        rightEngine.setFlameVisible(false);
    }
}

void Booster::rotateEngines(float deltaAngle) {
    if (!landed && !exploded && engineTiltEnabled) {
        engineAngle += deltaAngle;
        // Ограничение угла наклона двигателей
        if (engineAngle > maxEngineAngle) engineAngle = maxEngineAngle;
        if (engineAngle < -maxEngineAngle) engineAngle = -maxEngineAngle;
    }
}

void Booster::rotateGyroscope(float deltaAngle, float deltaTime) {
    if (!landed && !exploded) {
        // Медленная коррекция угла с помощью гироскопа
        float torque = engineTiltEnabled ? gyroscopeTorque : gyroscopeTorque * 10;
        angularVelocity += torque * deltaAngle * deltaTime;
    }
}

void Booster::checkLanding(const sf::FloatRect& platformBounds, const sf::FloatRect& groundBounds) {
    if (!exploded && shape.getGlobalBounds().intersects(platformBounds)) {
        if (std::abs(velocity.y) < maxLandingSpeed && 
            std::abs(velocity.x) < maxHorizontalLandingSpeed && 
            std::abs(angle) < 6) {
            landed = true;
            velocity = sf::Vector2f(0, 0);
            shape.setFillColor(sf::Color::Green);
            leftEngine.setFlameVisible(false);  // Выключаем огонь
            rightEngine.setFlameVisible(false); // Выключаем огонь
            if (!landingMessageShown) {
                std::cout << "Congratulations! You've successfully landed the booster!" << std::endl;
                if (game) game->showExitButton("Congratulations! You've successfully landed the booster!", sf::Color::Green);
                landingMessageShown = true;
            }
        } else {
            explode();
        }
    } else if (!exploded && position.y > 1000) {
        if (std::abs(velocity.y) < maxLandingSpeed && 
            std::abs(velocity.x) < maxHorizontalLandingSpeed && 
            std::abs(angle) < 6) {
            landed = true;
            velocity = sf::Vector2f(0, 0);
            shape.setFillColor(sf::Color::Yellow);
            leftEngine.setFlameVisible(false);  // Выключаем огонь
            rightEngine.setFlameVisible(false); // Выключаем огонь
            if (!groundLandingMessageShown) {
                std::cout << "You cannot land the booster on the ground!" << std::endl;
                if (game) game->showExitButton("You cannot land the booster on the ground!", sf::Color::Yellow);
                groundLandingMessageShown = true;
            }
        } else {
            explode();
        }
    }
}

void Booster::explode() {
    if (!exploded) {
        exploded = true;
        shape.setFillColor(sf::Color::Transparent);
        std::cout << "BOOM! The booster has exploded!" << std::endl;
        if (game) game->showExitButton("BOOM! The booster has exploded!", sf::Color::Red);
    }
}

void Booster::draw(sf::RenderWindow& window) {
    if (!exploded) {
        window.draw(shape); // Отрисовка бустера, если он не взорвался
    }

    // Отрисовка двигателей с учётом состояния взрыва
    leftEngine.draw(window, exploded);
    rightEngine.draw(window, exploded);
}

// Остальные методы остаются без изменений
bool Booster::isLanded() const { return landed; }
bool Booster::isExploded() const { return exploded; }
sf::Vector2f Booster::getVelocity() const { return velocity; }
void Booster::setVelocity(float x, float y) {
    velocity.x = x;
    velocity.y = y;
}
float Booster::getAngle() const { return angle; }
float Booster::getInitialAngle() const { return initialAngle; }
void Booster::setWind(const sf::Vector2f& windForce) { this->windForce = windForce; }
sf::Vector2f Booster::getWind() const { return windForce; }
sf::Vector2f Booster::getPosition() const { return position; }
void Booster::setPosition(float x, float y) {
    position.x = x;
    position.y = y;
    shape.setPosition(position);
    updateEngines();
}
void Booster::setEngineTiltEnabled(bool enabled) {
    engineTiltEnabled = enabled;
    if (!enabled) {
        engineAngle = 0;
    }
}
void Booster::setGravity(float gravity) { this->gravity = gravity; }

void Booster::updateEngines() {
    // Позиция двигателей относительно центра масс бустера
    float radians = angle * 3.14159265f / 180.0f; // Угол в радианах

    // Левый двигатель
    float leftX = position.x - (shape.getSize().x / 2) * std::cos(radians) - (shape.getSize().y / 2) * std::sin(radians);
    float leftY = position.y - (shape.getSize().x / 2) * std::sin(radians) + (shape.getSize().y / 2) * std::cos(radians);
    leftEngine.setPosition(sf::Vector2f(leftX, leftY));
    leftEngine.setRotation(angle + engineAngle);

    // Правый двигатель
    float rightX = position.x + (shape.getSize().x / 2) * std::cos(radians) - (shape.getSize().y / 2) * std::sin(radians);
    float rightY = position.y + (shape.getSize().x / 2) * std::sin(radians) + (shape.getSize().y / 2) * std::cos(radians);
    rightEngine.setPosition(sf::Vector2f(rightX, rightY));
    rightEngine.setRotation(angle + engineAngle);
}