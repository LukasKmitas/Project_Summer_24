#include "ExplosionManager.h"
#include "Enemy.h"
#include "Player.h"

void ExplosionManager::update(sf::Time m_deltaTime, Player& m_player, std::vector<std::unique_ptr<Enemy>>& m_enemies)
{
    for (auto explosionIt = m_explosions.begin(); explosionIt != m_explosions.end();)
    {
        explosionIt->update(m_deltaTime);

        if (explosionIt->checkPlayerCollision(m_player.getBoundingBox()))
        {
            m_player.takeDamage(30);
        }

        for (auto& enemy : m_enemies)
        {
            if (explosionIt->checkPlayerCollision(enemy->getCollisionBoundingBox()))
            {
                enemy->takeDamage(30);
            }
        }

        if (explosionIt->isFinished())
        {
            explosionIt = m_explosions.erase(explosionIt);
        }
        else
        {
            ++explosionIt;
        }
    }
}

void ExplosionManager::render(sf::RenderWindow& m_window)
{
    for (auto& explosion : m_explosions)
    {
        explosion.render(m_window);
    }
}

void ExplosionManager::spawnExplosion(const sf::Vector2f& m_position)
{
	m_explosions.emplace_back(m_position);
}
