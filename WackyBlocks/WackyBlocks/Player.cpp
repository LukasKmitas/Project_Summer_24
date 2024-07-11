#include "Player.h"

Player::Player()
    : m_animationState(PlayerState::Idle)
{
    setupPlayer();
    loadTextures();
    loadFrames();
    updateBoundingBox();
    m_playerSprite.setTextureRect(m_idleFrames[0]);
}

Player::~Player()
{
}


void Player::update(sf::Time t_deltaTime, const std::vector<Block>& m_gameBlocks)
{
    handleInput(t_deltaTime, m_gameBlocks);
    applyGravity(t_deltaTime, m_gameBlocks);
    updateAnimation(t_deltaTime);
    updateBoundingBox();
}

void Player::render(sf::RenderWindow& m_window)
{
    m_window.draw(m_playerSprite);
    m_window.draw(m_boundingBox);
    m_window.draw(m_groundBoundingBox);
}

void Player::setPosition(float m_x, float m_y)
{
    m_playerSprite.setPosition(m_x, m_y);
    updateBoundingBox();
}

void Player::setOtherPlayerColor()
{
    m_playerSprite.setColor(sf::Color::Yellow);
}

void Player::setCurrentFrame(int m_frame)
{
    if (m_frame >= 0 && m_frame < getFrameCountForState(m_animationState))
    {
        m_currentFrame = m_frame;
        updateAnimationFrame();
    }
}

void Player::setState(PlayerState m_state)
{
    if (m_state != m_animationState)
    {
        m_animationState = m_state;
        m_currentFrame = 0;
        updateAnimationFrame();
    }
}

int Player::getCurrentFrame() const
{
    return m_currentFrame;
}

PlayerState Player::getState() const
{
    return m_animationState;
}

