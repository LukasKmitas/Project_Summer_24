#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Coin.h"
#include "Block.h"
#include "SoundManager.h"

class CoinManager
{
public:

    static CoinManager& getInstance()
    {
        static CoinManager instance;
        return instance;
    }

    CoinManager() {}
    CoinManager(const CoinManager&) = delete;
    CoinManager& operator=(const CoinManager&) = delete;

    void update(sf::Time m_deltaTime, std::vector<Block>& m_blocks);
    void render(sf::RenderWindow& m_window);

    void spawnCoins(int m_numCoins, const sf::Vector2f& m_position);
    void handlePlayerCollision(const sf::FloatRect& m_playerBoundingBox, int& m_currency);

private:

    std::vector<Coin> m_coins;

};

