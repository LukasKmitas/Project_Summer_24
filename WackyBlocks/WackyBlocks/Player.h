#ifndef PLAYER_H
#define PLAYER_H
#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Block.h"

enum class PlayerState
{
    Idle,
    Jumping,
    Running,
    Falling,
    Attacking
};

class Player
{
public:

    Player();
    ~Player();

    void update(sf::Time t_deltaTime, const std::vector<Block>& m_gameBlocks);
    void render(sf::RenderWindow& m_window);
    void handleInput(sf::Time t_deltaTime, const std::vector<Block>& m_gameBlocks);

    void setPosition(float m_x, float m_y);
    void setOtherPlayerColor();
    void setCurrentFrame(int m_frame);
    void setState(PlayerState m_state);
    void updateFacingDirection(float m_x);

    int getCurrentFrame() const;
    int getFrameCountForState(PlayerState m_state) const;
    PlayerState getState() const;

    sf::FloatRect getBoundingBox() const;
    sf::FloatRect getGroundDetectionBox() const;
    sf::Vector2f getPosition() const;

private:

    PlayerState m_animationState;

    void setupPlayer();
    void updateAnimation(sf::Time t_deltaTime);
    void loadTextures();
    void loadFrames();
    void updateBoundingBox();
    void applyGravity(sf::Time t_deltaTime, const std::vector<Block>& m_gameBlocks);
    void updateAnimationFrame();

    sf::Sprite m_playerSprite;
    sf::RectangleShape m_boundingBox;
    sf::RectangleShape m_groundBoundingBox;
    sf::Texture m_idleTexture;
    sf::Texture m_jumpTexture;
    sf::Texture m_runTexture;
    sf::Texture m_attackTexture;
    sf::Texture m_fallingTexture;

    // Animation data
    std::vector<sf::IntRect> m_idleFrames;
    std::vector<sf::IntRect> m_jumpFrames;
    std::vector<sf::IntRect> m_runFrames;
    std::vector<sf::IntRect> m_attackFrames;
    std::vector<sf::IntRect> m_fallingFrames;

    sf::Time m_frameTime = sf::seconds(0.1f);
    sf::Time m_currentFrameTime = sf::Time::Zero;
    int m_currentFrame = 0;
    int m_frameCount = 0;
  
    int m_otherPlayerFacingDirection = 1;
    float m_speed = 100.0f;
    float m_jumpSpeed = 350.0f;
    float m_gravity = 10.0f;
    float m_verticalSpeed = 0.0f;
    float m_previousX = 0.0f;
    bool m_isJumping = false;
    bool m_isFalling = false;
    bool m_facingDirection = true;

};

#endif
