#include "CorruptedMushroom.h"

CorruptedMushroom::CorruptedMushroom(const sf::Vector2f& m_position, std::vector<Block>& m_blocks)
	: Enemy(m_position, m_blocks)
{
	loadTextures();
	loadFrames();
    m_sprite.setTexture(m_walkTexture);
    if (!m_walkFrames.empty())
    {
        m_sprite.setTextureRect(m_walkFrames[0]);
    }
    m_sprite.setOrigin(m_sprite.getGlobalBounds().width / 2.0f, m_sprite.getGlobalBounds().height / 2.0f);
    setupBoundingBox();
    setState(EnemyState::IDLE);
    m_health = 100;
    m_speed = 20;
    m_visionRange = 200.0f;
    m_visionAngle = 60.0f;
}

void CorruptedMushroom::update(sf::Time m_deltaTime, Player& m_player)
{
    if (!m_isDead)
    {
        Enemy::update(m_deltaTime, m_player);

    }
    m_collisionBox.setPosition(m_sprite.getPosition());
    updateMushroomAnimation(m_deltaTime, m_player);
}

void CorruptedMushroom::render(sf::RenderWindow& m_window)
{
    m_window.draw(m_sprite);

    if (!m_isDead)
    {
        Enemy::render(m_window);
    }
}

void CorruptedMushroom::loadTextures()
{
    if (!m_idleTexture.loadFromFile("Assets\\Images\\Enemies\\Mushroom\\Idle.png"))
    {
        std::cout << "problem loading Mushroom walk(fly) image" << std::endl;
    }

    if (!m_walkTexture.loadFromFile("Assets\\Images\\Enemies\\Mushroom\\Run.png"))
    {
        std::cout << "problem loading Mushroom walk(fly) image" << std::endl;
    }

    if (!m_attack1Texture.loadFromFile("Assets\\Images\\Enemies\\Mushroom\\Attack.png"))
    {
        std::cout << "problem loading Mushroom attack1 image" << std::endl;
    }

    if (!m_attack2Texture.loadFromFile("Assets\\Images\\Enemies\\Mushroom\\Attack2.png"))
    {
        std::cout << "problem loading Mushroom attack2 image" << std::endl;
    }

    if (!m_attack3Texture.loadFromFile("Assets\\Images\\Enemies\\Mushroom\\Attack3.png"))
    {
        std::cout << "problem loading Mushroom attack3 image" << std::endl;
    }

    if (!m_projectileTexture.loadFromFile("Assets\\Images\\Enemies\\Mushroom\\projectile_sprite.png"))
    {
        std::cout << "problem loading Mushroom projectile image" << std::endl;
    }

    if (!m_deathTexture.loadFromFile("Assets\\Images\\Enemies\\Mushroom\\Death.png"))
    {
        std::cout << "problem loading Mushroom death image" << std::endl;
    }

    if (!m_takeHitTexture.loadFromFile("Assets\\Images\\Enemies\\Mushroom\\TakeHit.png"))
    {
        std::cout << "problem loading Mushroom take hit image" << std::endl;
    }
}

void CorruptedMushroom::loadFrames()
{
    const int frameWidth = 150;
    const int frameHeight = 150;
    const int bulletFrameWidth = 50;
    const int bulletFrameHeight = 50;

    for (int i = 0; i < 4; ++i)
    {
        m_idleFrames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }

    for (int i = 0; i < 8; ++i)
    {
        m_walkFrames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }

    for (int i = 0; i < 8; ++i)
    {
        m_attack1Frames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }

    for (int i = 0; i < 8; ++i)
    {
        m_attack2Frames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }

    for (int i = 0; i < 11; ++i)
    {
        m_attack3Frames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }

    for (int i = 0; i < 8; ++i)
    {
        m_projectileFrames.push_back(sf::IntRect(i * bulletFrameWidth, 0, bulletFrameWidth, bulletFrameHeight));
    }

    for (int i = 0; i < 4; ++i)
    {
        m_deathFrames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }

    for (int i = 0; i < 4; ++i)
    {
        m_takeHitFrames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }
}

void CorruptedMushroom::setupBoundingBox()
{
    m_collisionBox.setSize(sf::Vector2f(40, 40));
    m_collisionBox.setOrigin(m_collisionBox.getSize() / 2.0f);
    m_collisionBox.setPosition(m_sprite.getPosition());
    m_collisionBox.setFillColor(sf::Color::Transparent);
    m_collisionBox.setOutlineColor(sf::Color::Red);
    m_collisionBox.setOutlineThickness(1.0f);
}

void CorruptedMushroom::updateMushroomAnimation(sf::Time m_deltaTime, Player& m_player)
{
}
