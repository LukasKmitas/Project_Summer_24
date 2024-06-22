#include "ParticleManager.h"

void Particle::update(sf::Time t_deltaTime, std::vector<Particle>& m_particles)
{
    m_age += t_deltaTime.asSeconds();

    if (m_shape)
    {
        if (m_type == ParticleType::BOUNCY_BALL)
        {
            // Apply gravity
            m_velocity.y += 981.0f * t_deltaTime.asSeconds();

            // Check for collisions with other particles
            for (auto& other : m_particles)
            {
                if (&other != this && other.m_type == ParticleType::BOUNCY_BALL && isColliding(other))
                {
                    handleCollision(other);
                }
            }

            // Bounce off screen boundaries
            sf::Vector2f pos = m_shape->getPosition();
            if (pos.x <= 0 || pos.x >= SCREEN_WIDTH - m_shape->getGlobalBounds().width)
            {
                m_velocity.x = -m_velocity.x;
                m_shape->setPosition(pos.x <= 0 ? 0 : SCREEN_WIDTH - m_shape->getGlobalBounds().width, pos.y);
            }
            if (pos.y <= 0 || pos.y >= SCREEN_HEIGHT - m_shape->getGlobalBounds().height)
            {
                m_velocity.y = -m_velocity.y;
                m_shape->setPosition(pos.x, pos.y <= 0 ? 0 : SCREEN_HEIGHT - m_shape->getGlobalBounds().height);
            }
        }
        else if (m_type == ParticleType::FIREFLY)
        {
            float angle = static_cast<float>(rand() % 360) * 3.14f / 180.0f;
            sf::Vector2f randomMovement(cos(angle) * 2.0f, sin(angle) * 2.0f);
            m_velocity += randomMovement;
        }

        // Move the particle
        m_shape->move(m_velocity * t_deltaTime.asSeconds());

        // Update the color alpha for fading effect
        m_alpha -= 255.0f * t_deltaTime.asSeconds() / m_lifetime;
        if (m_alpha < 0)
        {
            m_alpha = 0;
        }
        m_color.a = static_cast<sf::Uint8>(m_alpha);
        m_shape->setFillColor(m_color);
    }
}

void Particle::render(sf::RenderWindow& m_window)
{
    if (m_shape)
    {
        m_window.draw(*m_shape);
    }
    if (m_sprite)
    {
        m_window.draw(*m_sprite);
    }
}

bool Particle::isColliding(const Particle& m_other)
{
    if (m_shape && m_other.m_shape)
    {
        return m_shape->getGlobalBounds().intersects(m_other.m_shape->getGlobalBounds());
    }
    return false;
}

void Particle::handleCollision(Particle& m_other)
{
    sf::CircleShape* thisCircle = dynamic_cast<sf::CircleShape*>(m_shape);
    sf::CircleShape* otherCircle = dynamic_cast<sf::CircleShape*>(m_other.m_shape);

    if (thisCircle && otherCircle)
    {
        sf::Vector2f relativePosition = thisCircle->getPosition() - otherCircle->getPosition();
        float distance = std::sqrt(relativePosition.x * relativePosition.x + relativePosition.y * relativePosition.y);
        float overlap = 0.5f * (distance - thisCircle->getRadius() - otherCircle->getRadius());

        if (distance != 0) 
        {
            thisCircle->move(-overlap * (relativePosition.x / distance), -overlap * (relativePosition.y / distance));
            otherCircle->move(overlap * (relativePosition.x / distance), overlap * (relativePosition.y / distance));
        }

        // Calculate new velocities
        sf::Vector2f normal = relativePosition / distance;
        sf::Vector2f tangent(-normal.y, normal.x);

        float tangentVel1 = m_velocity.x * tangent.x + m_velocity.y * tangent.y;
        float tangentVel2 = m_other.m_velocity.x * tangent.x + m_other.m_velocity.y * tangent.y;

        float normalVel1 = m_velocity.x * normal.x + m_velocity.y * normal.y;
        float normalVel2 = m_other.m_velocity.x * normal.x + m_other.m_velocity.y * normal.y;

        float newVelocity1 = (normalVel1 * (thisCircle->getRadius() - otherCircle->getRadius()) + 2.0f * otherCircle->getRadius() * normalVel2) 
            / (thisCircle->getRadius() + otherCircle->getRadius());
        float newVelocity2 = (normalVel2 * (otherCircle->getRadius() - thisCircle->getRadius()) + 2.0f * thisCircle->getRadius() * normalVel1)
            / (thisCircle->getRadius() + otherCircle->getRadius());

        m_velocity = tangent * tangentVel1 + normal * newVelocity1;
        m_other.m_velocity = tangent * tangentVel2 + normal * newVelocity2;
    }
}

Particle::Particle(sf::Shape* m_shape, const sf::Vector2f& m_velocity, const sf::Color& m_color, float m_lifetime, ParticleType m_type)
    : m_shape(m_shape), m_velocity(m_velocity), m_color(m_color), m_lifetime(m_lifetime), m_type(m_type)
{
    m_alpha = 255;
}

