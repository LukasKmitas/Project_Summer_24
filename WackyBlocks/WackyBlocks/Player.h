#ifndef PLAYER_H
#define PLAYER_H
#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Block.h"
#include "SoundManager.h"

enum class PlayerState
{
    Idle,
    Jumping,
    Running,
    Falling,
    Attacking,
    Attacking2
};

enum class AttackState
{
    None,
    Attack1,
    Attack2,
};

struct Bullet
{
    sf::CircleShape shape;
    sf::Vector2f direction;
};

struct EnergyWave
{
    sf::Sprite sprite;
    sf::Vector2f direction;
    sf::Time lifetime;
    sf::Time elapsedTime;
};

class Player
{
public:

    Player();
    ~Player();

    void update(sf::Time t_deltaTime, std::vector<Block>& m_gameBlocks);
    void render(sf::RenderWindow& m_window);
    void renderHealthUI(sf::RenderWindow& m_window);

    void setPosition(float m_x, float m_y);
    void setOtherPlayerColor();
    void setCurrentFrame(int m_frame);
    void setState(PlayerState m_state);
    void updateFacingDirection(float m_x);
    void takeDamage(int m_amount);
    void heal(int m_amount);
    void shootBullet(const sf::Vector2f& m_target);
    void applyKnockback(const sf::Vector2f& m_force, sf::Time m_duration);

    bool isNearShop(const sf::Vector2f& m_shopPosition) const;
    void upgradeHealth();
    void upgradeBullets();
    void upgradeAmmo();
    void upgradeDoubleJump();
    void upgradeEnergyWaveAttack();

    int getCurrentFrame() const;
    int getFrameCountForState(PlayerState m_state) const;
    int getHealth() const;
    bool isPlayerAlive() const;
    PlayerState getState() const;

    sf::FloatRect getBoundingBox() const;
    sf::FloatRect getGroundDetectionBox() const;
    sf::Vector2f getPosition() const;
    std::vector<Bullet>& getBullets();

private:

    PlayerState m_animationState;
    AttackState m_currentAttackState = AttackState::None;
    std::vector<Bullet> m_bullets;
    std::vector<EnergyWave> m_energyWaves;

    void handleInput(sf::Time t_deltaTime, std::vector<Block>& m_gameBlocks);
    void setupPlayer();
    void setupHealth();
    void setupAmmo();
    void updateAnimation(sf::Time t_deltaTime);
    void loadTextures();
    void loadFrames();
    void updateBoundingBox();
    void applyGravity(sf::Time t_deltaTime, const std::vector<Block>& m_gameBlocks);
    void updateAnimationFrame();
    void updateHealthBar();
    void updateAmmoText();
    void updateBullets(sf::Time t_deltaTime, std::vector<Block>& m_gameBlocks);
    void startFirstAttack();
    void startSecondAttack();
    void resetCombo();
    void launchEnergyWave();
    void updateEnergyWaves(sf::Time t_deltaTime, const std::vector<Block>& m_gameBlocks);
    void refillAmmo(int m_ammoAmount);
    void updateOnAmmoPacks(std::vector<Block>& m_gameBlocks);
    void updateOnHealthPacks(std::vector<Block>& m_gameBlocks);

    sf::Sprite m_playerSprite;
    sf::RectangleShape m_boundingBox;
    sf::RectangleShape m_groundBoundingBox;
    sf::RectangleShape m_attackCollisionBox;
    sf::RectangleShape m_healthBarBackground;
    sf::RectangleShape m_healthBar;
    sf::RectangleShape m_secondaryHealthBarBackground;
    sf::RectangleShape m_secondaryHealthBar;
    sf::Texture m_idleTexture;
    sf::Texture m_jumpTexture;
    sf::Texture m_runTexture;
    sf::Texture m_fallingTexture;
    sf::Texture m_attackTexture;
    sf::Texture m_attack2Texture;
    sf::Texture m_energyWaveTexture;

    sf::Font m_font;
    sf::Text m_healthText;
    sf::Text m_ammoText;

    // Animation data
    std::vector<sf::IntRect> m_idleFrames;
    std::vector<sf::IntRect> m_jumpFrames;
    std::vector<sf::IntRect> m_runFrames;
    std::vector<sf::IntRect> m_fallingFrames;
    std::vector<sf::IntRect> m_attackFrames;
    std::vector<sf::IntRect> m_attack2Frames;

    sf::Time m_frameTime = sf::seconds(0.1f);
    sf::Time m_currentFrameTime = sf::Time::Zero;
    sf::Time m_attackDuration = sf::seconds(0.4f);
    sf::Time m_attackElapsed = sf::Time::Zero;
    sf::Time m_doubleJumpDelay = sf::seconds(0.2f);
    sf::Time m_jumpElapsedTime = sf::Time::Zero;
    sf::Time m_comboDelay = sf::seconds(0.1f);
    sf::Time m_comboTimeWindow = sf::seconds(0.4f);
    sf::Time m_comboElapsedTime = sf::Time::Zero;
    sf::Time m_knockbackDuration;
    sf::Time m_knockbackElapsedTime = sf::Time::Zero;

    sf::Vector2f m_knockbackForce;

    int m_currentFrame = 0;
    int m_frameCount = 0;
  
    int m_otherPlayerFacingDirection = 1;
    int m_health = 100;
    int m_maxHealth = 100;
    int m_currentAmmo = 100;
    int m_totalAmmo = 100;
    int m_extraBulletCount = 0;
    float m_speed = 100.0f;
    float m_bulletSpeed = 500.0f;
    float m_jumpSpeed = 350.0f;
    float m_gravity = 10.0f;
    float m_verticalSpeed = 0.0f;
    float m_previousX = 0.0f;

    bool m_isJumping = false;
    bool m_isFalling = false;
    bool m_facingDirection = true;
    bool m_isAttacking = false;
    bool m_isAttacking2 = false;
    bool m_secondaryHealthBarVisible = false;
    bool m_attackCollisionActivated = false;
    bool m_doubleJumpUnlocked = false;
    bool m_canDoubleJump = true;
    bool m_firstJumpOn = false;
    bool m_canComboAttack = false;
    bool m_unlockedEnergyWaveAttack = false;
    bool m_isPlayerDead = false;
    bool m_isKnockedBack = false;

};

#endif
