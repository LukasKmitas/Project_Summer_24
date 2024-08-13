#include "EnemyProjectile.h"

EnemyProjectile::EnemyProjectile(const sf::Vector2f& m_position, const sf::Vector2f& m_getDirection, const sf::Texture& m_texture, const std::vector<sf::IntRect>& m_getFrames)
	: m_direction(m_getDirection), m_frames(m_getFrames)
{
    m_sprite.setTexture(m_texture);
    m_sprite.setTextureRect(m_frames[m_currentFrame]);
    m_sprite.setPosition(m_position);
    m_sprite.setOrigin(m_frames[0].width / 2.0f, m_frames[0].height / 2.0f);

    m_frameTime = sf::seconds(0.1f);
    m_currentFrameTime = sf::Time::Zero;
}

void EnemyProjectile::update(sf::Time m_deltaTime, const std::vector<Block>& blocks)
{
    if (!m_isDestroyed)
    {
        m_currentFrameTime += m_deltaTime;

        if (!m_isInDestructionSequence)
        {
            m_sprite.move(m_direction * m_speed * m_deltaTime.asSeconds());

            for (const auto& block : blocks)
            {
                if (!block.traversable && block.shape.getGlobalBounds().intersects(getBoundingBox()))
                {
                    m_isInDestructionSequence = true;
                    m_currentFrame = m_destructionStartFrame;
                    break;
                }
            }

            // bullet form
            if (!m_isInDestructionSequence && m_currentFrameTime >= m_frameTime)
            {
                m_currentFrameTime = sf::Time::Zero;
                m_currentFrame = (m_currentFrame + 1) % 3;
                m_sprite.setTextureRect(m_frames[m_currentFrame]);
            }
        }
        else
        {
            // start the destruction of projectile
            if (m_currentFrameTime >= m_frameTime)
            {
                m_currentFrameTime = sf::Time::Zero;

                if (m_currentFrame < m_frames.size() - 1)
                {
                    m_currentFrame++;
                    m_sprite.setTextureRect(m_frames[m_currentFrame]);
                }
                else
                {
                    m_isDestroyed = true;
                }
            }
        }
    }
}

void EnemyProjectile::render(sf::RenderWindow& m_window)
{
    if (!m_isDestroyed)
    {
        m_window.draw(m_sprite);
    }
}

bool EnemyProjectile::isDestroyed() const
{
    return m_isDestroyed;
}

sf::FloatRect EnemyProjectile::getBoundingBox() const
{
    return m_sprite.getGlobalBounds();
}

bool EnemyProjectile::checkCollisionWithPlayer(Player& m_player)
{
    if (!m_isInDestructionSequence && getBoundingBox().intersects(m_player.getBoundingBox()))
    {
        m_player.takeDamage(10);
        m_isInDestructionSequence = true;
        m_currentFrame = m_destructionStartFrame;
        return true;
    }
    return false;
}
