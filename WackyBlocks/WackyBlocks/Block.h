#pragma once
#ifndef BLOCK_H
#define BLOCK_H

#include <SFML/Graphics.hpp>

enum class BlockType
{
    NONE,
    DIRT, // World blocks
    GRANITE,
    STONE,
    SAND,
    WATER,
    LAVA,
    TRAP_SPIKE, // Traps
    TRAP_BARREL,
    SKELETON, // Enemies
    EVIL_EYE,
    GOBLIN,
    MUSHROOM,
    ENEMY_DEMON_BOSS,
    HEALTH_PACK, // Misc
    AMMO_PACK,
    TORCH,
    SHOP,
    PLAYER, // Essentials
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
