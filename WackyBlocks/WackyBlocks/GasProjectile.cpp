#include "GasProjectile.h"

sf::Texture GasProjectile::m_texture;

GasProjectile::GasProjectile(const sf::Vector2f& m_startPosition, const sf::Vector2f& m_direction, float m_getSpeed)
	: m_speed(m_getSpeed)
{
    initGasSprite();
	m_sprite.setPosition(m_startPosition);
	m_velocity = m_direction * m_getSpeed;
	m_elapsedTime = sf::Time::Zero;
}

void GasProjectile::update(sf::Time m_deltaTime, const std::vector<Block>& m_blocks, Player& m_player)
{
    m_elapsedTime += m_deltaTime;

    if (m_elapsedTime.asSeconds() >= m_lifetime)
    {
        m_isDestroyed = true;
        return;
    }

    m_sprite.move(m_velocity * m_deltaTime.asSeconds());

    handleCollision(m_blocks);
    updateAnimation(m_deltaTime);
    checkCollisionWithPlayer(m_player);

    float alpha = 255 * (1.0f - m_elapsedTime.asSeconds() / m_lifetime);
    m_sprite.setColor(sf::Color(0, 255, 0, static_cast<sf::Uint8>(alpha)));
}

void GasProjectile::render(sf::RenderWindow& m_window)
{
	if (!m_isDestroyed)
	{
		m_window.draw(m_sprite);
	}
}

bool GasProjectile::isDestroyed() const
{
	return m_isDestroyed;
}

void GasProjectile::initGasSprite()
{
    if (m_texture.getSize().x == 0)
    {
        if (!m_texture.loadFromFile("Assets\\Images\\Enemies\\Mushroom\\Smoke15Frames.png"))
        {
            std::cout << "Failed to load Smoke15Frames.png" << std::endl;
        }
    }
    m_sprite.setTexture(m_texture);
    m_sprite.setScale(0.2f, 0.2f);
    m_frameRect = sf::IntRect(0, 0, 256, 256);
    m_sprite.setTextureRect(m_frameRect);
    m_sprite.setOrigin(m_sprite.getGlobalBounds().width / 2, m_sprite.getGlobalBounds().height / 2);
    m_sprite.setColor(sf::Color(0, 255, 0, 255));
}

void GasProjectile::handleCollision(const std::vector<Block>& m_blocks)
{
    for (const auto& block : m_blocks)
    {
        if (!block.traversable && block.shape.getGlobalBounds().intersects(m_sprite.getGlobalBounds()))
        {
            if (m_velocity.x != 0)
            {
                m_velocity.x = -m_velocity.x;
            }
            if (m_velocity.y != 0)
            {
                m_velocity.y = -m_velocity.y;
            }
        }
    }
}

void GasProjectile::updateAnimation(sf::Time m_deltaTime)
{
    m_currentFrameTime += m_deltaTime;

    if (m_currentFrameTime >= m_frameTime)
    {
        m_currentFrameTime = sf::Time::Zero;
        m_currentFrame++;

        // 5x3 grid
        if (m_currentFrame < 15)
        {
            int col = m_currentFrame % 5;  // Column (0-4)
            int row = m_currentFrame / 5;  // Row (0-2)

            m_frameRect.left = col * 256;
            m_frameRect.top = row * 256;

            m_sprite.setTextureRect(m_frameRect);
        }
        else
        {
            m_currentFrame = 0;
        }
    }
}

void GasProjectile::checkCollisionWithPlayer(Player& m_player)
{
    if (m_sprite.getGlobalBounds().intersects(m_player.getBoundingBox()))
    {
        m_player.takeDamage(0.003f);
    }
}
