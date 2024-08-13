#pragma once
#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <SFML/Graphics.hpp>
#include "Block.h"
#include "Player.h"

class EnemyProjectile
{
public:

    EnemyProjectile(const sf::Vector2f& m_position, const sf::Vector2f& m_direction, const sf::Texture& m_texture, const std::vector<sf::IntRect>& m_frames);

    void update(sf::Time m_deltaTime, const std::vector<Block>& blocks);
    void render(sf::RenderWindow& m_window);

    bool checkCollisionWithPlayer(Player& m_player);
    bool isDestroyed() const;
    sf::FloatRect getBoundingBox() const;

private:

    sf::Sprite m_sprite;

    sf::Vector2f m_direction;
    float m_speed = 300.0f;

    sf::Time m_currentFrameTime;
    sf::Time m_frameTime;
    std::vector<sf::IntRect> m_frames;
    int m_currentFrame = 0;
    int m_destructionStartFrame = 3;

    bool m_isDestroyed = false;
    bool m_isInDestructionSequence = false;

};

#endif
