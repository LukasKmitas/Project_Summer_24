#include "Enemy.h"

Enemy::Enemy(const sf::Vector2f& position, std::vector<Block>& m_blocks)
    : m_animationState(EnemyState::IDLE), m_gameBlocks(m_blocks)
{
    m_sprite.setPosition(position);
    setupHealthBar();
    initVisionCone();
}

Enemy::~Enemy()
{
}

void Enemy::update(sf::Time m_deltaTime, Player& m_player)
{
    updateVisionCone();
    updateHealthBar();
    if (isPlayerInVisionCone(m_player))
    {
        m_playerPos = m_player.getPosition();
        m_playerFound = true;
    }
    else
    {
        m_playerFound = false;
    }
}

void Enemy::render(sf::RenderWindow& m_window)
{
    if (!m_isDead)
    {
        m_window.draw(m_healthBarBackground);
        m_window.draw(m_healthBar);

        // DEBUG STUFF
        m_window.draw(m_visionCone);
        m_window.draw(m_collisionBox);
        m_window.draw(m_gravityBox);
        m_window.draw(m_groundDetectionDebug);

        // Line in sight for player
        if (m_playerFound)
        {
            sf::Vertex line[] =
            {
                sf::Vertex(m_sprite.getPosition(), sf::Color::Blue),
                sf::Vertex(m_playerPos, sf::Color::Blue)
            };
            m_window.draw(line, 2, sf::Lines);
        }
    }
}

void Enemy::takeDamage(int m_damage)
{
    if (m_isShieldActive)
    {
        std::cout << "Enemy is shielded" << std::endl;
        return;
    }
    else
    {
        m_health -= m_damage;
        if (m_health <= 0)
        {
            m_isDead = true;
            setState(EnemyState::DEATH);
        }
        else
        {
            if (m_animationState != EnemyState::ATTACK1 && m_animationState != EnemyState::ATTACK2 && m_animationState != EnemyState::ATTACK3)
            {
                if (m_animationState != EnemyState::TAKE_HIT)
                {
                    m_previousState = m_animationState;
                    setState(EnemyState::TAKE_HIT);
                }
            }
        }
    }
}

sf::FloatRect Enemy::getCollisionBoundingBox() const
{
    return m_collisionBox.getGlobalBounds();
}

sf::FloatRect Enemy::getGravityBoundingBox() const
{
    return m_gravityBox.getGlobalBounds();
}

bool Enemy::isDead() const
{
    return m_isDead;
}

void Enemy::updateAnimation(sf::Time m_deltaTime)
{
    m_currentFrameTime += m_deltaTime;
    if (m_currentFrameTime >= m_frameTime)
    {
        m_currentFrameTime = sf::Time::Zero;

        switch (m_animationState)
        {
        case EnemyState::IDLE:
            m_currentFrame = (m_currentFrame + 1) % m_idleFrames.size();
            m_sprite.setTexture(m_idleTexture);
            m_sprite.setTextureRect(m_idleFrames[m_currentFrame]);
            break;
        case EnemyState::ATTACK1:
            m_currentFrame = (m_currentFrame + 1) % m_attack1Frames.size();
            m_sprite.setTexture(m_attack1Texture);
            m_sprite.setTextureRect(m_attack1Frames[m_currentFrame]);
            break;
        case EnemyState::ATTACK2:
            m_currentFrame = (m_currentFrame + 1) % m_attack2Frames.size();
            m_sprite.setTexture(m_attack2Texture);
            m_sprite.setTextureRect(m_attack2Frames[m_currentFrame]);
            break;
        case EnemyState::ATTACK3:
            m_currentFrame = (m_currentFrame + 1) % m_attack3Frames.size();
            m_sprite.setTexture(m_attack3Texture);
            m_sprite.setTextureRect(m_attack3Frames[m_currentFrame]);
            break;
        case EnemyState::PATROL:
            m_currentFrame = (m_currentFrame + 1) % m_walkFrames.size();
            m_sprite.setTexture(m_walkTexture);
            m_sprite.setTextureRect(m_walkFrames[m_currentFrame]);
            break;
        case EnemyState::DEATH:
            if (m_currentFrame < m_deathFrames.size() - 1)
            {
                m_currentFrame = (m_currentFrame + 1) % m_deathFrames.size();
            }
            m_sprite.setTexture(m_deathTexture);
            m_sprite.setTextureRect(m_deathFrames[m_currentFrame]);
            break;
        case EnemyState::TAKE_HIT:
            m_currentFrame = (m_currentFrame + 1) % m_takeHitFrames.size();
            m_sprite.setTexture(m_takeHitTexture);
            m_sprite.setTextureRect(m_takeHitFrames[m_currentFrame]);
            if (m_currentFrame == m_takeHitFrames.size() - 1)
            {
                setState(m_previousState);
            }
            break;
        case EnemyState::SHIELD:
            m_currentFrame = (m_currentFrame + 1) % m_shieldFrames.size();
            m_sprite.setTexture(m_shieldTexture);
            m_sprite.setTextureRect(m_shieldFrames[m_currentFrame]);
            break;
        }
    }
}

