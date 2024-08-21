#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Block.h"

class Coin
{
public:

    Coin(const sf::Vector2f& m_position);
    ~Coin();

    void update(sf::Time m_deltaTime, std::vector<Block>& m_blocks);
    void render(sf::RenderWindow& m_window);

    void destroyCoin();
    bool getIsDestroyed() const;

    sf::FloatRect getGlobalBounds() const;

private:

    void setupCoin();
    void handleCollisionWithGround(sf::Time m_deltaTime, std::vector<Block>& m_blocks);
    void applyGravity(sf::Time m_deltaTime);
    void animate(sf::Time m_deltaTime);

    sf::Sprite m_coinSprite;
    static sf::Texture m_coinTexture;

    sf::Vector2f m_velocity;
    sf::Time m_elapsedTime = sf::Time::Zero;
    bool m_grounded = false;
    bool isDestroyed = false;

    std::vector<sf::IntRect> m_coinFrames;
    sf::Time m_currentFrameTime = sf::Time::Zero;
    sf::Time m_frameTime = sf::seconds(0.1f);
    int m_currentFrame = 0;
};