Particle::Particle(sf::Sprite* m_sprite, const sf::Vector2f& m_velocity, const sf::Color& m_color, float m_lifetime, ParticleType m_type)
    : m_sprite(m_sprite), m_velocity(m_velocity), m_color(m_color), m_lifetime(m_lifetime), m_type(m_type)
{
    m_alpha = 255;
}

Particle::Particle(sf::Shader* m_shader, const sf::Vector2f& m_velocity, const sf::Color& m_color, float m_lifetime, ParticleType m_type)
    : m_shader(m_shader), m_velocity(m_velocity), m_color(m_color), m_lifetime(m_lifetime), m_type(m_type)
{
    m_alpha = 255;
}

ParticleManager::ParticleManager()
{
}

ParticleManager::~ParticleManager()
{
    clearAllParticles();
}

void ParticleManager::update(sf::Time t_deltaTime, ParticleType m_type)
{
    std::vector<Particle>* particles;

    if (m_type == ParticleType::FIREFLY)
    {
        particles = &m_fireflyParticles;
    }
    else if (m_type == ParticleType::BOUNCY_BALL)
    {
        particles = &m_bouncyBallParticles;
    }
    else
    {
        return;
    }

    for (auto it = particles->begin(); it != particles->end();)
    {
        it->update(t_deltaTime, *particles);
        if (it->m_age >= it->m_lifetime || it->m_alpha == 0)
        {
            it = particles->erase(it);
        }
        else
        {
            ++it;
        }
    }

    if (m_type == ParticleType::FIREFLY && particles->size() < 10)
    {
        createBackgroundEffect();
    }
}

void ParticleManager::render(sf::RenderWindow& m_window, ParticleType m_type)
{
    std::vector<Particle>* particles;

    if (m_type == ParticleType::FIREFLY)
    {
        particles = &m_fireflyParticles;
    }
    else if (m_type == ParticleType::BOUNCY_BALL)
    {
        particles = &m_bouncyBallParticles;
    }
    else
    {
        return;
    }

    for (auto& particle : *particles)
    {
        particle.render(m_window);
    }
}

void ParticleManager::addParticle(sf::Shape* m_shape, const sf::Vector2f& m_velocity, const sf::Color& m_color, float m_lifetime, ParticleType m_type)
{
    if (m_type == ParticleType::FIREFLY)
    {
        m_fireflyParticles.emplace_back(m_shape, m_velocity, m_color, m_lifetime, m_type);
    }
    else if (m_type == ParticleType::BOUNCY_BALL)
    {
        m_bouncyBallParticles.emplace_back(m_shape, m_velocity, m_color, m_lifetime, m_type);
    }
}

void ParticleManager::addParticle(sf::Sprite* m_sprite, const sf::Vector2f& m_velocity, const sf::Color& m_color, float m_lifetime, ParticleType m_type)
{
    if (m_type == ParticleType::FIREFLY)
    {
        m_fireflyParticles.emplace_back(m_sprite, m_velocity, m_color, m_lifetime, m_type);
    }
    else if (m_type == ParticleType::BOUNCY_BALL)
    {
        m_bouncyBallParticles.emplace_back(m_sprite, m_velocity, m_color, m_lifetime, m_type);
    }
}

void ParticleManager::addParticle(sf::Shader* m_shader, const sf::Vector2f& m_velocity, const sf::Color& m_color, float m_lifetime, ParticleType m_type)
{
    if (m_type == ParticleType::FIREFLY)
    {
        m_fireflyParticles.emplace_back(m_shader, m_velocity, m_color, m_lifetime, m_type);
    }
    else if (m_type == ParticleType::BOUNCY_BALL)
    {
        m_bouncyBallParticles.emplace_back(m_shader, m_velocity, m_color, m_lifetime, m_type);
    }
}

void ParticleManager::createBackgroundEffect()
{
    for (int i = 0; i < 10; ++i)
    {
        sf::CircleShape* particle = new sf::CircleShape(2);
        particle->setPosition(sf::Vector2f(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT));
        particle->setFillColor(sf::Color(255, 255, 0, 255));
        float m_randomLifetime = 3.0f + static_cast<float>(rand() % 3000) / 1000.0f;
        addParticle(particle, sf::Vector2f((rand() % 20 - 10) * 0.05f, (rand() % 20 - 10) * 0.05f), sf::Color(255, 255, 0, 255), m_randomLifetime, ParticleType::FIREFLY);
    }
}

void ParticleManager::createBouncyBallEffect(const sf::Vector2f& position)
{
    sf::CircleShape* particle = new sf::CircleShape(10);
    particle->setPosition(position);
    particle->setFillColor(sf::Color(rand() % 256, rand() % 256, rand() % 256, 255));
    float randomLifetime = 20.0f;
    sf::Vector2f initialVelocity(0.0f, 100.0f);
    addParticle(particle, initialVelocity, particle->getFillColor(), randomLifetime, ParticleType::BOUNCY_BALL);
}

void ParticleManager::clearParticles(ParticleType m_type)
{
    if (m_type == ParticleType::FIREFLY)
    {
        m_fireflyParticles.clear();
    }
    else if (m_type == ParticleType::BOUNCY_BALL)
    {
        m_bouncyBallParticles.clear();
    }
}

void ParticleManager::clearAllParticles()
{
    m_fireflyParticles.clear();
    m_bouncyBallParticles.clear();
}
