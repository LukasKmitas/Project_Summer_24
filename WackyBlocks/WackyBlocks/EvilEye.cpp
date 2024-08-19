#include "EvilEye.h"

EvilEye::EvilEye(const sf::Vector2f& m_position, std::vector<Block>& m_blocks)
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
    setupDebug();
    setState(EnemyState::PATROL);
    createInitialPatrolPoint();
    m_health = 100;
    m_speed = 50;
    m_visionRange = 300.0f;
    m_visionAngle = 90.0f;
}

EvilEye::~EvilEye()
{
}

void EvilEye::update(sf::Time m_deltaTime, Player& m_player)
{
    if (!m_isDead)
    {
        Enemy::update(m_deltaTime, m_player);

        if (!m_canBiteAttack)
        {
            m_timeSinceLastBiteAttack += m_deltaTime;
            if (m_timeSinceLastBiteAttack >= m_biteAttackCooldown)
            {
                m_canBiteAttack = true;
            }
        }

        if (m_canBiteAttack && m_animationState != EnemyState::ATTACK1 && m_animationState != EnemyState::ATTACK2 && m_animationState != EnemyState::ATTACK3)
        {
            float distanceToPlayer = std::sqrt(std::pow(m_player.getPosition().x - m_sprite.getPosition().x, 2) +
                std::pow(m_player.getPosition().y - m_sprite.getPosition().y, 2));

            if (distanceToPlayer <= 100.0f && isPlayerInVisionCone(m_player))
            {
                m_previousState = m_animationState;
                if (m_animationState != EnemyState::ATTACK1)
                {
                    m_previousState = m_animationState;
                    setState(EnemyState::ATTACK1);
                    m_timeSinceLastBiteAttack = sf::Time::Zero;
                    m_canBiteAttack = false;
                }
            }
        }

        if (!m_canSpinAttack)
        {
            m_timeSinceLastSpinAttack += m_deltaTime;
            if (m_timeSinceLastSpinAttack >= m_spinAttackCooldown)
            {
                m_canSpinAttack = true;
                m_timeSinceLastSpinAttack = sf::Time::Zero;
            }
        }

        if (m_canSpinAttack && m_animationState != EnemyState::ATTACK3 && m_animationState != EnemyState::ATTACK2 && m_animationState != EnemyState::ATTACK1)
        {
            if (m_collisionBox.getGlobalBounds().intersects(m_player.getBoundingBox()))
            {
                m_previousState = m_animationState;
                if (m_animationState != EnemyState::ATTACK2)
                {
                    m_previousState = m_animationState;
                    setState(EnemyState::ATTACK2);
                    m_canSpinAttack = false;
                }
            }
        }

        m_timeSinceLastShot += m_deltaTime;
        if (m_timeSinceLastShot >= m_shootTimer && isPlayerInVisionCone(m_player) && m_animationState != EnemyState::ATTACK1)
        {
            m_timeSinceLastShot = sf::Time::Zero;
            m_previousState = m_animationState;
            if (m_animationState != EnemyState::ATTACK3)
            {
                m_previousState = m_animationState;
                setState(EnemyState::ATTACK3);
            }
        }

        if (m_animationState == EnemyState::PATROL)
        {
            if (m_isWaiting)
            {
                delayIfReachedPoint(m_deltaTime);
            }
            else
            {
                moveToNextPatrolPoint(m_deltaTime);
            }
        }
    }
    m_collisionBox.setPosition(m_sprite.getPosition());
    updateEvilEyeAnimation(m_deltaTime, m_player);

    for (auto& projectile : m_projectiles)
    {
        projectile.update(m_deltaTime, m_gameBlocks);
        if (projectile.checkCollisionWithPlayer(const_cast<Player&>(m_player)))
        {
            // Sound or particle
        }
    }

    // Remove destroyed projectiles
    m_projectiles.erase(std::remove_if(m_projectiles.begin(), m_projectiles.end(),
        [](const EnemyProjectile& proj) { return proj.isDestroyed(); }), m_projectiles.end());
}

