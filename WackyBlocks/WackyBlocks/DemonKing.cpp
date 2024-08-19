#include "DemonKing.h"

DemonKing::DemonKing(const sf::Vector2f& m_position, std::vector<Block>& m_blocks)
    : Enemy(m_position, m_blocks)
{
    m_movingDirection = false;
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
    m_speed = 20;
    m_visionRange = 200.0f;
    m_visionAngle = 100.0f;
    m_spriteOffSet = sf::Vector2f(0, 117);
    m_collisionBoxOffSet = sf::Vector2f(0, 30);
    m_gravityBoxOffSet = sf::Vector2f(0, 74);
}

DemonKing::~DemonKing()
{
}

void DemonKing::update(sf::Time m_deltaTime, Player& m_player)
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

        if (m_animationState == EnemyState::ATTACK1 ||
            m_animationState == EnemyState::ATTACK2 ||
            m_animationState == EnemyState::ATTACK3)
        {
            m_velocity.x = 0;
        }

        if (m_animationState == EnemyState::PATROL)
        {
            patrol(m_deltaTime);
        }

        m_timeSinceLastAttack += m_deltaTime;
        m_attackRange = m_collisionBox.getGlobalBounds();
        m_attackRange.left += (m_movingDirection ? 60.0f : -60.0f);
        m_attackDebugRect.setSize(sf::Vector2f(m_attackRange.width, m_attackRange.height));
        m_attackDebugRect.setPosition(m_attackRange.left, m_attackRange.top);

        if (m_timeSinceLastAttack >= m_attackCooldown && m_attackDebugRect.getGlobalBounds().intersects(m_player.getBoundingBox()) && !m_isAttacking)
        {
            int randomAttack = 1;

            if (randomAttack == 1)
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
    m_collisionBox.setPosition(m_sprite.getPosition() + m_collisionBoxOffSet);
    m_gravityBox.setPosition(m_sprite.getPosition() + m_gravityBoxOffSet);
    updateDemonKingAnimation(m_deltaTime, m_player);
}

void DemonKing::render(sf::RenderWindow& m_window)
{
    m_window.draw(m_sprite);

    if (!m_isDead)
    {
        Enemy::render(m_window);

        if (m_animationState == EnemyState::ATTACK1 && m_currentFrame == 10)
        {
            m_window.draw(m_attackDebugRect);
        }
    }
}

void DemonKing::loadTextures()
{
    if (!m_idleTexture.loadFromFile("Assets\\Images\\Enemies\\DemonBoss\\demon_slime_288x160.png"))
    {
        std::cout << "problem loading DemonBoss Idle image" << std::endl;
    }

    if (!m_walkTexture.loadFromFile("Assets\\Images\\Enemies\\DemonBoss\\demon_slime_288x160.png"))
    {
        std::cout << "problem loading DemonBoss walk image" << std::endl;
    }

    if (!m_attack1Texture.loadFromFile("Assets\\Images\\Enemies\\DemonBoss\\demon_slime_288x160.png"))
    {
        std::cout << "problem loading DemonBoss attack1 image" << std::endl;
    }

    if (!m_deathTexture.loadFromFile("Assets\\Images\\Enemies\\DemonBoss\\demon_slime_288x160.png"))
    {
        std::cout << "problem loading DemonBoss death image" << std::endl;
    }

    if (!m_takeHitTexture.loadFromFile("Assets\\Images\\Enemies\\DemonBoss\\demon_slime_288x160.png"))
    {
        std::cout << "problem loading DemonBoss take hit image" << std::endl;
    }
}

void DemonKing::loadFrames()
{
    const int frameWidth = 288;
    const int frameHeight = 160;

    for (int i = 0; i < 6; ++i)
    {
        m_idleFrames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }

    for (int i = 0; i < 12; ++i)
    {
        m_walkFrames.push_back(sf::IntRect(i * frameWidth, 160, frameWidth, frameHeight));
    }

    for (int i = 0; i < 15; ++i)
    {
        m_attack1Frames.push_back(sf::IntRect(i * frameWidth, 320, frameWidth, frameHeight));
    }

    for (int i = 0; i < 22; ++i)
    {
        m_deathFrames.push_back(sf::IntRect(i * frameWidth, 640, frameWidth, frameHeight));
    }

    for (int i = 0; i < 5; ++i)
    {
        m_takeHitFrames.push_back(sf::IntRect(i * frameWidth, 480, frameWidth, frameHeight));
    }
}

void DemonKing::setupBoundingBox()
{
    m_collisionBox.setSize(sf::Vector2f(80, 80));
    m_collisionBox.setOrigin(m_collisionBox.getSize() / 2.0f);
    m_collisionBox.setPosition(m_sprite.getPosition() + m_collisionBoxOffSet);
    m_collisionBox.setFillColor(sf::Color::Transparent);
    m_collisionBox.setOutlineColor(sf::Color::Red);
    m_collisionBox.setOutlineThickness(1.0f);

    m_gravityBox.setSize(sf::Vector2f(80, 10));
    m_gravityBox.setOrigin(m_gravityBox.getSize() / 2.0f);
    m_gravityBox.setPosition(m_sprite.getPosition() + m_gravityBoxOffSet);
    m_gravityBox.setFillColor(sf::Color::Transparent);
    m_gravityBox.setOutlineColor(sf::Color::Magenta);
    m_gravityBox.setOutlineThickness(1.0f);

    m_groundDetectionDebug.setSize(sf::Vector2f(40, 40));
    m_groundDetectionDebug.setFillColor(sf::Color(0, 255, 0, 100));

    m_attackDebugRect.setFillColor(sf::Color(255, 0, 0, 100));
}

void DemonKing::updateDemonKingAnimation(sf::Time m_deltaTime, Player& m_player)
{
    if (m_animationState == EnemyState::ATTACK1)
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

                if (m_currentFrame == 10)
                {
                    if (m_attackRange.intersects(m_player.getBoundingBox()))
                    {
                        m_player.takeDamage(12);
                    }
                }
            }

            if (m_currentFrame == m_attack1Frames.size() - 1 || m_currentFrame == m_attack2Frames.size() - 1)
            {
                m_isAttacking = false;
                m_isIdling = true;
                m_timeIdleElapsed = sf::Time::Zero;
                setState(EnemyState::IDLE);
            }
        }
    }
    else
    {
        Enemy::updateAnimation(m_deltaTime);
    }
}
