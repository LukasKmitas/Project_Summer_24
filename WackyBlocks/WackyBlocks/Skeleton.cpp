#include "Skeleton.h"

Skeleton::Skeleton(const sf::Vector2f& m_position, std::vector<Block>& m_blocks)
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
    m_speed = 20;
    m_visionRange = 300.0f;
    m_visionAngle = 50.0f;
    m_spriteOffSet = sf::Vector2f(0, 65);
    m_gravityBoxOffSet = sf::Vector2f(0, 24);
}

Skeleton::~Skeleton()
{
}

void Skeleton::update(sf::Time m_deltaTime, Player& m_player)
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
            m_animationState == EnemyState::ATTACK3 ||
            m_animationState == EnemyState::SHIELD
            )
        {
            m_velocity.x = 0;
        }

        if (m_animationState == EnemyState::PATROL)
        {
            patrol(m_deltaTime);
        }

        m_timeSinceLastAttack += m_deltaTime;
        m_timeSinceLastRangeAttack += m_deltaTime;
        m_attackRange = m_collisionBox.getGlobalBounds();
        m_attackRange.left += (m_movingDirection ? 40.0f : -40.0f);
        m_attackDebugRect.setSize(sf::Vector2f(m_attackRange.width, m_attackRange.height));
        m_attackDebugRect.setPosition(m_attackRange.left, m_attackRange.top);
        sf::Vector2f detectionPos = m_sprite.getPosition();
        detectionPos.x += (m_movingDirection ? 150.0f : -150.0f);
        m_shieldDectectionBox.setPosition(detectionPos);
        m_isShieldActive = false;

        for (auto& bullet : m_player.getBullets())
        {
            if (m_shieldDectectionBox.getGlobalBounds().intersects(bullet.shape.getGlobalBounds()))
            {
                if (isBulletMovingTowardsSkeleton(bullet))
                {
                    setState(EnemyState::SHIELD);
                    m_isShieldActive = true;
                    break;
                }
            }
        }

        if (!m_isShieldActive && m_animationState == EnemyState::SHIELD)
        {
            m_isIdling = true;
            setState(EnemyState::IDLE);
        }

        if (m_timeSinceLastAttack >= m_attackCooldown && m_attackDebugRect.getGlobalBounds().intersects(m_player.getBoundingBox()) && !m_isAttacking)
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
        else if (m_timeSinceLastRangeAttack >= m_rangeAttackCooldown && isPlayerInVisionCone(m_player) && !m_isAttacking)
        {
            setState(EnemyState::ATTACK3);
            m_isAttacking = true;
            m_timeSinceLastAttack = sf::Time::Zero;
            m_timeSinceLastRangeAttack = sf::Time::Zero;
        }
    }
    m_collisionBox.setPosition(m_sprite.getPosition());
    m_gravityBox.setPosition(m_sprite.getPosition() + m_gravityBoxOffSet);
    updateSkeletonAnimation(m_deltaTime, m_player);

    for (auto& projectile : m_projectiles)
    {
        projectile.update(m_deltaTime, m_gameBlocks);
        if (projectile.checkCollisionWithPlayer(const_cast<Player&>(m_player)))
        {
            // Sound or particle
        }
    }

    m_projectiles.erase(std::remove_if(m_projectiles.begin(), m_projectiles.end(),
        [](const EnemyProjectile& proj) { return proj.isDestroyed(); }), m_projectiles.end());
}

void Skeleton::render(sf::RenderWindow& m_window)
{
    m_window.draw(m_sprite);

    if (!m_isDead)
    {
        Enemy::render(m_window);

        if (m_animationState == EnemyState::ATTACK1 || m_animationState == EnemyState::ATTACK2 && m_currentFrame == 6)
        {
            m_window.draw(m_attackDebugRect);
        }

        m_window.draw(m_shieldDectectionBox);

    }

    for (auto& projectile : m_projectiles)
    {
        projectile.render(m_window);
    }
}

