#ifndef FLYINGEYE_H
#define FLYINGEYE_H
#pragma once

#include "Enemy.h"
#include "EnemyProjectile.h"

class EvilEye : public Enemy
{
public:

    EvilEye(const sf::Vector2f& m_position, std::vector<Block>& m_blocks);

    void update(sf::Time m_deltaTime, const Player& m_player) override;
    void render(sf::RenderWindow& m_window) override;

private:

    void loadTextures();
    void loadFrames();
    void setupBoundingBox();
    void setupDebug();

    void createInitialPatrolPoint();
    void createNewPatrolPoint();
    void moveToNextPatrolPoint(sf::Time m_deltaTime);
    void delayIfReachedPoint(sf::Time m_deltaTime);
    void updateEvilEyeAnimation(sf::Time m_deltaTime, const Player& m_player);
    void shootProjectile(const Player& m_player);
    bool isValidPatrolPoint(const sf::Vector2f& m_point) const;
    bool isLineOfSightClearPatrol(const sf::Vector2f& m_from, const sf::Vector2f& m_to);

    sf::Vector2f generateRandomPointOnCircle(float m_radius) const;

    std::vector<sf::Vector2f> m_patrolPoints;
    std::vector<EnemyProjectile> m_projectiles;

    std::vector<sf::CircleShape> m_patrolDebugCircles;
    sf::CircleShape m_debugCircleForPatrol;
    sf::Color m_lineColor;

    sf::Time m_waitTime = sf::seconds(2.0f);
    sf::Time m_currentWaitTime = sf::Time::Zero;
    sf::Time m_shootTimer = sf::seconds(4.0f);
    sf::Time m_timeSinceLastShot = sf::Time::Zero;

    const int m_amountOfPatrolPoints = 3;
    int m_currentPatrolIndex = 0;
    float m_patrolRadius = 500.0f;
    float m_fallSpeed = 200.0f;

    bool m_isWaiting = false;
    bool m_isReturning = false;
    bool m_hasHitGround = false;

};

#endif