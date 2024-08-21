#include "CorruptedMushroom.h"

CorruptedMushroom::CorruptedMushroom(const sf::Vector2f& m_position, std::vector<Block>& m_blocks)
	: Enemy(m_position, m_blocks)
{
	loadTextures();
	loadFrames();
    m_sprite.setTexture(m_idleTexture);
    if (!m_idleFrames.empty())
    {
        m_sprite.setTextureRect(m_idleFrames[0]);
    }
    m_sprite.setOrigin(m_sprite.getGlobalBounds().width / 2.0f, m_sprite.getGlobalBounds().height / 2.0f);
    setupBoundingBox();
    setState(EnemyState::PATROL);
    m_health = 100;
    m_speed = 13;
    m_visionRange = 80.0f;
    m_visionAngle = 110.0f;
    m_spriteOffSet = sf::Vector2f(0, 65);
    m_gravityBoxOffSet = sf::Vector2f(0, 24);
    m_numCoins = 7;
}

CorruptedMushroom::~CorruptedMushroom()
{
}

void CorruptedMushroom::update(sf::Time m_deltaTime, Player& m_player)
{
    if (!m_isDead)
    {
        Enemy::update(m_deltaTime, m_player);

        applyGravity(m_deltaTime);

        if (m_isIdling && m_animationState == EnemyState::IDLE)
        {
            m_timeIdleElapsed += m_deltaTime;
            if (m_timeIdleElapsed >= m_idleTime)
            {
                m_isIdling = false;
                m_timeIdleElapsed = sf::Time::Zero;
                m_movingDirection = !m_movingDirection;
                setState(EnemyState::PATROL);
            }
        }

        if (m_animationState == EnemyState::ATTACK1 || m_animationState == EnemyState::ATTACK2 || m_animationState == EnemyState::ATTACK3)
        {
            m_velocity.x = 0;
        }

        if (m_animationState == EnemyState::PATROL)
        {
            patrol(m_deltaTime);
        }

        m_timeSinceLastAttack += m_deltaTime;
        m_timeSinceLastGasAttack += m_deltaTime;

        if (m_timeSinceLastAttack >= m_attackCooldown && isPlayerInVisionCone(m_player) && !m_isAttacking)
        {
            if (static_cast<float>(m_health) / m_maxHealth < 0.4f && m_timeSinceLastGasAttack >= m_gasAttackCooldown)
            {
                setState(EnemyState::ATTACK3);
                m_isAttacking = true;
                m_timeSinceLastAttack = sf::Time::Zero;
                m_timeSinceLastGasAttack = sf::Time::Zero;  // Reset gas attack cooldown
            }
            else
            {
                int randomAttack = std::rand() % 2;

                if (randomAttack == 0)
                {
                    setState(EnemyState::ATTACK1);
                }
                else
                {
                    setState(EnemyState::ATTACK2);
                }

                m_isAttacking = true;
                m_timeSinceLastAttack = sf::Time::Zero;
            }
        }
        m_attackRange = m_collisionBox.getGlobalBounds();
        m_attackRange.left += (m_movingDirection ? 30.0f : -30.0f);
        m_attackDebugRect.setSize(sf::Vector2f(m_attackRange.width, m_attackRange.height));
        m_attackDebugRect.setPosition(m_attackRange.left, m_attackRange.top);
    }
    m_collisionBox.setPosition(m_sprite.getPosition());
    m_gravityBox.setPosition(m_sprite.getPosition() + m_gravityBoxOffSet);
    updateMushroomAnimation(m_deltaTime, m_player);

    for (auto& projectile : m_gasProjectiles)
    {
        projectile.update(m_deltaTime, m_gameBlocks, m_player);
    }

    m_gasProjectiles.erase(std::remove_if(m_gasProjectiles.begin(), m_gasProjectiles.end(),
        [](const GasProjectile& proj) { return proj.isDestroyed(); }), m_gasProjectiles.end());

}

void CorruptedMushroom::render(sf::RenderWindow& m_window)
{
    m_window.draw(m_sprite);

    if (!m_isDead)
    {
        Enemy::render(m_window);

        if (m_animationState == EnemyState::ATTACK1 || m_animationState == EnemyState::ATTACK2 && m_currentFrame == 6)
        {
            m_window.draw(m_attackDebugRect);
        }
    }

    for (auto& projectile : m_gasProjectiles)
    {
        projectile.render(m_window);
    }
}

