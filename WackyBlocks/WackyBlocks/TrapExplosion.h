#pragma once
#include <SFML/Graphics.hpp>

class TrapExplosion
{
public:

    TrapExplosion(const sf::Vector2f& m_position);

    void update(sf::Time m_deltaTime);
    void render(sf::RenderWindow& m_window);

    bool isFinished() const;
    bool checkPlayerCollision(const sf::FloatRect& m_playerBounds);

private:

    sf::CircleShape m_explosionShape;
    float m_expansionSpeed = 200.0f;
    float m_maxRadius = 150.0f;
    bool m_hasDamagedPlayer = false;
    bool m_isDestroyed = false;

};