void EvilEye::render(sf::RenderWindow& m_window)
{
    m_window.draw(m_sprite);

    for (auto& projectile : m_projectiles)
    {
        projectile.render(m_window);
    }

    if (!m_isDead)
    {
        Enemy::render(m_window);

        // DEBUG STUFF

        // Draw patrol point lines
        for (int i = 0; i < m_patrolPoints.size(); ++i)
        {
            if (i > 0)
            {
                if (!m_isReturning)
                {
                    m_lineColor = (i == m_currentPatrolIndex) ? sf::Color::Red : sf::Color::Green;
                }
                else
                {
                    m_lineColor = (i - 1 == m_currentPatrolIndex) ? sf::Color::Red : sf::Color::Green;
                }

                sf::Vertex line[] =
                {
                    sf::Vertex(m_patrolPoints[i - 1], m_lineColor),
                    sf::Vertex(m_patrolPoints[i], m_lineColor)
                };
                m_window.draw(line, 2, sf::Lines);
            }
        }

        // Draw the circles representing patrol points
        for (int i = 0; i < m_patrolDebugCircles.size(); ++i)
        {
            if (i == m_currentPatrolIndex)
            {
                m_patrolDebugCircles[i].setFillColor(sf::Color::Red);
            }
            else
            {
                m_patrolDebugCircles[i].setFillColor(sf::Color::Yellow);
            }
            m_window.draw(m_patrolDebugCircles[i]);
        }
    }
}

void EvilEye::loadTextures()
{
    if (!m_walkTexture.loadFromFile("Assets\\Images\\Enemies\\EvilEye\\Flight.png"))
    {
        std::cout << "problem loading EvilEye walk(fly) image" << std::endl;
    }

    if (!m_attack1Texture.loadFromFile("Assets\\Images\\Enemies\\EvilEye\\Attack.png"))
    {
        std::cout << "problem loading EvilEye attack1 image" << std::endl;
    }

    if (!m_attack2Texture.loadFromFile("Assets\\Images\\Enemies\\EvilEye\\Attack2.png"))
    {
        std::cout << "problem loading EvilEye attack2 image" << std::endl;
    }

    if (!m_attack3Texture.loadFromFile("Assets\\Images\\Enemies\\EvilEye\\Attack3.png"))
    {
        std::cout << "problem loading EvilEye attack3 image" << std::endl;
    }

    if (!m_projectileTexture.loadFromFile("Assets\\Images\\Enemies\\EvilEye\\projectile_sprite.png"))
    {
        std::cout << "problem loading EvilEye projectile image" << std::endl;
    }

    if (!m_deathTexture.loadFromFile("Assets\\Images\\Enemies\\EvilEye\\Death.png"))
    {
        std::cout << "problem loading EvilEye death image" << std::endl;
    }

    if (!m_takeHitTexture.loadFromFile("Assets\\Images\\Enemies\\EvilEye\\TakeHit.png"))
    {
        std::cout << "problem loading EvilEye take hit image" << std::endl;
    }
}

void EvilEye::loadFrames()
{
    const int frameWidth = 150;
    const int frameHeight = 150;
    const int bulletFrameWidth = 48;
    const int bulletFrameHeight = 48;

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
}

void EvilEye::setupBoundingBox()
{
    m_collisionBox.setSize(sf::Vector2f(40, 40));
    m_collisionBox.setOrigin(m_collisionBox.getSize() / 2.0f);
    m_collisionBox.setPosition(m_sprite.getPosition());
    m_collisionBox.setFillColor(sf::Color::Transparent);
    m_collisionBox.setOutlineColor(sf::Color::Red);
    m_collisionBox.setOutlineThickness(1.0f);
}

void EvilEye::setupDebug()
{
    m_debugCircleForPatrol.setRadius(5);
    m_debugCircleForPatrol.setOrigin(5.0f, 5.0f);
    m_debugCircleForPatrol.setFillColor(sf::Color::Yellow);
}

void EvilEye::createInitialPatrolPoint()
{
    sf::Vector2f startPosition = m_sprite.getPosition();
    m_patrolPoints.push_back(startPosition);

    m_debugCircleForPatrol.setPosition(startPosition);
    m_patrolDebugCircles.push_back(m_debugCircleForPatrol);
}