void Player::handleInput(sf::Time t_deltaTime, const std::vector<Block>& m_gameBlocks)
{
    bool moving = false;
    sf::Vector2f movement(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && !m_isJumping && !m_isFalling)
    {
        m_animationState = PlayerState::Jumping;
        m_playerSprite.setTexture(m_jumpTexture);
        m_frameCount = 3;
        m_verticalSpeed = -m_jumpSpeed;
        m_isJumping = true;
        m_isFalling = false;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        movement.x -= m_speed * t_deltaTime.asSeconds();
        if (m_facingDirection)
        {
            m_facingDirection = false;
            m_playerSprite.setScale(-2, 2);
        }
        moving = true;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        movement.x += m_speed * t_deltaTime.asSeconds();
        if (!m_facingDirection)
        {
            m_facingDirection = true;
            m_playerSprite.setScale(2, 2);
        }
        moving = true;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    {
        m_animationState = PlayerState::Attacking;
        m_playerSprite.setTexture(m_attackTexture);
        m_frameCount = 4;
    }
    else if (!m_isJumping && !m_isFalling)
    {
        m_animationState = PlayerState::Idle;
        m_playerSprite.setTexture(m_idleTexture);
        m_frameCount = 10;
    }

    if (moving)
    {
        if (!m_isJumping && !m_isFalling)
        {
            m_animationState = PlayerState::Running;
            m_playerSprite.setTexture(m_runTexture);
            m_frameCount = 10;
        }

        sf::Vector2f newPos = m_playerSprite.getPosition() + movement;
        sf::FloatRect newBounds = m_boundingBox.getGlobalBounds();
        newBounds.left += movement.x;

        bool collisionDetected = false;
        for (const auto& block : m_gameBlocks)
        {
            if (block.shape.getGlobalBounds().intersects(newBounds))
            {
                collisionDetected = true;
                break;
            }
        }

        if (!collisionDetected)
        {
            m_playerSprite.setPosition(newPos);
            updateBoundingBox();
        }
    }
}

sf::FloatRect Player::getBoundingBox() const
{
    return m_boundingBox.getGlobalBounds();
}

sf::FloatRect Player::getGroundDetectionBox() const
{
    return m_groundBoundingBox.getGlobalBounds();
}

sf::Vector2f Player::getPosition() const
{
    return m_playerSprite.getPosition();
}

void Player::setupPlayer()
{
    m_playerSprite.setTexture(m_idleTexture);
    m_playerSprite.setScale(2, 2);
    m_playerSprite.setOrigin(60, 40);

    // Set up the hitbox
    m_boundingBox.setSize(sf::Vector2f(40, 70));
    m_boundingBox.setFillColor(sf::Color::Transparent);
    m_boundingBox.setOutlineColor(sf::Color::Yellow);
    m_boundingBox.setOutlineThickness(1.0f);

    // Set up the ground hit box
    m_groundBoundingBox.setSize(sf::Vector2f(40, 5));
    m_groundBoundingBox.setFillColor(sf::Color::Transparent);
    m_groundBoundingBox.setOutlineColor(sf::Color::Magenta);
    m_groundBoundingBox.setOutlineThickness(1.0f);
}

void Player::updateAnimation(sf::Time t_deltaTime)
{
    m_currentFrameTime += t_deltaTime;

    if (m_currentFrameTime >= m_frameTime)
    {
        m_currentFrameTime = sf::Time::Zero;
        m_currentFrame = (m_currentFrame + 1) % m_frameCount;

        switch (m_animationState)
        {
        case PlayerState::Idle:
            m_playerSprite.setTextureRect(m_idleFrames[m_currentFrame]);
            break;
        case PlayerState::Jumping:
            m_playerSprite.setTextureRect(m_jumpFrames[m_currentFrame]);
            break;
        case PlayerState::Running:
            m_playerSprite.setTextureRect(m_runFrames[m_currentFrame]);
            break;
        case PlayerState::Falling:
            m_playerSprite.setTextureRect(m_fallingFrames[m_currentFrame]);
            break;
        case PlayerState::Attacking:
            m_playerSprite.setTextureRect(m_attackFrames[m_currentFrame]);
            break;
        }
    }
}

void Player::loadTextures()
{
    if (!m_idleTexture.loadFromFile("Assets\\Images\\Player\\player_idle.png"))
    {
        std::cout << "problem loading player idle image" << std::endl;
    }

    if (!m_jumpTexture.loadFromFile("Assets\\Images\\Player\\player_jump.png"))
    {
        std::cout << "problem loading player jump image" << std::endl;
    }

    if (!m_runTexture.loadFromFile("Assets\\Images\\Player\\player_run.png"))
    {
        std::cout << "problem loading player run image" << std::endl;
    }

    if (!m_fallingTexture.loadFromFile("Assets\\Images\\Player\\player_falling.png"))
    {
        std::cout << "problem loading player falling image" << std::endl;
    }

    if (!m_attackTexture.loadFromFile("Assets\\Images\\Player\\player_attack1.png"))
    {
        std::cout << "problem loading player attack image" << std::endl;
    }
}

void Player::loadFrames()
{
    const int frameWidth = 120;
    const int frameHeight = 80;

    for (int i = 0; i < 10; ++i)
    {
        m_idleFrames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }

    for (int i = 0; i < 3; ++i)
    {
        m_jumpFrames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }

    for (int i = 0; i < 10; ++i)
    {
        m_runFrames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }

    for (int i = 0; i < 3; ++i)
    {
        m_fallingFrames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }

    for (int i = 0; i < 4; ++i)
    {
        m_attackFrames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }
}

void Player::updateBoundingBox()
{
    sf::Vector2f spritePos = m_playerSprite.getPosition();
    m_boundingBox.setPosition(spritePos.x - m_boundingBox.getSize().x / 2, spritePos.y + 5);
    m_groundBoundingBox.setPosition(spritePos.x - m_groundBoundingBox.getSize().x / 2, spritePos.y + m_boundingBox.getSize().y + 6);
}

void Player::applyGravity(sf::Time t_deltaTime, const std::vector<Block>& m_gameBlocks)
{
    if (m_isJumping || m_isFalling)
    {
        m_verticalSpeed += m_gravity;
        sf::Vector2f newPos = m_playerSprite.getPosition();
        newPos.y += m_verticalSpeed * t_deltaTime.asSeconds();

        sf::FloatRect newBounds = m_boundingBox.getGlobalBounds();
        newBounds.top += m_verticalSpeed * t_deltaTime.asSeconds();

        bool collisionDetected = false;
        for (const auto& block : m_gameBlocks)
        {
            if (block.shape.getGlobalBounds().intersects(newBounds))
            {
                collisionDetected = true;
                if (m_verticalSpeed > 0) // Falling down
                {
                    newPos.y = block.shape.getPosition().y - m_boundingBox.getSize().y - 10;
                    m_isFalling = false;
                    m_isJumping = false;
                    m_verticalSpeed = 0;
                    m_animationState = PlayerState::Idle;
                }
                else if (m_verticalSpeed < 0) // Jumping up
                {
                    newPos.y = block.shape.getPosition().y + block.shape.getSize().y;
                    m_verticalSpeed = 0;
                    m_isJumping = true;
                    m_isFalling = false;
                }
                break;
            }
        }

        if (!collisionDetected)
        {
            if (m_verticalSpeed > 0)
            {
                m_isFalling = true;
                m_isJumping = false;
                m_animationState = PlayerState::Falling;
                m_playerSprite.setTexture(m_fallingTexture);
                m_frameCount = 3;
            }
        }

        m_playerSprite.setPosition(newPos);
        updateBoundingBox();
    }
    else
    {
        // Check for ground detection
        sf::FloatRect groundDetectionBounds = getGroundDetectionBox();
        bool onGround = false;
        for (const auto& block : m_gameBlocks)
        {
            if (block.shape.getGlobalBounds().intersects(groundDetectionBounds))
            {
                onGround = true;
                break;
            }
        }
        if (!onGround)
        {
            m_isFalling = true;
        }
    }
}

void Player::updateAnimationFrame()
{
    if (m_currentFrame >= 0 && m_currentFrame < getFrameCountForState(m_animationState))
    {
        switch (m_animationState)
        {
        case PlayerState::Idle:
            m_playerSprite.setTextureRect(m_idleFrames[m_currentFrame]);
            break;
        case PlayerState::Jumping:
            m_playerSprite.setTextureRect(m_jumpFrames[m_currentFrame]);
            break;
        case PlayerState::Running:
            m_playerSprite.setTextureRect(m_runFrames[m_currentFrame]);
            break;
        case PlayerState::Falling:
            m_playerSprite.setTextureRect(m_fallingFrames[m_currentFrame]);
            break;
        case PlayerState::Attacking:
            m_playerSprite.setTextureRect(m_attackFrames[m_currentFrame]);
            break;
        }
    }
}

void Player::updateFacingDirection(float m_x)
{
    if (m_x < m_previousX) // Moving left
    {
        if (m_otherPlayerFacingDirection != -1)
        {
            m_playerSprite.setScale(-2, 2);
            m_otherPlayerFacingDirection = -1;
        }
    }
    else if (m_x > m_previousX) // Moving right
    {
        if (m_otherPlayerFacingDirection != 1)
        {
            m_playerSprite.setScale(2, 2);
            m_otherPlayerFacingDirection = 1;
        }
    }
    m_previousX = m_x;
}

int Player::getFrameCountForState(PlayerState m_state) const
{
    switch (m_state)
    {
    case PlayerState::Idle:
        return m_idleFrames.size();
    case PlayerState::Jumping:
        return m_jumpFrames.size();
    case PlayerState::Running:
        return m_runFrames.size();
    case PlayerState::Falling:
        return m_fallingFrames.size();
    case PlayerState::Attacking:
        return m_attackFrames.size();
    default:
        return 0;
    }
}