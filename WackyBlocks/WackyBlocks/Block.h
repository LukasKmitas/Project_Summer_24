#pragma once
#ifndef BLOCK_H
#define BLOCK_H

#include <SFML/Graphics.hpp>

enum class BlockType
{
    NONE,
    DIRT,
    GRANITE,
    STONE,
    SAND,
    WATER,
    LAVA,
    TRAP_SPIKE,
    TRAP_BARREL,
    SLIME,
    EVIL_EYE,
    SQUIG,
    ENEMY_BOSS,
    HEALTH_PACK,
    AMMO_PACK,
    PLAYER,
    END_GATE
};

struct Block
{
    BlockType type = BlockType::NONE;
    int health = 0;
    int damage = 0;
    bool traversable = false;
    sf::RectangleShape shape;
};

#endif
