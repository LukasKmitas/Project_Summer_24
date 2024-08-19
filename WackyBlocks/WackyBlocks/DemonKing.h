#pragma once
#include "Enemy.h"

class DemonKing : public Enemy
{
public:

    DemonKing(const sf::Vector2f& m_position, std::vector<Block>& m_blocks);
    ~DemonKing();

    void update(sf::Time m_deltaTime, Player& m_player) override;
    void render(sf::RenderWindow& m_window) override;

private:

    void loadTextures();
    void loadFrames();
    void setupBoundingBox();

    void updateDemonKingAnimation(sf::Time m_deltaTime, Player& m_player);

    sf::Time m_attackCooldown = sf::seconds(3.0f);
    sf::Time m_timeSinceLastAttack = sf::Time::Zero;
};