void CorruptedMushroom::loadTextures()
{
    if (!m_idleTexture.loadFromFile("Assets\\Images\\Enemies\\Mushroom\\Idle.png"))
    {
        std::cout << "problem loading Mushroom idle image" << std::endl;
    }

    if (!m_walkTexture.loadFromFile("Assets\\Images\\Enemies\\Mushroom\\Run.png"))
    {
        std::cout << "problem loading Mushroom walk image" << std::endl;
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

    m_gravityBox.setSize(sf::Vector2f(40, 5));
    m_gravityBox.setOrigin(m_gravityBox.getSize() / 2.0f);
    m_gravityBox.setPosition(m_sprite.getPosition() + m_gravityBoxOffSet);
    m_gravityBox.setFillColor(sf::Color::Transparent);
    m_gravityBox.setOutlineColor(sf::Color::Magenta);
    m_gravityBox.setOutlineThickness(1.0f);

    m_groundDetectionDebug.setSize(sf::Vector2f(40, 40));
    m_groundDetectionDebug.setFillColor(sf::Color(0, 255, 0, 100));

    m_attackDebugRect.setFillColor(sf::Color(255, 0, 0, 100));
}

void CorruptedMushroom::updateMushroomAnimation(sf::Time m_deltaTime, Player& m_player)
{
    if (m_animationState == EnemyState::ATTACK1 || m_animationState == EnemyState::ATTACK2)
    {
        m_currentFrameTime += m_deltaTime;
        if (m_currentFrameTime >= m_frameTime)
        {
            m_currentFrameTime = sf::Time::Zero;

            if (m_animationState == EnemyState::ATTACK1)
            {
                m_currentFrame = (m_currentFrame + 1) % m_attack1Frames.size();
                m_sprite.setTexture(m_attack1Texture);
                m_sprite.setTextureRect(m_attack1Frames[m_currentFrame]);

                if (m_currentFrame == 6)
                {
                    if (m_attackRange.intersects(m_player.getBoundingBox()))
                    {
                        m_player.takeDamage(6);
                    }
                }
            }
            else if (m_animationState == EnemyState::ATTACK2)
            {
                m_currentFrame = (m_currentFrame + 1) % m_attack2Frames.size();
                m_sprite.setTexture(m_attack2Texture);
                m_sprite.setTextureRect(m_attack2Frames[m_currentFrame]);

                if (m_currentFrame == 6)
                {
                    if (m_attackRange.intersects(m_player.getBoundingBox()))
                    {
                        m_player.takeDamage(7);
                    }
                }
            }

            if (m_currentFrame == m_attack1Frames.size() - 1 || m_currentFrame == m_attack2Frames.size() - 1)
            {
                m_isAttacking = false;
                m_isIdling = true;
                setState(EnemyState::IDLE);
                m_timeIdleElapsed = sf::Time::Zero;
            }
        }
    }
    else if (m_animationState == EnemyState::ATTACK3)
    {
        m_currentFrameTime += m_deltaTime;
        if (m_currentFrameTime >= m_frameTime)
        {
            m_currentFrameTime = sf::Time::Zero;
            m_currentFrame = (m_currentFrame + 1) % m_attack3Frames.size();
            m_sprite.setTexture(m_attack3Texture);
            m_sprite.setTextureRect(m_attack3Frames[m_currentFrame]);

            if (m_currentFrame == 9)
            {
                spawnGasProjectiles();
            }

            if (m_currentFrame == m_attack3Frames.size() - 1)
            {
                m_isAttacking = false;
                setState(EnemyState::IDLE);
                m_timeIdleElapsed = sf::Time::Zero;
            }
        }
    }
    else
    {
        Enemy::updateAnimation(m_deltaTime);
    }
}

void CorruptedMushroom::spawnGasProjectiles()
{
    const int numProjectiles = 50;
    const float baseSpeed = 5.0f;
    const float speedVariance = 10.0f;

    for (int i = 0; i < numProjectiles; ++i)
    {
        float angle = i * (360.0f / numProjectiles);
        float radians = angle * 3.14159265f / 180.0f;

        sf::Vector2f direction(std::cos(radians), std::sin(radians));
        float randomSpeed = baseSpeed + static_cast<float>(std::rand() % static_cast<int>(speedVariance));

        m_gasProjectiles.emplace_back(m_collisionBox.getPosition() - sf::Vector2f(20, 40), direction, randomSpeed);
    }
}
