#include "Enemy.h"

Enemy::Enemy(const sf::Vector2f& position)
    : m_animationState(EnemyState::IDLE)
{
    m_sprite.setPosition(position);
}

Enemy::~Enemy()
{
}

void Enemy::update(sf::Time m_deltaTime)
{
}

void Enemy::render(sf::RenderWindow& m_window)
{
    if (!m_isDead)
    {
        m_window.draw(m_sprite);
    }
}

void Enemy::takeDamage(int m_damage)
{
    m_health -= m_damage;
    if (m_health <= 0)
    {
        m_isDead = true;
        setState(EnemyState::DEATH);
    }
    else
    {
        setState(EnemyState::TAKE_HIT);
    }
}

sf::FloatRect Enemy::getBoundingBox() const
{
    return m_sprite.getGlobalBounds();
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
        m_currentFrame = (m_currentFrame + 1) % m_idleFrames.size();

        switch (m_animationState)
        {
        case EnemyState::IDLE:
            if (m_currentFrame < m_idleFrames.size())
            {
                m_sprite.setTextureRect(m_idleFrames[m_currentFrame]);
            }
            break;
        case EnemyState::DEATH:
            if (m_currentFrame < m_deathFrames.size())
            {
                m_sprite.setTextureRect(m_deathFrames[m_currentFrame]);
            }
            break;
        case EnemyState::WALK:
            if (m_currentFrame < m_walkFrames.size())
            {
                m_sprite.setTextureRect(m_walkFrames[m_currentFrame]);
            }
            break;
        case EnemyState::ATTACK1:
            if (m_currentFrame < m_attack1Frames.size())
            {
                m_sprite.setTextureRect(m_attack1Frames[m_currentFrame]);
            }
            break;
        case EnemyState::SHIELD:
            if (m_currentFrame < m_shieldFrames.size())
            {
                m_sprite.setTextureRect(m_shieldFrames[m_currentFrame]);
            }
            break;
        case EnemyState::TAKE_HIT:
            if (m_currentFrame < m_takeHitFrames.size())
            {
                m_sprite.setTextureRect(m_takeHitFrames[m_currentFrame]);
            }
            break;
        }
    }
}

void Enemy::setState(EnemyState m_state)
{
    if (m_animationState != m_state)
    {
        m_animationState = m_state;
        m_currentFrame = 0;
    }
}
