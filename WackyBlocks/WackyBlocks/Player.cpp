#include "Player.h"

Player::Player()
    : m_animationState(PlayerState::Idle)
{
    setupPlayer();
    loadTextures();
    loadFrames();
    updateBoundingBox();
    setupHealth();
    setupAmmo();
    m_playerSprite.setTextureRect(m_idleFrames[0]);
}

Player::~Player()
{
}


void Player::update(sf::Time t_deltaTime, std::vector<Block>& m_gameBlocks)
{
    if (!m_isAttacking)
    {
        handleInput(t_deltaTime, m_gameBlocks);
        applyGravity(t_deltaTime, m_gameBlocks);
    }
    updateAnimation(t_deltaTime);
    updateBoundingBox();
    updateHealthBar();
    updateBullets(t_deltaTime, m_gameBlocks);
    updateEnergyWaves(t_deltaTime, m_gameBlocks);
    updateOnAmmoPacks(m_gameBlocks);
    updateOnHealthPacks(m_gameBlocks);
}

void Player::render(sf::RenderWindow& m_window)
{
    m_window.draw(m_playerSprite);
    m_window.draw(m_boundingBox);
    m_window.draw(m_groundBoundingBox);
    m_window.draw(m_attackCollisionBox);
    for (const auto& bullet : m_bullets)
    {
        m_window.draw(bullet.shape);
    }
}