void EvilEye::createNewPatrolPoint()
{
    if (m_patrolPoints.empty())
    {
        return;
    }

    sf::Vector2f currentPosition = m_patrolPoints.back();

    bool foundValidPoint = false;
    const int maxAttemps = 100;
    const float minDistanceBetweenPoints = 150.0f;

    for (int attempts = 0; attempts < maxAttemps && !foundValidPoint; ++attempts)
    {
        sf::Vector2f potentialPoint = generateRandomPointOnCircle(m_patrolRadius) + currentPosition;

        potentialPoint.x = std::max(40.0f, std::min(static_cast<float>(SCREEN_WIDTH - 40), potentialPoint.x));
        potentialPoint.y = std::max(40.0f, std::min(static_cast<float>(SCREEN_HEIGHT - 40), potentialPoint.y));

        bool isFarEnough = true;
        for (const auto& point : m_patrolPoints)
        {
            float distance = std::sqrt((point.x - potentialPoint.x) * (point.x - potentialPoint.x) +
                (point.y - potentialPoint.y) * (point.y - potentialPoint.y));
            if (distance < minDistanceBetweenPoints)
            {
                isFarEnough = false;
                break;
            }
        }

        if (isFarEnough && isValidPatrolPoint(potentialPoint) && isLineOfSightClearPatrol(currentPosition, potentialPoint))
        {
            m_patrolPoints.push_back(potentialPoint);
            foundValidPoint = true;

            m_debugCircleForPatrol.setPosition(potentialPoint);
            m_patrolDebugCircles.push_back(m_debugCircleForPatrol);
        }
    }

    if (!foundValidPoint)
    {
        std::cout << "Failed to create a patrol point after " << maxAttemps << " attempts" << std::endl;
    }
}

void EvilEye::moveToNextPatrolPoint(sf::Time m_deltaTime)
{
    if (m_patrolPoints.empty())
    {
        return;
    }

    sf::Vector2f currentPosition = m_sprite.getPosition();
    sf::Vector2f targetPosition = m_patrolPoints[m_currentPatrolIndex];

    sf::Vector2f direction = targetPosition - currentPosition;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length != 0)
    {
        direction /= length;
    }

    if (direction.x > 0)
    {
        m_sprite.setScale(1.0f, 1.0f); 
    }
    else if (direction.x < 0)
    {
        m_sprite.setScale(-1.0f, 1.0f);
    }

    sf::Vector2f velocity = direction * m_speed * m_deltaTime.asSeconds();
    m_sprite.move(velocity);
    m_collisionBox.setPosition(m_sprite.getPosition());

    if (std::abs(currentPosition.x - targetPosition.x) < 1.0f && std::abs(currentPosition.y - targetPosition.y) < 1.0f)
    {
        m_isWaiting = true;
        m_currentWaitTime = sf::Time::Zero;

        // Check if we need to create a new patrol point
        if (m_currentPatrolIndex == m_patrolPoints.size() - 1 && m_patrolPoints.size() < m_amountOfPatrolPoints)
        {
            createNewPatrolPoint();
        }

        // Forward
        if (!m_isReturning)
        {
            if (m_currentPatrolIndex < m_patrolPoints.size() - 1)
            {
                m_currentPatrolIndex++;
            }
            else
            {
                m_isReturning = true;
            }
        }
        // Backward
        else
        {
            if (m_currentPatrolIndex > 0)
            {
                m_currentPatrolIndex--;
            }
            else
            {
                m_isReturning = false;
            }
        }
    }
}

void EvilEye::delayIfReachedPoint(sf::Time m_deltaTime)
{
    if (m_isWaiting)
    {
        m_currentWaitTime += m_deltaTime;
        if (m_currentWaitTime >= m_waitTime)
        {
            m_isWaiting = false;
        }
    }
}

