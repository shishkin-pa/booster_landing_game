#include "Engine.h"
#include <cmath>

Engine::Engine() : flameVisible(false), currentFlameFrame(0), flameAnimationTime(0), 
                  flameFrameDuration(0.05f), isStarting(true) {
    shape.setSize(sf::Vector2f(30, 64));
    shape.setOrigin(shape.getSize().x / 2, shape.getSize().y);
    flame.setSize(sf::Vector2f(30, 45));
    flame.setOrigin(flame.getSize().x / 2, flame.getSize().y - 15);
}

void Engine::setTexture(const sf::Texture& texture) {
    shape.setTexture(&texture);
    shape.setFillColor(sf::Color::White);
}

void Engine::setFlameTextures(const std::vector<const sf::Texture*>& textures) {
    flameTextures = textures;
    if (!flameTextures.empty()) {
        flame.setTexture(flameTextures[0]);
    }
}

void Engine::setPosition(const sf::Vector2f& position) {
    shape.setPosition(position);
    
    float radians = shape.getRotation() * 3.14159265f / 180.0f;
    float offsetX = 0;
    float offsetY = shape.getSize().y / 2;
    float flameX = position.x + offsetX * cos(radians) - offsetY * sin(radians);
    float flameY = position.y + offsetX * sin(radians) + offsetY * cos(radians);
    flame.setPosition(flameX, flameY);
}

void Engine::setRotation(float angle) {
    shape.setRotation(angle);
    flame.setRotation(angle);
    setPosition(shape.getPosition());
}

void Engine::update(float deltaTime) {
    if (!flameVisible || flameTextures.empty()) return;

    flameAnimationTime += deltaTime;
    
    if (flameAnimationTime >= flameFrameDuration) {
        flameAnimationTime = 0;
        
        if (isStarting) {
            currentFlameFrame++;
            if (currentFlameFrame >= 3) {
                isStarting = false;
                currentFlameFrame = 3;
            }
        } else {
            currentFlameFrame = 3 + (currentFlameFrame + 1 - 3) % 3;
        }
        
        if (currentFlameFrame < flameTextures.size()) {
            flame.setTexture(flameTextures[currentFlameFrame], true);
        }
    }
}

void Engine::draw(sf::RenderWindow& window, bool isExploded) {
    if (!isExploded) {
        window.draw(shape);
        if (flameVisible && !flameTextures.empty()) {
            window.draw(flame);
        }
    }
}

void Engine::setFlameVisible(bool visible) {
    if (flameVisible != visible) {
        flameVisible = visible;
        if (visible) {
            isStarting = true;
            currentFlameFrame = 0;
            flameAnimationTime = 0;
            if (!flameTextures.empty()) {
                flame.setTexture(flameTextures[0]);
            }
        }
    }
}