#include "Coin.h"

sf::Texture Coin::m_coinTexture;

Coin::Coin(const sf::Vector2f& m_position)
{
	setupCoin();
	m_coinSprite.setPosition(m_position);
	m_velocity.x = static_cast<float>((rand() % 60 - 40));
	m_velocity.y = static_cast<float>(-(rand() % 200 + 100));
}

Coin::~Coin()
{
}

void Coin::update(sf::Time m_deltaTime, std::vector<Block>& m_blocks)
{
	if (!isDestroyed)
	{
		if (!m_grounded)
		{
			applyGravity(m_deltaTime);
		}

		handleCollisionWithGround(m_deltaTime, m_blocks);
		animate(m_deltaTime);
	}
}

void Coin::render(sf::RenderWindow& m_window)
{
	if (!isDestroyed)
	{
		m_window.draw(m_coinSprite);
	}
}

void Coin::destroyCoin()
{
	isDestroyed = true;
}

bool Coin::getIsDestroyed() const
{
	return isDestroyed;
}

sf::FloatRect Coin::getGlobalBounds() const
{
	return m_coinSprite.getGlobalBounds();
}

void Coin::setupCoin()
{
	if (!m_coinTexture.loadFromFile("Assets\\Images\\World\\Coins.png"))
	{
		std::cout << "problem loading Coin image" << std::endl;
	}
	m_coinSprite.setTexture(m_coinTexture);
	for (int i = 0; i < 8; ++i)
	{
		m_coinFrames.push_back(sf::IntRect(i * 15.875f, 0, 15.875f, 16));
	}
	m_coinSprite.setTextureRect(m_coinFrames[m_currentFrame]);
}

void Coin::handleCollisionWithGround(sf::Time m_deltaTime, std::vector<Block>& m_blocks)
{
	bool wasOnGround = m_grounded;
	m_grounded = false;

	for (auto& block : m_blocks)
	{
		if (!block.traversable && block.shape.getGlobalBounds().intersects(m_coinSprite.getGlobalBounds()))
		{
			if (m_velocity.y > 0)
			{
				m_velocity.y = -m_velocity.y * 0.5f;
			}
			if (abs(m_velocity.y) < 50.0f)
			{
				m_grounded = true;
				m_velocity.x = 0.0f;
				m_velocity.y = 0.0f;
				break;
			}
		}
	}

	if (wasOnGround && !m_grounded)
	{
		m_velocity.y += 500.0f * m_deltaTime.asSeconds();
	}
}

void Coin::applyGravity(sf::Time m_deltaTime)
{
	m_velocity.y += 500.0f * m_deltaTime.asSeconds();
	m_coinSprite.move(m_velocity * m_deltaTime.asSeconds());
}

void Coin::animate(sf::Time m_deltaTime)
{
	m_currentFrameTime += m_deltaTime;
	if (m_currentFrameTime >= m_frameTime)
	{
		m_currentFrameTime = sf::Time::Zero;
		m_currentFrame = (m_currentFrame + 1) % m_coinFrames.size();
		m_coinSprite.setTextureRect(m_coinFrames[m_currentFrame]);
	}
}