void Enemy::updateHealthBar()
{
    sf::Vector2f healthBarPosition = m_sprite.getPosition() - sf::Vector2f(0, 30);

    m_healthBarBackground.setPosition(healthBarPosition);
    m_healthBar.setPosition(healthBarPosition);

    float healthPercentage = static_cast<float>(m_health) / m_maxHealth;
    m_healthBar.setSize(sf::Vector2f(50.0f * healthPercentage, 5.0f));
}

void Enemy::applyGravity(sf::Time m_deltaTime)
{
    if (!m_isOnGround)
    {
        m_velocity.y += m_gravity * m_deltaTime.asSeconds();
    }
    else
    {
        m_velocity.y = 0;
    }

    sf::Vector2f newPosition = m_sprite.getPosition() + m_velocity * m_deltaTime.asSeconds();

    m_sprite.setPosition(newPosition);
    m_gravityBox.setPosition(m_sprite.getPosition() + m_gravityBoxOffSet);
    m_isOnGround = false;

    for (const auto& block : m_gameBlocks)
    {
        if (!block.traversable && block.shape.getGlobalBounds().intersects(getGravityBoundingBox()))
        {
            if (m_velocity.y > 0)
            {
                newPosition.y = block.shape.getPosition().y;
                m_isOnGround = true;

                m_sprite.setPosition(newPosition - m_spriteOffSet);
                m_gravityBox.setPosition(m_sprite.getPosition() + m_gravityBoxOffSet);
            }
            break;
        }
    }
}

void Enemy::patrol(sf::Time m_deltaTime)
{
    m_moveDirection = m_movingDirection ? 1.0f : -1.0f;
    m_velocity.x = m_speed * m_moveDirection;

    sf::FloatRect detectionBox = m_collisionBox.getGlobalBounds();
    detectionBox.left += m_moveDirection * 30.0f;

    bool blocked = false;
    bool edgeDetected = false;

    for (const auto& block : m_gameBlocks)
    {
        if (!block.traversable && block.shape.getGlobalBounds().intersects(detectionBox))
        {
            blocked = true;
            break;
        }
    }

    m_groundDetectionBox = m_collisionBox.getGlobalBounds();
    m_groundDetectionBox.left += m_moveDirection * 30.0f;
    m_groundDetectionBox.top += 40.0f;
    m_groundDetectionDebug.setPosition(m_groundDetectionBox.left, m_groundDetectionBox.top);

    for (const auto& block : m_gameBlocks)
    {
        if (!block.traversable && block.shape.getGlobalBounds().intersects(m_groundDetectionBox))
        {
            edgeDetected = true;
            break;
        }
    }

    if (!edgeDetected || blocked)
    {
        m_velocity.x = 0;
        m_isIdling = true;
        setState(EnemyState::IDLE);
    }

    m_sprite.move(m_velocity * m_deltaTime.asSeconds());

    if (m_movingDirection)
    {
        m_sprite.setScale(1.0f, 1.0f);
    }
    else
    {
        m_sprite.setScale(-1.0f, 1.0f);
    }
}

