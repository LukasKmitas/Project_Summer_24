#ifndef ENEMY_H
#define ENEMY_H
#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Block.h"
#include "Global.h"
#include "Player.h"

enum class EnemyState 
{
	IDLE,
	ATTACK1,
    ATTACK2,
    ATTACK3,
	PATROL,
	DEATH,
	TAKE_HIT,
	SHIELD
};

class Enemy
{
public:

	Enemy(const sf::Vector2f& m_position, std::vector<Block>& m_blocks);
	virtual ~Enemy();

	virtual void update(sf::Time m_deltaTime, Player& m_player);
	virtual void render(sf::RenderWindow& m_window);

	virtual void takeDamage(int m_damage);

	sf::FloatRect getBoundingBox() const;
	bool isDead() const;

protected:

    void setState(EnemyState m_state);
    void setupHealthBar();

    void updateAnimation(sf::Time m_deltaTime);
    void updateHealthBar();

    void initVisionCone();
    void updateVisionCone();
    bool isPlayerInVisionCone(const Player& m_player) const;
    bool isLineOfSightClearToPlayer(const sf::Vector2f& m_from, const sf::Vector2f& m_to) const;

    std::vector<Block>& m_gameBlocks;

    EnemyState m_animationState;
    EnemyState m_previousState;

    sf::Sprite m_sprite;

    sf::RectangleShape m_collisionBox;
    sf::RectangleShape m_healthBarBackground;
    sf::RectangleShape m_healthBar;
    sf::ConvexShape m_visionCone;

    sf::Texture m_idleTexture;
    sf::Texture m_walkTexture;
    sf::Texture m_attack1Texture;
    sf::Texture m_attack2Texture;
    sf::Texture m_attack3Texture;
    sf::Texture m_projectileTexture;
    sf::Texture m_deathTexture;
    sf::Texture m_shieldTexture;
    sf::Texture m_takeHitTexture;

    std::vector<sf::IntRect> m_idleFrames;
    std::vector<sf::IntRect> m_walkFrames;
    std::vector<sf::IntRect> m_attack1Frames;
    std::vector<sf::IntRect> m_attack2Frames;
    std::vector<sf::IntRect> m_attack3Frames;
    std::vector<sf::IntRect> m_projectileFrames;
    std::vector<sf::IntRect> m_deathFrames;
    std::vector<sf::IntRect> m_shieldFrames;
    std::vector<sf::IntRect> m_takeHitFrames;

    sf::Vector2f m_playerPos;

    int m_health = 100;
    float m_speed = 100.0f;
    float m_visionRange = 300.0f;
    float m_visionAngle = 100.0f;
    const int m_numPoints = 30;
    const float m_maxHealth = 100.0f;

    int m_currentFrame = 0;
    sf::Time m_frameTime = sf::seconds(0.1f);
    sf::Time m_currentFrameTime = sf::Time::Zero;

    bool m_isDead = false;
    bool m_playerFound = false;
    bool m_hasShotProjectile = false;

};

#endif