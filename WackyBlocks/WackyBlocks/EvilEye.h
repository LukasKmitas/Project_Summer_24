#ifndef FLYINGEYE_H
#define FLYINGEYE_H
#pragma once
#include "Enemy.h"
#include "EnemyProjectile.h"

class EvilEye : public Enemy
{
public:

    EvilEye(const sf::Vector2f& m_position, std::vector<Block>& m_blocks);
    ~EvilEye();

    void update(sf::Time m_deltaTime, Player& m_player) override;
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
    void updateEvilEyeAnimation(sf::Time m_deltaTime, Player& m_player);
    void shootProjectile(const Player& m_player);

    bool isValidPatrolPoint(const sf::Vector2f& m_point) const;
    bool isLineOfSightClearPatrol(const sf::Vector2f& m_from, const sf::Vector2f& m_to);

    sf::Vector2f generateRandomPointOnCircle(float m_radius) const;

    std::vector<sf::Vector2f> m_patrolPoints;
    std::vector<EnemyProjectile> m_projectiles;
    std::vector<sf::CircleShape> m_patrolDebugCircles;
    
    sf::Vector2f m_originalPosition;
    sf::Vector2f m_targetPosition;

    sf::CircleShape m_debugCircleForPatrol;
    sf::Color m_lineColor;

    sf::Time m_waitTime = sf::seconds(2.0f);
    sf::Time m_currentWaitTime = sf::Time::Zero;
    sf::Time m_shootTimer = sf::seconds(4.0f);
    sf::Time m_timeSinceLastShot = sf::Time::Zero;
    sf::Time m_spinAttackCooldown = sf::seconds(5.0f);
    sf::Time m_timeSinceLastSpinAttack = sf::Time::Zero;
    sf::Time m_biteAttackCooldown = sf::seconds(6.0f);
    sf::Time m_timeSinceLastBiteAttack = sf::Time::Zero;

    const int m_amountOfPatrolPoints = 3;
    int m_currentPatrolIndex = 0;
    float m_patrolRadius = 500.0f;
    float m_fallSpeed = 200.0f;
    float m_knockbackStrength = 300.0f;

    bool m_isWaiting = false;
    bool m_isReturning = false;
    bool m_hasHitGround = false;
    bool m_canSpinAttack = true;
    bool m_canBiteAttack = true;

};

#endif