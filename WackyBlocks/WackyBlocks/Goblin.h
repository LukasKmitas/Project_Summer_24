#pragma once
#include "Enemy.h"
#include "EnemyBomb.h"

class Goblin : public Enemy
{
public:

    Goblin(const sf::Vector2f& m_position, std::vector<Block>& m_blocks);
    ~Goblin();

    void update(sf::Time m_deltaTime, Player& m_player) override;
    void render(sf::RenderWindow& m_window) override;

private:

    void loadTextures();
    void loadFrames();
    void setupBoundingBox();

    void updateGoblinAnimation(sf::Time m_deltaTime, Player& m_player);
    void throwBomb(Player& player);

    std::vector<EnemyBomb> m_bombs;

    sf::Time m_attackCooldown = sf::seconds(3.0f);
    sf::Time m_timeSinceLastAttack = sf::Time::Zero;
    sf::Time m_rangeAttackCooldown = sf::seconds(8.0f);
    sf::Time m_timeSinceLastRangeAttack = sf::Time::Zero;

};

