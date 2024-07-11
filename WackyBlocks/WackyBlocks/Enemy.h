#ifndef ENEMY_H
#define ENEMY_H
#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

enum class EnemyState 
{
	IDLE,
	ATTACK1,
    ATTACK2,
    ATTACK3,
	WALK,
	DEATH,
	TAKE_HIT,
	SHIELD
};

class Enemy
{
public:

	Enemy(const sf::Vector2f& m_position);
	virtual ~Enemy();

	virtual void update(sf::Time m_deltaTime);
	virtual void render(sf::RenderWindow& m_window);

	virtual void takeDamage(int m_damage);

	sf::FloatRect getBoundingBox() const;
	bool isDead() const;

protected:

    void updateAnimation(sf::Time m_deltaTime);
    void setState(EnemyState m_state);

    EnemyState m_animationState;

    sf::Sprite m_sprite;

    sf::Texture m_idleTexture;
    sf::Texture m_walkTexture;
    sf::Texture m_attack1Texture;
    sf::Texture m_attack2Texture;
    sf::Texture m_attack3Texture;
    sf::Texture m_deathTexture;
    sf::Texture m_shieldTexture;
    sf::Texture m_takeHitTexture;

    std::vector<sf::IntRect> m_idleFrames;
    std::vector<sf::IntRect> m_walkFrames;
    std::vector<sf::IntRect> m_attack1Frames;
    std::vector<sf::IntRect> m_attack2Frames;
    std::vector<sf::IntRect> m_attack3Frames;
    std::vector<sf::IntRect> m_deathFrames;
    std::vector<sf::IntRect> m_shieldFrames;
    std::vector<sf::IntRect> m_takeHitFrames;

    int m_health = 100;
    int m_currentFrame = 0;
    sf::Time m_frameTime = sf::seconds(0.1f);
    sf::Time m_currentFrameTime = sf::Time::Zero;
    bool m_isDead = false;

};

#endif