#include "TrapSpikes.h"

TrapSpikes::TrapSpikes(const sf::Vector2f& m_position)
{
    setupSpikes();
    m_sprite.setPosition(m_position + sf::Vector2f(0, 10));
    m_sprite.setOrigin(m_sprite.getGlobalBounds().width / 2, m_sprite.getGlobalBounds().height / 2);
    m_hitbox = m_sprite.getGlobalBounds();
}

void TrapSpikes::update(sf::Time m_deltaTime, Player& m_player, std::vector<std::unique_ptr<Enemy>>& m_enemies)
{
    if (m_isAnimating)
    {
        if (m_animationClock.getElapsedTime() >= m_frameTime)
        {
            m_animationClock.restart();
            m_currentFrame = (m_currentFrame + 1) % m_frames.size();
            m_sprite.setTextureRect(m_frames[m_currentFrame]);

            if (m_currentFrame == m_frames.size() - 1 && !m_dealtDamage)
            {
                if (m_hitbox.intersects(m_player.getBoundingBox()))
                {
                    m_player.takeDamage(m_damage);
                    m_dealtDamage = true;
                }

                for (auto& enemy : m_enemies)
                {
                    if (m_hitbox.intersects(enemy->getCollisionBoundingBox()))
                    {
                        enemy->takeDamage(m_damage);
                    }
                }
            }

            if (m_currentFrame == 0)
            {
                m_dealtDamage = false;
                m_isAnimating = false;
            }
        }
    }
    else
    {
        m_currentDelayTime += m_deltaTime;
        if (m_currentDelayTime >= m_delayTime)
        {
            m_isAnimating = true;
            m_currentDelayTime = sf::Time::Zero;
        }
    }

    m_hitbox = m_sprite.getGlobalBounds();
}

void TrapSpikes::render(sf::RenderWindow& m_window)
{
    m_window.draw(m_sprite);
}

sf::FloatRect TrapSpikes::getBoundingBox() const
{
    return m_hitbox;
}

void TrapSpikes::setupSpikes()
{
    if (!m_texture.loadFromFile("Assets\\Images\\World\\spikes.png"))
    {
        std::cout << "Error - loading spikes image" << std::endl;
    }
    m_sprite.setTexture(m_texture);

    const int frameWidth = 64;
    const int frameHeight = 64;

    for (int i = 0; i < 4; ++i)
    {
        m_frames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }
    m_sprite.setTextureRect(m_frames[0]);
}

