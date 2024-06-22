#pragma once
#ifndef PARTICLEMANAGER_H
#define PARTICLEMANAGER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Global.h"

enum class ParticleType
{
    FIREFLY,
    BOUNCY_BALL
};

class Particle
{
public:

    ParticleType m_type;

    void update(sf::Time t_deltaTime, std::vector<Particle>& m_particles);
    void render(sf::RenderWindow& m_window);

    Particle(sf::Shape* m_shape, const sf::Vector2f& m_velocity, const sf::Color& m_color, float m_lifetime, ParticleType m_type);
    Particle(sf::Sprite* m_sprite, const sf::Vector2f& m_velocity, const sf::Color& m_color, float m_lifetime, ParticleType m_type);
    Particle(sf::Shader* m_shader, const sf::Vector2f& m_velocity, const sf::Color& m_color, float m_lifetime, ParticleType m_type);

    bool isColliding(const Particle& m_other);
    void handleCollision(Particle& m_other);

    sf::Shape* m_shape = nullptr;
    sf::Sprite* m_sprite = nullptr;
    sf::Shader* m_shader = nullptr;
    sf::Vector2f m_velocity;
    sf::Color m_color;

    float m_lifetime;
    float m_age = 0;
    float m_alpha = 255;
};

class ParticleManager
{
public:

    ParticleManager();
    ~ParticleManager();

    void update(sf::Time t_deltaTime, ParticleType m_type);
    void render(sf::RenderWindow& m_window, ParticleType m_type);

    void addParticle(sf::Shape* m_shape, const sf::Vector2f& m_velocity, const sf::Color& m_color, float m_lifetime, ParticleType m_type);
    void addParticle(sf::Sprite* m_sprite, const sf::Vector2f& m_velocity, const sf::Color& m_color, float m_lifetime, ParticleType m_type);
    void addParticle(sf::Shader* m_shader, const sf::Vector2f& m_velocity, const sf::Color& m_color, float m_lifetime, ParticleType m_type);

    void createBackgroundEffect();
    void createBouncyBallEffect(const sf::Vector2f& m_position);

    void clearParticles(ParticleType m_type);
    void clearAllParticles();

private:

    std::vector<Particle> m_fireflyParticles;
    std::vector<Particle> m_bouncyBallParticles;

};

#endif
