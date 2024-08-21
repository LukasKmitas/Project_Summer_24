#include "CoinManager.h"

void CoinManager::update(sf::Time m_deltaTime, std::vector<Block>& m_blocks)
{
    for (auto& coin : m_coins) 
    {
        coin.update(m_deltaTime, m_blocks);
    }

    m_coins.erase(std::remove_if(m_coins.begin(), m_coins.end(),
        [](const Coin& coin) { return coin.getIsDestroyed(); }), m_coins.end());
}

void CoinManager::render(sf::RenderWindow& m_window)
{
    for (auto& coin : m_coins)
    {
        coin.render(m_window);
    }
}

void CoinManager::spawnCoins(int m_numCoins, const sf::Vector2f& m_position)
{
    for (int i = 0; i < m_numCoins; ++i)
    {
        m_coins.emplace_back(m_position);
    }
}

void CoinManager::handlePlayerCollision(const sf::FloatRect& m_playerBoundingBox, int& m_currency)
{
    for (auto& coin : m_coins)
    {
        if (!coin.getIsDestroyed() && m_playerBoundingBox.intersects(coin.getGlobalBounds()))
        {
            m_currency += 1;
            //std::cout << "Coin collected, Currency: " << m_currency << std::endl;
            coin.destroyCoin();
        }
    }
}
