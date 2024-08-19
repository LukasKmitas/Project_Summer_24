#pragma once

#include "Enemy.h"
#include "EnemyProjectile.h"

class Skeleton : public Enemy
{
public:

    Skeleton(const sf::Vector2f& m_position, std::vector<Block>& m_blocks);
    ~Skeleton();

    void update(sf::Time m_deltaTime, Player& m_player) override;
    void render(sf::RenderWindow& m_window) override;

private:

    void loadTextures();
    void loadFrames();
    void setupBoundingBox();

    void updateSkeletonAnimation(sf::Time m_deltaTime, Player& m_player);
    void shootProjectile(const Player& m_player);
    bool isBulletMovingTowardsSkeleton(const Bullet& m_bullet);

    std::vector<EnemyProjectile> m_projectiles;

    sf::RectangleShape m_shieldDectectionBox;

    sf::Time m_attackCooldown = sf::seconds(4.0f);
    sf::Time m_timeSinceLastAttack = sf::Time::Zero;
    sf::Time m_rangeAttackCooldown = sf::seconds(6.0f);
    sf::Time m_timeSinceLastRangeAttack = sf::Time::Zero;

};