void Player::renderHealthUI(sf::RenderWindow& m_window)
{
    m_window.draw(m_healthBarBackground);
    m_window.draw(m_healthBar);
    if (m_secondaryHealthBarVisible)
    {
        m_window.draw(m_secondaryHealthBarBackground);
        m_window.draw(m_secondaryHealthBar);
    }
    m_window.draw(m_healthText);
    m_window.draw(m_ammoText);
    for (const auto& wave : m_energyWaves)
    {
        m_window.draw(wave.sprite);
    }
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

void Player::handleInput(sf::Time t_deltaTime, std::vector<Block>& m_gameBlocks)
{
    bool moving = false;
    sf::Vector2f movement(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !m_isJumping && !m_isFalling)
    {
        if (m_currentAttackState == AttackState::None) // First attack
        {
            startFirstAttack();
        }
        else if (m_currentAttackState == AttackState::Attack1 && m_canComboAttack) // Second attack
        {
            startSecondAttack();
        }
    }

    // Reset combo if no input received within the first attack duration
    if (m_currentAttackState == AttackState::Attack1 && !sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    {
        m_comboElapsedTime += t_deltaTime;
        if (m_comboElapsedTime >= m_comboTimeWindow)
        {
            resetCombo();
        }
    }

    // First jump
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && !m_isJumping && !m_isFalling)
    {
        m_animationState = PlayerState::Jumping;
        m_playerSprite.setTexture(m_jumpTexture);
        m_frameCount = m_jumpFrames.size();
        m_verticalSpeed = -m_jumpSpeed;
        m_isJumping = true;
        m_isFalling = false;
        m_firstJumpOn = true;
        m_canDoubleJump = false;
        m_jumpElapsedTime = sf::Time::Zero;
        std::cout << "First JUMP" << std::endl;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && m_doubleJumpUnlocked && m_canDoubleJump) // Double jump (upgradable item)
    {
        if (m_jumpElapsedTime >= m_doubleJumpDelay)
        {
            m_animationState = PlayerState::Jumping;
            m_playerSprite.setTexture(m_jumpTexture);
            m_frameCount = m_jumpFrames.size();
            m_verticalSpeed = -m_jumpSpeed;
            m_isJumping = true;
            m_isFalling = false;
            m_canDoubleJump = false;
            m_firstJumpOn = false;
            std::cout << "Second JUMP" << std::endl;
        }
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
    else if (!m_isJumping && !m_isFalling && !m_isAttacking && !m_isAttacking2)
    {
        m_animationState = PlayerState::Idle;
        m_playerSprite.setTexture(m_idleTexture);
        m_frameCount = m_idleFrames.size();
    }

    if (moving)
    {
        if (!m_isJumping && !m_isFalling)
        {
            m_animationState = PlayerState::Running;
            m_playerSprite.setTexture(m_runTexture);
            m_frameCount = m_runFrames.size();
        }

        sf::Vector2f newPos = m_playerSprite.getPosition() + movement;
        sf::FloatRect newBounds = m_boundingBox.getGlobalBounds();
        newBounds.left += movement.x;

        bool collisionDetected = false;
        for (const auto& block : m_gameBlocks)
        {
            if (!block.traversable && block.shape.getGlobalBounds().intersects(newBounds))
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

    // Update jump timer
    if (m_firstJumpOn)
    {
        m_jumpElapsedTime += t_deltaTime;
        if (m_jumpElapsedTime >= m_doubleJumpDelay)
        {
            m_canDoubleJump = true;
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

std::vector<Bullet>& Player::getBullets()
{
    return m_bullets;
}

void Player::setupPlayer()
{
    if (!m_font.loadFromFile("Assets\\Fonts\\ariblk.ttf"))
    {
        std::cout << "Error loading font" << std::endl;
    }

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

    m_attackCollisionBox.setSize(sf::Vector2f(100, 70));
    m_attackCollisionBox.setFillColor(sf::Color::Transparent);
    m_attackCollisionBox.setOutlineColor(sf::Color::Red);
    m_attackCollisionBox.setOutlineThickness(1.0f);
}

void Player::setupHealth()
{
    m_healthBarBackground.setSize(sf::Vector2f(300, 20));
    m_healthBarBackground.setFillColor(sf::Color::Black);
    m_healthBarBackground.setOutlineColor(sf::Color::White);
    m_healthBarBackground.setOutlineThickness(1.0f);
    m_healthBarBackground.setPosition(10, 10);
    m_healthBar.setSize(sf::Vector2f(300, 20));
    m_healthBar.setFillColor(sf::Color::Green);
    m_healthBar.setPosition(10, 10);

    // hidden at the start (upgrade item)
    m_secondaryHealthBarBackground.setSize(sf::Vector2f(60, 20));
    m_secondaryHealthBarBackground.setFillColor(sf::Color::Black);
    m_secondaryHealthBarBackground.setOutlineColor(sf::Color::White);
    m_secondaryHealthBarBackground.setOutlineThickness(1.0f);
    m_secondaryHealthBarBackground.setPosition(320, 10);
    m_secondaryHealthBar.setSize(sf::Vector2f(60, 20));
    m_secondaryHealthBar.setFillColor(sf::Color::Green);
    m_secondaryHealthBar.setPosition(320, 10);

    m_healthText.setFont(m_font);
    m_healthText.setString("Health");
    m_healthText.setCharacterSize(15);
    m_healthText.setFillColor(sf::Color(192, 192, 192, 255));
    m_healthText.setPosition(150 - m_healthText.getGlobalBounds().width / 2, 10);
}

void Player::setupAmmo()
{
    m_ammoText.setFont(m_font);
    m_ammoText.setCharacterSize(15);
    m_ammoText.setFillColor(sf::Color::White);
    m_ammoText.setPosition(10, 35);
    updateAmmoText();
}

void Player::updateAnimation(sf::Time t_deltaTime)
{
    m_currentFrameTime += t_deltaTime;

    if (m_isAttacking || m_isAttacking2)
    {
        m_attackElapsed += t_deltaTime;

        // Check for attack duration completion
        if (m_isAttacking && m_attackElapsed >= m_attackDuration)
        {
            m_isAttacking = false;
            m_canComboAttack = true;
            std::cout << "First Attack Complete, ready for 2nd attack" << std::endl;
        }
        else if (m_isAttacking2 && m_attackElapsed >= m_attackDuration * 1.5f)
        {
            resetCombo();
        }
    }

    if (m_currentFrameTime >= m_frameTime)
    {
        m_currentFrameTime -= m_frameTime;
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
            std::cout << "FRAME A1: " << m_currentFrame << std::endl;
            if (m_currentFrame == 1)
            {
                m_attackCollisionActivated = true;
                m_attackCollisionBox.setOutlineColor(sf::Color::Green);
                if (m_unlockedEnergyWaveAttack)
                {
                    launchEnergyWave();
                }
                std::cout << "FIRST RECT" << std::endl;
            }
            else
            {
                m_attackCollisionActivated = false;
                m_attackCollisionBox.setOutlineColor(sf::Color::Red);
            }
            m_playerSprite.setTextureRect(m_attackFrames[m_currentFrame]);
            break;
        case PlayerState::Attacking2:
            std::cout << "FRAME A2: " << m_currentFrame << std::endl;
            if (m_currentFrame == 2)
            {
                m_attackCollisionActivated = true;
                m_attackCollisionBox.setOutlineColor(sf::Color::Green);
                if (m_unlockedEnergyWaveAttack)
                {
                    launchEnergyWave();
                }
                std::cout << "FIRST RECT" << std::endl;
            }
            else
            {
                m_attackCollisionActivated = false;
                m_attackCollisionBox.setOutlineColor(sf::Color::Red);
            }
            m_playerSprite.setTextureRect(m_attack2Frames[m_currentFrame]);
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

    if (!m_attack2Texture.loadFromFile("Assets\\Images\\Player\\player_attack2.png"))
    {
        std::cout << "problem loading player attack2 image" << std::endl;
    }

    if (!m_energyWaveTexture.loadFromFile("Assets/Images/Player/shotwave.png"))
    {
        std::cout << "Error loading energy wave texture" << std::endl;
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

    for (int i = 0; i < 6; ++i)
    {
        m_attack2Frames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }
}

void Player::updateBoundingBox()
{
    sf::Vector2f spritePos = m_playerSprite.getPosition();
    m_boundingBox.setPosition(spritePos.x - m_boundingBox.getSize().x / 2, spritePos.y + 5);
    m_groundBoundingBox.setPosition(spritePos.x - m_groundBoundingBox.getSize().x / 2, spritePos.y + m_boundingBox.getSize().y + 6);
    if (m_facingDirection)
    {
        spritePos.x += 20;  // Adjust for right-facing
    }
    else
    {
        spritePos.x -= 120; // Adjust for left-facing
    }
    m_attackCollisionBox.setPosition(spritePos.x, spritePos.y + 5);
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
            if (!block.traversable && block.shape.getGlobalBounds().intersects(newBounds))
            {
                collisionDetected = true;
                if (m_verticalSpeed > 0) // Falling down
                {
                    newPos.y = block.shape.getPosition().y - m_boundingBox.getSize().y - 50;
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
                m_frameCount = m_fallingFrames.size();
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
            if (!block.traversable && block.shape.getGlobalBounds().intersects(groundDetectionBounds))
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
        case PlayerState::Attacking2:
            m_playerSprite.setTextureRect(m_attack2Frames[m_currentFrame]);
            break;
        }
    }
}

void Player::updateHealthBar()
{
    float healthPercentage = static_cast<float>(m_health) / m_maxHealth;
    float secondaryHealthPercentage = std::min(1.0f, healthPercentage / 0.2f);
    float mainHealthPercentage = std::max(0.0f, (healthPercentage - 0.2f) / 0.8f);

    m_healthBar.setSize(sf::Vector2f(300 * mainHealthPercentage, 20));

    if (m_secondaryHealthBarVisible) // Update only if secondary bar is visible
    {
        m_secondaryHealthBar.setSize(sf::Vector2f(60 * secondaryHealthPercentage, 20));
    }

    if (mainHealthPercentage > 0.5f)
    {
        m_healthBar.setFillColor(sf::Color::Green);
    }
    else if (mainHealthPercentage > 0.25f)
    {
        m_healthBar.setFillColor(sf::Color::Yellow);
    }
    else
    {
        m_healthBar.setFillColor(sf::Color::Red);
    }
}

void Player::updateAmmoText()
{
    m_ammoText.setString("Ammo: " + std::to_string(m_currentAmmo) + " / " + std::to_string(m_totalAmmo));
}

void Player::shootBullet(const sf::Vector2f& m_target)
{
    if (m_currentAmmo > 0)
    {
        sf::Vector2f direction = m_target - m_playerSprite.getPosition();
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        direction /= length; // Normalize
        direction *= m_bulletSpeed;

        // Main bullet
        Bullet mainBullet;
        mainBullet.shape.setRadius(5);
        mainBullet.shape.setFillColor(sf::Color::Yellow);
        mainBullet.shape.setPosition(m_playerSprite.getPosition());
        mainBullet.shape.setOrigin(2.5f, 2.5f);
        mainBullet.direction = direction;
        m_bullets.push_back(mainBullet);

        // Extra bullets (upgrade item)
        float offset = 0.2f;
        for (int i = 1; i <= m_extraBulletCount; ++i)
        {
            Bullet extraBullet = mainBullet;
            sf::Vector2f extraDirection = direction;
            if (i % 2 == 1)
            {
                extraDirection.y -= offset * length;
            }
            else
            {
                extraDirection.y += offset * length;
            }
            extraBullet.direction = extraDirection;
            m_bullets.push_back(extraBullet);
        }

        --m_currentAmmo;
        updateAmmoText();
    }
}

void Player::refillAmmo(int m_ammoAmount)
{
    m_currentAmmo += m_ammoAmount;
    if (m_currentAmmo > m_totalAmmo)
    {
        m_currentAmmo = m_totalAmmo;
    }
    updateAmmoText();
}

void Player::updateBullets(sf::Time t_deltaTime, std::vector<Block>& m_gameBlocks)
{
    for (auto bulletIt = m_bullets.begin(); bulletIt != m_bullets.end();)
    {
        Bullet& bullet = *bulletIt;
        bullet.shape.move(bullet.direction * t_deltaTime.asSeconds());

        bool collision = false;
        for (auto blockIt = m_gameBlocks.begin(); blockIt != m_gameBlocks.end();)
        {
            Block& block = *blockIt;

            if (!block.traversable && bullet.shape.getGlobalBounds().intersects(block.shape.getGlobalBounds()))
            {
                collision = true;
                bool destroyed = block.takeDamage(10);

                if (destroyed)
                {
                    blockIt = m_gameBlocks.erase(blockIt);
                    std::cout << "Block destroyed" << std::endl;
                }
                else
                {
                    ++blockIt;
                }
                break; 
            }
            else
            {
                ++blockIt;
            }
        }

        if (collision)
        {
            bulletIt = m_bullets.erase(bulletIt);
        }
        else
        {
            ++bulletIt;
        }
    }
}

void Player::startFirstAttack()
{
    m_isAttacking = true;
    m_attackElapsed = sf::Time::Zero;
    m_animationState = PlayerState::Attacking;
    m_currentAttackState = AttackState::Attack1;
    m_playerSprite.setTexture(m_attackTexture);
    m_frameCount = m_attackFrames.size();
    m_currentFrame = 0;
    std::cout << "Start First Attack" << std::endl;
}

void Player::startSecondAttack()
{
    m_isAttacking = false;
    m_isAttacking2 = true;
    m_attackElapsed = sf::Time::Zero;
    m_animationState = PlayerState::Attacking2;
    m_currentAttackState = AttackState::Attack2;
    m_playerSprite.setTexture(m_attack2Texture);
    m_frameCount = m_attack2Frames.size();
    m_currentFrame = 0;
    std::cout << "Start Second Attack" << std::endl;
}

void Player::resetCombo()
{
    m_isAttacking = false;
    m_isAttacking2 = false;
    m_canComboAttack = false;
    m_currentAttackState = AttackState::None;
    m_animationState = PlayerState::Idle;
    m_playerSprite.setTexture(m_idleTexture);
    m_frameCount = m_idleFrames.size();
    m_currentFrame = 0;
    std::cout << "Combo Reset" << std::endl;
}

void Player::launchEnergyWave()
{
    if (m_attackCollisionActivated)
    {
        EnergyWave wave;
        wave.sprite.setTexture(m_energyWaveTexture);
        wave.sprite.setOrigin(m_energyWaveTexture.getSize().x / 2, m_energyWaveTexture.getSize().y / 2);
        wave.sprite.setScale(2.5, 2.5);
        wave.sprite.setPosition(m_playerSprite.getPosition() + sf::Vector2f(0, 30));

        float speed = 300.0f;
        if (m_facingDirection) 
        {
            wave.direction = sf::Vector2f(speed, 0.0f);
        }
        else
        {
            wave.direction = sf::Vector2f(-speed, 0.0f);
            wave.sprite.setScale(-2.5f, 2.5f);
        }

        wave.lifetime = sf::seconds(2.0f);
        wave.elapsedTime = sf::Time::Zero;

        m_energyWaves.push_back(wave);

        std::cout << "Energy Wave Launched!" << std::endl;
    }
}

void Player::updateEnergyWaves(sf::Time t_deltaTime, const std::vector<Block>& m_gameBlocks)
{
    for (auto it = m_energyWaves.begin(); it != m_energyWaves.end();)
    {
        it->sprite.move(it->direction * t_deltaTime.asSeconds());
        it->elapsedTime += t_deltaTime;

        bool shouldRemove = false;

        if (it->elapsedTime >= it->lifetime)
        {
            shouldRemove = true;
        }
        else
        {
            sf::FloatRect waveBounds = it->sprite.getGlobalBounds();
            for (const auto& block : m_gameBlocks)
            {
                if (block.shape.getGlobalBounds().intersects(waveBounds))
                {
                    shouldRemove = true;
                    break;
                }
            }
        }

        if (shouldRemove)
        {
            it = m_energyWaves.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void Player::updateOnAmmoPacks(std::vector<Block>& m_gameBlocks)
{
    for (auto blockIt = m_gameBlocks.begin(); blockIt != m_gameBlocks.end();)
    {
        Block& block = *blockIt;

        if (block.type == BlockType::AMMO_PACK)
        {
            if (getBoundingBox().intersects(block.shape.getGlobalBounds()))
            {
                if (m_currentAmmo < m_totalAmmo)
                {
                    refillAmmo(30);

                    blockIt = m_gameBlocks.erase(blockIt);
                    continue;
                }
            }
        }
     
        ++blockIt;
    }
}

void Player::updateOnHealthPacks(std::vector<Block>& m_gameBlocks)
{
    for (auto blockIt = m_gameBlocks.begin(); blockIt != m_gameBlocks.end();)
    {
        Block& block = *blockIt;

        if (block.type == BlockType::HEALTH_PACK)
        {
            if (getBoundingBox().intersects(block.shape.getGlobalBounds()))
            {
                if (m_health < m_maxHealth)
                {
                    heal(60);

                    blockIt = m_gameBlocks.erase(blockIt);
                    continue;
                }
            }
        }

        ++blockIt;
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

void Player::takeDamage(int m_amount)
{
    m_health -= m_amount;
    if (m_health < 0)
    {
        m_health = 0;
        m_isPlayerDead = true;
    }
    updateHealthBar();
}

void Player::heal(int m_amount)
{
    m_health += m_amount;
    if (m_health > m_maxHealth)
    {
        m_health = m_maxHealth;
    }
    updateHealthBar();
}

bool Player::isNearShop(const sf::Vector2f& m_shopPosition) const
{
    float distance = std::sqrt(std::pow(m_shopPosition.x - m_playerSprite.getPosition().x, 2) + 
                               std::pow(m_shopPosition.y - m_playerSprite.getPosition().y, 2));
    return distance < 50.0f;
}

void Player::upgradeHealth()
{
    m_maxHealth = static_cast<int>(m_maxHealth * 1.2); // Increase max health by 20%
    m_health = m_maxHealth;
    m_secondaryHealthBarVisible = true;
    updateHealthBar();
}

void Player::upgradeBullets()
{
    ++m_extraBulletCount;
}

void Player::upgradeAmmo()
{
    m_totalAmmo += 20;
    updateAmmoText();
}

void Player::upgradeDoubleJump()
{
    m_doubleJumpUnlocked = true;
}

void Player::upgradeEnergyWaveAttack()
{
    m_unlockedEnergyWaveAttack = true;
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
    case PlayerState::Attacking2:
        return m_attack2Frames.size();
    default:
        return 0;
    }
}

int Player::getHealth() const
{
    return m_health;
}

bool Player::isPlayerAlive() const
{
    return m_isPlayerDead;
}
