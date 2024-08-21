#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Player.h"
#include "Enemy.h"

class TrapSpikes
{
public:

    TrapSpikes(const sf::Vector2f& m_position);

    void update(sf::Time m_deltaTime, Player& m_player, std::vector<std::unique_ptr<Enemy>>& m_enemies);
    void render(sf::RenderWindow& m_window);

    sf::FloatRect getBoundingBox() const;

private:

    void setupSpikes();

    sf::Sprite m_sprite;
    sf::Texture m_texture;

    std::vector<sf::IntRect> m_frames;
    sf::Clock m_animationClock;
    sf::Time m_frameTime = sf::seconds(0.1f);
    sf::Time m_delayTime = sf::seconds(3.0f);
    sf::Time m_currentDelayTime = sf::Time::Zero;
    int m_currentFrame = 0;

    sf::FloatRect m_hitbox;
    int m_damage = 20;
    bool m_dealtDamage = false;
    bool m_isAnimating = true;
};

