#pragma once

#include "Enemy.h"

class CorruptedMushroom : public Enemy
{
public:

    CorruptedMushroom(const sf::Vector2f& m_position, std::vector<Block>& m_blocks);

    void update(sf::Time m_deltaTime, Player& m_player) override;
    void render(sf::RenderWindow& m_window) override;

private:

    void loadTextures();
    void loadFrames();
    void setupBoundingBox();

    void updateMushroomAnimation(sf::Time m_deltaTime, Player& m_player);

};

