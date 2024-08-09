#ifndef FLYINGEYE_H
#define FLYINGEYE_H
#pragma once

#include "Enemy.h"

class EvilEye : public Enemy
{
public:

    EvilEye(const sf::Vector2f& m_position, const std::vector<Block>& m_blocks);

    void update(sf::Time m_deltaTime) override;
    void render(sf::RenderWindow& m_window) override;

private:

    void loadTextures();
    void loadFrames();
    void setupBoundingBox();
    void setupDebug();

    void findPatrolPoints(const std::vector<Block>& m_blocks);
    void moveTowardsCurrentPoint(sf::Time m_deltaTime);
    void delayIfReachedPoint(sf::Time m_deltaTime);
    bool isValidPatrolPoint(const sf::Vector2f& m_point, const std::vector<Block>& m_blocks) const;

    sf::Vector2f generateRandomPointOnCircle(float radius) const;

    const std::vector<Block>& m_gameBlocks;
    std::vector<sf::Vector2f> m_patrolPoints;
    sf::RectangleShape m_collisionBox;
    std::vector<sf::CircleShape> m_patrolDebugCircles;
    sf::CircleShape m_debugCircleForPatrol;

    sf::Time m_waitTime = sf::seconds(2.0f);
    sf::Time m_currentWaitTime = sf::Time::Zero;
    bool m_isWaiting = false;

    int m_currentPatrolIndex = 0;

};

#endif