#ifndef FLYINGEYE_H
#define FLYINGEYE_H
#pragma once

#include "Enemy.h"

class EvilEye : public Enemy
{
public:

    EvilEye(const sf::Vector2f& m_position);

    void update(sf::Time m_deltaTime) override;

private:

    void loadTextures();
    void loadFrames();

};

#endif