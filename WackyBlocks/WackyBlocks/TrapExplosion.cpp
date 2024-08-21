#include "TrapExplosion.h"

TrapExplosion::TrapExplosion(const sf::Vector2f& m_position)
{
    m_explosionShape.setRadius(0.0f);
    m_explosionShape.setOrigin(0.0f, 0.0f);
    m_explosionShape.setPosition(m_position);
    m_explosionShape.setFillColor(sf::Color(255, 0, 0, 130));
}

void TrapExplosion::update(sf::Time m_deltaTime)
{
    float expansion = m_expansionSpeed * m_deltaTime.asSeconds();
    float newRadius = m_explosionShape.getRadius() + expansion;

    if (newRadius <= m_maxRadius)
    {
        m_explosionShape.setRadius(newRadius);
        m_explosionShape.setOrigin(newRadius, newRadius);
    }
    else
    {
        m_isDestroyed = true;
    }
}

void TrapExplosion::render(sf::RenderWindow& m_window)
{
    if (!m_isDestroyed) 
    {
        m_window.draw(m_explosionShape);
    }
}

bool TrapExplosion::isFinished() const
{
    return m_isDestroyed;
}

bool TrapExplosion::checkPlayerCollision(const sf::FloatRect& m_playerBounds)
{
    if (!m_hasDamagedPlayer && m_explosionShape.getGlobalBounds().intersects(m_playerBounds))
    {
        m_hasDamagedPlayer = true;
        return true;
    }
    return false;
}