void EvilEye::updateEvilEyeAnimation(sf::Time m_deltaTime, Player& m_player)
{
    m_currentFrameTime += m_deltaTime;

    if (m_animationState == EnemyState::DEATH)
    {
        // During the death sequence, stay on the falling frame (2nd frame) until hitting the ground
        if (!m_hasHitGround)
        {
            m_currentFrame = 1;
            m_sprite.setTexture(m_deathTexture);
            m_sprite.setTextureRect(m_deathFrames[m_currentFrame]);

            sf::Vector2f velocity(0, m_fallSpeed * m_deltaTime.asSeconds());
            m_sprite.move(velocity);

            for (auto& block : m_gameBlocks)
            {
                if (block.shape.getGlobalBounds().intersects(m_collisionBox.getGlobalBounds()))
                {
                    m_hasHitGround = true;
                    m_sprite.setPosition(m_sprite.getPosition().x, m_sprite.getPosition().y - 10);
                    break;
                }
            }
        }
        else
        {
            if (m_currentFrame < m_deathFrames.size() - 1)
            {
                m_currentFrame++;  // Move to the splatter frames
            }
            else
            {
                m_currentFrame = m_deathFrames.size() - 1;  // Stay on the final frame
            }
            m_sprite.setTexture(m_deathTexture);
            m_sprite.setTextureRect(m_deathFrames[m_currentFrame]);
        }
    }
    else if (m_animationState == EnemyState::ATTACK1)
    {
        if (m_currentFrameTime >= m_frameTime)
        {
            m_currentFrameTime = sf::Time::Zero;
            m_currentFrame = (m_currentFrame + 1) % m_attack1Frames.size();
            m_sprite.setTexture(m_attack1Texture);
            m_sprite.setTextureRect(m_attack1Frames[m_currentFrame]);

            if (m_currentFrame == 5)
            {
                m_originalPosition = m_sprite.getPosition();
                sf::Vector2f leapDirection = m_player.getPosition() - m_sprite.getPosition();
                float length = std::sqrt(leapDirection.x * leapDirection.x + leapDirection.y * leapDirection.y);
                if (length != 0)
                {
                    leapDirection /= length;
                }
                sf::Vector2f leapDistance = leapDirection * 120.0f;
                m_sprite.move(leapDistance);
            }

            if (m_currentFrame == 6)
            {
                if (m_collisionBox.getGlobalBounds().intersects(m_player.getBoundingBox()))
                {
                    m_player.takeDamage(15);
                }
                m_sprite.setPosition(m_originalPosition);
            }

            if (m_currentFrame == m_attack1Frames.size() - 1)
            {
                setState(m_previousState);
            }
        }
    }
    else if (m_animationState == EnemyState::ATTACK2)
    {
        if (m_currentFrameTime >= m_frameTime)
        {
            m_currentFrameTime = sf::Time::Zero;
            m_currentFrame = (m_currentFrame + 1) % m_attack2Frames.size();
            m_sprite.setTexture(m_attack2Texture);
            m_sprite.setTextureRect(m_attack2Frames[m_currentFrame]);

            if (m_currentFrame == 5)
            {
                if (m_collisionBox.getGlobalBounds().intersects(m_player.getBoundingBox()))
                {
                    sf::Vector2f knockbackDirection = m_player.getPosition() - m_sprite.getPosition();
                    float length = std::sqrt(knockbackDirection.x * knockbackDirection.x + knockbackDirection.y * knockbackDirection.y);
                    if (length != 0)
                    {
                        knockbackDirection /= length;
                    }
                    sf::Vector2f knockbackForce = knockbackDirection * m_knockbackStrength;

                    m_player.takeDamage(5);
                    m_player.applyKnockback(knockbackForce, sf::seconds(0.5f));
                }
            }

            if (m_currentFrame == m_attack2Frames.size() - 1)
            {
                setState(m_previousState);
            }
        }
    }
    else if (m_animationState == EnemyState::ATTACK3)
    {
        if (m_currentFrameTime >= m_frameTime)
        {
            m_currentFrameTime = sf::Time::Zero;
            m_currentFrame = (m_currentFrame + 1) % m_attack3Frames.size();
            m_sprite.setTexture(m_attack3Texture);
            m_sprite.setTextureRect(m_attack3Frames[m_currentFrame]);

            if (m_currentFrame == 4 && !m_hasShotProjectile)
            {
                shootProjectile(m_player);
                m_hasShotProjectile = true;
            }

            if (m_currentFrame == m_attack3Frames.size() - 1)
            {
                setState(m_previousState);
                m_hasShotProjectile = false;
            }
        }
    }
    else
    {
        Enemy::updateAnimation(m_deltaTime);
    }
}

void EvilEye::shootProjectile(const Player& m_player)
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

bool EvilEye::isValidPatrolPoint(const sf::Vector2f& m_point) const
{
    const float bufferDistance = 40.0f;
    sf::FloatRect pointBounds(m_point.x - bufferDistance, m_point.y - bufferDistance, 2 * bufferDistance, 2 * bufferDistance);

    for (const auto& block : m_gameBlocks)
    {
        if (!block.traversable && block.shape.getGlobalBounds().intersects(pointBounds))
        {
            return false;
        }
    }

    return true;
}

bool EvilEye::isLineOfSightClearPatrol(const sf::Vector2f& m_from, const sf::Vector2f& m_to)
{
    sf::Vector2f direction = m_to - m_from;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    direction /= length;

    for (float i = 0; i < length; i += 5.0f)
    {
        sf::Vector2f checkPoint = m_from + direction * i;
        sf::FloatRect lineBounds(checkPoint.x - 20.0f, checkPoint.y - 20.0f, 40.0f, 40.0f);

        for (const auto& block : m_gameBlocks)
        {
            if (!block.traversable && block.shape.getGlobalBounds().intersects(lineBounds))
            {
                return false;
            }
        }
    }

    return true;
}

sf::Vector2f EvilEye::generateRandomPointOnCircle(float m_radius) const
{
    float angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f * 3.14159265f;
    float distance = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * m_radius;
    return sf::Vector2f(distance * cos(angle), distance * sin(angle));
}

