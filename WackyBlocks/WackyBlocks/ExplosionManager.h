#pragma once
#include <SFML/Graphics.hpp>
#include "TrapExplosion.h"
#include <vector>

class Enemy;
class Player;

class ExplosionManager
{
public:

    static ExplosionManager& getInstance()
    {
        static ExplosionManager instance;
        return instance;
    }

    ExplosionManager() {}
    ExplosionManager(const ExplosionManager&) = delete;
    ExplosionManager& operator=(const ExplosionManager&) = delete;

    void update(sf::Time m_deltaTime, Player& m_player, std::vector<std::unique_ptr<Enemy>>& m_enemies);
    void render(sf::RenderWindow& m_window);

    void spawnExplosion(const sf::Vector2f& m_position);

private:

    std::vector<TrapExplosion> m_explosions;

};