void Enemy::initVisionCone()
{
    m_visionCone.setPointCount(m_numPoints + 2);
    m_visionCone.setFillColor(sf::Color(255, 255, 0, 100));
}

void Enemy::updateVisionCone()
{
    m_visionCone.setPoint(0, m_sprite.getPosition());

    sf::Vector2f forwardDirection = m_sprite.getScale().x > 0 ? sf::Vector2f(1.0f, 0.0f) : sf::Vector2f(-1.0f, 0.0f);
    float halfAngleRad = m_visionAngle * 0.5f * 3.14159265f / 180.0f;

    for (int i = 0; i <= m_numPoints; ++i)
    {
        float angle = -halfAngleRad + (i * m_visionAngle / m_numPoints) * 3.14159265f / 180.0f;
        sf::Vector2f direction = sf::Vector2f(
            cos(angle) * forwardDirection.x - sin(angle) * forwardDirection.y,
            sin(angle) * forwardDirection.x + cos(angle) * forwardDirection.y
        );
        sf::Vector2f point = m_sprite.getPosition() + direction * m_visionRange;
        m_visionCone.setPoint(i + 1, point);
    }
}

bool Enemy::isPlayerInVisionCone(const Player& m_player) const
{
    sf::Vector2f playerPosition = m_player.getPosition();
    sf::Vector2f directionToPlayer = playerPosition - m_sprite.getPosition();

    float distanceToPlayer = std::sqrt(directionToPlayer.x * directionToPlayer.x + directionToPlayer.y * directionToPlayer.y);
    if (distanceToPlayer > m_visionRange)
    {
        return false;
    }

    directionToPlayer /= distanceToPlayer;  // Normalize the direction vector
    sf::Vector2f forwardDirection = m_sprite.getScale().x > 0 ? sf::Vector2f(1.0f, 0.0f) : sf::Vector2f(-1.0f, 0.0f);  // Facing direction

    float dotProduct = directionToPlayer.x * forwardDirection.x + directionToPlayer.y * forwardDirection.y;
    float angleToPlayer = std::acos(dotProduct) * 180.0f / 3.14159265f;

    if (angleToPlayer < m_visionAngle / 2.0f && isLineOfSightClearToPlayer(m_sprite.getPosition(), playerPosition))
    {
        return true;
    }

    return false;
}

bool Enemy::isLineOfSightClearToPlayer(const sf::Vector2f& m_from, const sf::Vector2f& m_to) const
{
    sf::Vector2f direction = m_to - m_from;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    direction /= length;

    for (float i = 0; i < length; i += 5.0f)
    {
        sf::Vector2f checkPoint = m_from + direction * i;

        for (const auto& block : m_gameBlocks)
        {
            if (!block.traversable && block.shape.getGlobalBounds().contains(checkPoint))
            {
                return false;
            }
        }
    }
    return true;
}

void Enemy::setState(EnemyState m_state)
{
    if (m_animationState != m_state)
    {
        m_animationState = m_state;
        m_currentFrame = 0;
    }
}

void Enemy::setupHealthBar()
{
    m_healthBarBackground.setSize(sf::Vector2f(50.0f, 5.0f));
    m_healthBarBackground.setFillColor(sf::Color::Red);
    m_healthBarBackground.setOrigin(m_healthBarBackground.getSize().x / 2.0f, m_healthBarBackground.getSize().y / 2.0f);

    m_healthBar.setSize(sf::Vector2f(50.0f, 5.0f));
    m_healthBar.setFillColor(sf::Color::Green);
    m_healthBar.setOrigin(m_healthBar.getSize().x / 2.0f, m_healthBar.getSize().y / 2.0f);
}