void Skeleton::loadTextures()
{
    if (!m_idleTexture.loadFromFile("Assets\\Images\\Enemies\\Skeleton\\Idle.png"))
    {
        std::cout << "problem loading Skeleton Idle image" << std::endl;
    }

    if (!m_walkTexture.loadFromFile("Assets\\Images\\Enemies\\Skeleton\\Walk.png"))
    {
        std::cout << "problem loading Skeleton walk image" << std::endl;
    }

    if (!m_attack1Texture.loadFromFile("Assets\\Images\\Enemies\\Skeleton\\Attack.png"))
    {
        std::cout << "problem loading Skeleton attack1 image" << std::endl;
    }

    if (!m_attack2Texture.loadFromFile("Assets\\Images\\Enemies\\Skeleton\\Attack2.png"))
    {
        std::cout << "problem loading Skeleton attack2 image" << std::endl;
    }

    if (!m_attack3Texture.loadFromFile("Assets\\Images\\Enemies\\Skeleton\\Attack3.png"))
    {
        std::cout << "problem loading Skeleton attack3 image" << std::endl;
    }

    if (!m_projectileTexture.loadFromFile("Assets\\Images\\Enemies\\Skeleton\\Sword_sprite.png"))
    {
        std::cout << "problem loading Skeleton projectile image" << std::endl;
    }

    if (!m_deathTexture.loadFromFile("Assets\\Images\\Enemies\\Skeleton\\Death.png"))
    {
        std::cout << "problem loading Skeleton death image" << std::endl;
    }

    if (!m_takeHitTexture.loadFromFile("Assets\\Images\\Enemies\\Skeleton\\TakeHit.png"))
    {
        std::cout << "problem loading Skeleton take hit image" << std::endl;
    }

    if (!m_shieldTexture.loadFromFile("Assets\\Images\\Enemies\\Skeleton\\Shield.png"))
    {
        std::cout << "problem loading Skeleton shield image" << std::endl;
    }
}

void Skeleton::loadFrames()
{
    const int frameWidth = 150;
    const int frameHeight = 150;
    const int bulletFrameWidth = 92;
    const int bulletFrameHeight = 102;

    for (int i = 0; i < 4; ++i)
    {
        m_idleFrames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }

    for (int i = 0; i < 4; ++i)
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

    for (int i = 0; i < 6; ++i)
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

    for (int i = 0; i < 4; ++i)
    {
        m_shieldFrames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }
}

void Skeleton::setupBoundingBox()
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

    m_shieldDectectionBox.setSize(sf::Vector2f(300.0f, 50.0f));
    m_shieldDectectionBox.setFillColor(sf::Color(0, 255, 0, 100));
    m_shieldDectectionBox.setOrigin(m_shieldDectectionBox.getSize().x / 2, m_shieldDectectionBox.getSize().y / 2);

}

void Skeleton::updateSkeletonAnimation(sf::Time m_deltaTime, Player& m_player)
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
                m_timeIdleElapsed = sf::Time::Zero;
                setState(EnemyState::IDLE);
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

            if (m_currentFrame == 3)
            {
                shootProjectile(m_player);
            }

            if (m_currentFrame == m_attack3Frames.size() - 1)
            {
                m_isAttacking = false;
                m_isIdling = true;
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

void Skeleton::shootProjectile(const Player& m_player)
{
    sf::Vector2f playerPos = m_player.getPosition() + sf::Vector2f(0, 40);
    sf::Vector2f direction = playerPos - m_sprite.getPosition();
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length != 0)
    {
        direction /= length;
    }

    m_projectiles.emplace_back(m_sprite.getPosition(), direction, m_projectileTexture, m_projectileFrames);
}

bool Skeleton::isBulletMovingTowardsSkeleton(const Bullet& m_bullet)
{
    sf::Vector2f bulletPosition = m_bullet.shape.getPosition();
    sf::Vector2f bulletDirection = m_bullet.direction;
    sf::FloatRect skeletonBounds = m_collisionBox.getGlobalBounds();
    sf::Vector2f skeletonCenter(skeletonBounds.left + skeletonBounds.width / 2, skeletonBounds.top + skeletonBounds.height / 2);
    sf::Vector2f toSkeleton = skeletonCenter - bulletPosition;

    float bulletLength = std::sqrt(bulletDirection.x * bulletDirection.x + bulletDirection.y * bulletDirection.y);
    float toSkeletonLength = std::sqrt(toSkeleton.x * toSkeleton.x + toSkeleton.y * toSkeleton.y);

    if (bulletLength != 0)
    {
        bulletDirection /= bulletLength;
    }
    if (toSkeletonLength != 0)
    {
        toSkeleton /= toSkeletonLength;
    }

    float dotProduct = bulletDirection.x * toSkeleton.x + bulletDirection.y * toSkeleton.y;

    return dotProduct > 0.9f;
}
