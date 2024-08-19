#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Block.h"
#include "Player.h"

class GasProjectile
{
public:

    GasProjectile(const sf::Vector2f& m_startPosition, const sf::Vector2f& m_direction, float m_getSpeed);

    void update(sf::Time m_deltaTime, const std::vector<Block>& m_blocks, Player& m_player);
    void render(sf::RenderWindow& m_window);

    bool isDestroyed() const;

private:

    void initGasSprite();
    void handleCollision(const std::vector<Block>& m_blocks);
    void updateAnimation(sf::Time m_deltaTime);
    void checkCollisionWithPlayer(Player& m_player);

    sf::Sprite m_sprite;
    static sf::Texture m_texture;

    sf::Vector2f m_velocity;
    sf::Time m_elapsedTime;

    int m_currentFrame = 0;
    sf::Time m_frameTime = sf::seconds(0.1f);
    sf::Time m_currentFrameTime = sf::Time::Zero;
    sf::IntRect m_frameRect;

    float m_speed;
    float m_lifetime = 10.0f;
    bool m_isDestroyed = false;

};

