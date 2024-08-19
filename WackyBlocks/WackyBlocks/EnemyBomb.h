#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Block.h"

class EnemyBomb
{
public:

    EnemyBomb(const sf::Vector2f& m_startPosition, const sf::Vector2f& m_targetPosition);

    void update(sf::Time m_deltaTime, const std::vector<Block>& m_blocks);
    void render(sf::RenderWindow& m_window);

    void markPlayerDamaged();

    bool isDestroyed() const;
    bool hasDamagedPlayer() const;

    sf::FloatRect getExplosionHitbox() const;

private:

    void setupBomb();

    sf::Sprite m_sprite;
    sf::Texture m_texture;

    sf::CircleShape m_collisionCircle;
    sf::CircleShape m_explosionHitbox;

    sf::Time m_timeToExplode = sf::seconds(4.0f);
    sf::Time m_timeSinceLanded = sf::Time::Zero;

    std::vector<sf::IntRect> m_bombFrames;
    std::vector<sf::IntRect> m_explosionFrames;

    int m_currentFrame = 0;
    sf::Time m_frameTime = sf::seconds(0.1f);
    sf::Time m_currentFrameTime = sf::Time::Zero;

    sf::Vector2f m_velocity;

    float m_gravity = 400.0f;

    bool m_isRolling = false;
    bool m_hasExploded = false;
    bool m_isDestroyed = false;
    bool m_hasDamagedPlayer = false;

};

