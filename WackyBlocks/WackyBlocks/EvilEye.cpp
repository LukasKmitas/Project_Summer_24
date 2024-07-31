#include "EvilEye.h"

EvilEye::EvilEye(const sf::Vector2f& m_position, const std::vector<Block>& m_blocks)
	: Enemy(m_position), m_gameBlocks(m_blocks)
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
    setState(EnemyState::IDLE);
    findPatrolPoints(m_blocks);
    m_health = 60;
    m_speed = 50;
}

void EvilEye::update(sf::Time m_deltaTime)
{
    delayIfReachedPoint(m_deltaTime);
    if (!m_isWaiting)
    {
        moveTowardsCurrentPoint(m_deltaTime);
    }
    updateAnimation(m_deltaTime);
    m_collisionBox.setPosition(m_sprite.getPosition());
}

void EvilEye::render(sf::RenderWindow& m_window)
{
    m_window.draw(m_sprite);
    m_window.draw(m_collisionBox);

    // Draw patrol points and lines
    for (size_t i = 0; i < m_patrolPoints.size(); ++i)
    {
        sf::CircleShape patrolPointShape(5.0f);
        patrolPointShape.setPosition(m_patrolPoints[i]);
        patrolPointShape.setOrigin(5.0f, 5.0f);

        if (i == m_currentPatrolIndex)
        {
            patrolPointShape.setFillColor(sf::Color::Red);
        }
        else
        {
            patrolPointShape.setFillColor(sf::Color::Green);
        }

        m_window.draw(patrolPointShape);

        // Draw lines between patrol points
        if (i > 0)
        {
            sf::Color lineColor = (i == m_currentPatrolIndex) ? sf::Color::Red : sf::Color::Green;
            sf::Vertex line[] =
            {
                sf::Vertex(m_patrolPoints[i - 1], lineColor),
                sf::Vertex(m_patrolPoints[i], lineColor)
            };
            m_window.draw(line, 2, sf::Lines);
        }
    }

    // Connect the last point to the first to complete the loop
    if (m_patrolPoints.size() > 1)
    {
        sf::Color lineColor = (m_currentPatrolIndex == 0) ? sf::Color::Red : sf::Color::Green;
        sf::Vertex line[] =
        {
            sf::Vertex(m_patrolPoints.back(), lineColor),
            sf::Vertex(m_patrolPoints.front(), lineColor)
        };
        m_window.draw(line, 2, sf::Lines);
    }

    for (auto& circle : m_debugCircles)
    {
        m_window.draw(circle);
    }
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

void EvilEye::setupBoundingBox()
{
    sf::FloatRect bounds = m_sprite.getGlobalBounds();
    m_collisionBox.setSize(sf::Vector2f(bounds.width * 0.6f, bounds.height * 0.6f));
    m_collisionBox.setOrigin(m_collisionBox.getSize() / 2.0f);
    m_collisionBox.setPosition(m_sprite.getPosition());
    m_collisionBox.setFillColor(sf::Color::Transparent);
    m_collisionBox.setOutlineColor(sf::Color::Red);
    m_collisionBox.setOutlineThickness(1.0f);
}

void EvilEye::findPatrolPoints(const std::vector<Block>& m_blocks)
{
    const float patrolRadius = 300.0f; 
    const int numPoints = 2;
    sf::Vector2f startPosition = m_sprite.getPosition();

    m_patrolPoints.push_back(startPosition);

    for (int i = 0; i < numPoints; ++i)
    {
        sf::Vector2f point;
        bool valid = false;

        while (!valid)
        {
            point = generateRandomPointOnCircle(patrolRadius);
            point += startPosition;

            if (isValidPatrolPoint(point, m_blocks))
            {
                valid = true;
                m_patrolPoints.push_back(point);

                // Add debug circle
                sf::CircleShape debugCircle(5.0f);
                debugCircle.setPosition(point);
                debugCircle.setOrigin(5.0f, 5.0f);
                debugCircle.setFillColor(sf::Color::Yellow);
                m_debugCircles.push_back(debugCircle);
            }
        }
    }
}

void EvilEye::moveTowardsCurrentPoint(sf::Time m_deltaTime)
{
    if (m_patrolPoints.empty() || m_isWaiting)
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

    sf::Vector2f velocity = direction * m_speed * m_deltaTime.asSeconds();
    m_sprite.move(velocity);
    m_collisionBox.setPosition(m_sprite.getPosition());

    if (std::abs(currentPosition.x - targetPosition.x) < 1.0f && std::abs(currentPosition.y - targetPosition.y) < 1.0f)
    {
        m_isWaiting = true;
        m_currentWaitTime = sf::Time::Zero;
        m_currentPatrolIndex = (m_currentPatrolIndex + 1) % m_patrolPoints.size();
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

bool EvilEye::isValidPatrolPoint(const sf::Vector2f& m_point, const std::vector<Block>& m_blocks) const
{
    const float bufferDistance = 40.0f;
    sf::FloatRect bufferBounds(m_point.x - bufferDistance, m_point.y - bufferDistance, 2 * bufferDistance, 2 * bufferDistance);
    for (const auto& block : m_blocks)
    {
        if (block.shape.getGlobalBounds().intersects(bufferBounds) && !block.traversable)
        {
            return false;
        }
    }
    return true;
}

sf::Vector2f EvilEye::generateRandomPointOnCircle(float radius) const
{
    float angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f * 3.14159265f;
    return sf::Vector2f(radius * cos(angle), radius * sin(angle));
}

