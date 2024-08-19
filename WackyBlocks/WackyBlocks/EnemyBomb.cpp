#include "EnemyBomb.h"

EnemyBomb::EnemyBomb(const sf::Vector2f& m_startPosition, const sf::Vector2f& m_targetPosition)
{
    if (!m_texture.loadFromFile("Assets\\Images\\Enemies\\Goblin\\Bomb_sprite.png"))
    {
        std::cout << "Error - loading bomb sprite" << std::endl;
    }

    setupBomb();
    m_sprite.setPosition(m_startPosition);
    m_collisionCircle.setPosition(m_startPosition);

    float xDistance = m_targetPosition.x - m_startPosition.x;
    float yDistance = m_targetPosition.y - m_startPosition.y;

    m_velocity.x = xDistance / 1.5f;
    m_velocity.y = -300.0f;
}

void EnemyBomb::update(sf::Time m_deltaTime, const std::vector<Block>& m_blocks)
{
    if (!m_hasExploded)
    {
        m_timeSinceLanded += m_deltaTime;

        if (m_timeSinceLanded >= m_timeToExplode)
        {
            m_hasExploded = true;
            m_currentFrame = 0;
            m_explosionHitbox.setPosition(m_sprite.getPosition());
        }

        m_currentFrameTime += m_deltaTime;

        if (m_currentFrameTime >= m_frameTime)
        {
            m_currentFrameTime = sf::Time::Zero;
            m_currentFrame = (m_currentFrame + 1) % 3;
            m_sprite.setTextureRect(m_bombFrames[m_currentFrame]);
        }

        if (!m_isRolling)
        {
            m_velocity.y += m_gravity * m_deltaTime.asSeconds();
            m_sprite.move(m_velocity * m_deltaTime.asSeconds());
            m_collisionCircle.setPosition(m_sprite.getPosition());

            for (const auto& block : m_blocks)
            {
                if (!block.traversable && block.shape.getGlobalBounds().intersects(m_collisionCircle.getGlobalBounds()))
                {
                    sf::FloatRect bombBounds = m_collisionCircle.getGlobalBounds();
                    sf::FloatRect blockBounds = block.shape.getGlobalBounds();

                    float overlapLeft = bombBounds.left + bombBounds.width - blockBounds.left;
                    float overlapRight = blockBounds.left + blockBounds.width - bombBounds.left;
                    float overlapTop = bombBounds.top + bombBounds.height - blockBounds.top;
                    float overlapBottom = blockBounds.top + blockBounds.height - bombBounds.top;

                    if (std::min(overlapLeft, overlapRight) < std::min(overlapTop, overlapBottom))
                    {
                        m_velocity.x = -m_velocity.x * 0.3f;
                        if (overlapLeft < overlapRight)
                        {
                            m_sprite.move(-overlapLeft, 0);
                        }
                        else
                        {
                            m_sprite.move(overlapRight, 0);
                        }
                    }
                    else
                    {
                        m_velocity.y = -m_velocity.y * 0.3f;

                        if (overlapTop < overlapBottom)
                        {
                            m_sprite.move(0, -overlapTop);
                        }
                        else
                        {
                            m_sprite.move(0, overlapBottom);
                        }

                        if (std::abs(m_velocity.y) < 50.0f)
                        {
                            m_isRolling = true;
                            m_velocity.y = 0.0f;
                        }
                    }

                    break;
                }
            }
        }
        else
        {
            m_sprite.move(m_velocity * m_deltaTime.asSeconds());
            m_collisionCircle.setPosition(m_sprite.getPosition());

            m_velocity.x *= 0.9f;

            for (const auto& block : m_blocks)
            {
                if (!block.traversable && block.shape.getGlobalBounds().intersects(m_collisionCircle.getGlobalBounds()))
                {
                    m_velocity.x = -m_velocity.x * 0.3f;
                    break;
                }
            }
        }
    }
    else
    {
        m_currentFrameTime += m_deltaTime;
        if (m_currentFrameTime >= m_frameTime)
        {
            m_currentFrameTime = sf::Time::Zero;

            if (m_currentFrame < m_explosionFrames.size())
            {
                m_sprite.setTextureRect(m_explosionFrames[m_currentFrame]);
                m_currentFrame++;

                if (m_currentFrame >= 10)
                {
                    float scaleFactor = 1.0f + 0.2f * (m_currentFrame - 10);
                    m_sprite.setScale(scaleFactor, scaleFactor);
                }

                if (m_currentFrame >= 10 && m_currentFrame <= 16)
                {
                    float currentRadius = m_explosionHitbox.getRadius();
                    float newRadius = currentRadius + 200.0f * m_deltaTime.asSeconds();
                    m_explosionHitbox.setRadius(newRadius);
                    m_explosionHitbox.setOrigin(newRadius, newRadius);
                    m_explosionHitbox.setPosition(m_sprite.getPosition());
                }
                if (m_currentFrame >= 17)
                {
                    m_explosionHitbox.setRadius(0.0f);
                }
            }
            else
            {
                m_isDestroyed = true;
            }
        }
    }
}

void EnemyBomb::render(sf::RenderWindow& m_window)
{
    if (!m_isDestroyed)
    {
        m_window.draw(m_sprite);
        m_window.draw(m_collisionCircle);
        m_window.draw(m_explosionHitbox);
    }
}

void EnemyBomb::markPlayerDamaged()
{
    m_hasDamagedPlayer = true;
}

bool EnemyBomb::isDestroyed() const
{
	return m_isDestroyed;
}

bool EnemyBomb::hasDamagedPlayer() const
{
    return m_hasDamagedPlayer;
}

sf::FloatRect EnemyBomb::getExplosionHitbox() const
{
	return m_explosionHitbox.getGlobalBounds();
}

void EnemyBomb::setupBomb()
{
    m_sprite.setTexture(m_texture);

    m_collisionCircle.setRadius(7.0f);
    m_collisionCircle.setFillColor(sf::Color::Transparent);
    m_collisionCircle.setOutlineColor(sf::Color::Yellow);
    m_collisionCircle.setOutlineThickness(1);
    m_collisionCircle.setOrigin(m_collisionCircle.getRadius(), m_collisionCircle.getRadius());

    m_explosionHitbox.setRadius(0.0f);
    m_explosionHitbox.setFillColor(sf::Color::Transparent);
    m_explosionHitbox.setOutlineColor(sf::Color::Red);
    m_explosionHitbox.setOutlineThickness(1);
    m_explosionHitbox.setOrigin(m_explosionHitbox.getRadius(), m_explosionHitbox.getRadius());

    for (int i = 0; i < 3; ++i) // Bomb idle
    {
        m_bombFrames.push_back(sf::IntRect(i * 100, 0, 100, 100));
    }

    for (int i = 0; i < 19; ++i) // Explosion frames
    {
        m_explosionFrames.push_back(sf::IntRect(i * 100, 0, 100, 100));
    }

    m_sprite.setTextureRect(m_bombFrames[0]);
    m_sprite.setOrigin(m_sprite.getGlobalBounds().width / 2, m_sprite.getGlobalBounds().height / 2);
}
