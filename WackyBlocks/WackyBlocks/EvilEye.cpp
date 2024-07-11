#include "EvilEye.h"

EvilEye::EvilEye(const sf::Vector2f& m_position)
	: Enemy(m_position)
{
    loadTextures();
	loadFrames();
    m_sprite.setTexture(m_idleTexture);
    if (!m_idleFrames.empty()) 
    {
        m_sprite.setTextureRect(m_idleFrames[0]);
    }
    setState(EnemyState::IDLE);
}

void EvilEye::update(sf::Time m_deltaTime)
{
	updateAnimation(m_deltaTime);
}

void EvilEye::loadTextures()
{
    if (!m_idleTexture.loadFromFile("Assets\\Images\\Enemies\\EvilEye\\Flight.png"))
    {
        std::cout << "problem loading EvilEye idle image" << std::endl;
    }

    if (!m_attack1Texture.loadFromFile("Assets\\Images\\Enemies\\EvilEye\\Attack.png"))
    {
        std::cout << "problem loading player idle image" << std::endl;
    }

    if (!m_deathTexture.loadFromFile("Assets\\Images\\Enemies\\EvilEye\\Death.png"))
    {
        std::cout << "problem loading player idle image" << std::endl;
    }

    if (!m_takeHitTexture.loadFromFile("Assets\\Images\\Enemies\\EvilEye\\TakeHit.png"))
    {
        std::cout << "problem loading player idle image" << std::endl;
    }
}

void EvilEye::loadFrames()
{
    const int frameWidth = 150;
    const int frameHeight = 150;

    for (int i = 0; i < 8; ++i)
    {
        m_idleFrames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }

    for (int i = 0; i < 8; ++i)
    {
        m_attack1Frames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }

    for (int i = 0; i < 4; ++i)
    {
        m_deathFrames.push_back(sf::IntRect(i * frameWidth, 50, frameWidth, frameHeight));
    }

    for (int i = 0; i < 4; ++i)
    {
        m_takeHitFrames.push_back(sf::IntRect(i * frameWidth, 150, frameWidth, frameHeight));
    }

}
