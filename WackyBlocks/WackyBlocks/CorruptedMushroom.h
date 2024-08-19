#pragma once

#include "Enemy.h"
#include "GasProjectile.h"

class CorruptedMushroom : public Enemy
{
public:

    CorruptedMushroom(const sf::Vector2f& m_position, std::vector<Block>& m_blocks);
    ~CorruptedMushroom();

    void update(sf::Time m_deltaTime, Player& m_player) override;
    void render(sf::RenderWindow& m_window) override;

private:

    void loadTextures();
    void loadFrames();
    void setupBoundingBox();

    void updateMushroomAnimation(sf::Time m_deltaTime, Player& m_player);
    void spawnGasProjectiles();

    std::vector<GasProjectile> m_gasProjectiles;

    sf::Time m_attackCooldown = sf::seconds(4.0f);
    sf::Time m_timeSinceLastAttack = sf::Time::Zero;
    sf::Time m_gasAttackCooldown = sf::seconds(10.0f);
    sf::Time m_timeSinceLastGasAttack = sf::Time::Zero;

};

