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
    sf::RectangleShape shape;
    sf::Clock flashClock;
    int health = 100;
    int maxHealth = 100;
    int damage = 100;
    bool traversable = false;
    bool flashState = false;

    bool takeDamage(int damageAmount) 
    {
        health -= damageAmount;
        return health <= 0;
    }

    float getHealthPercentage() const 
    {
        return (static_cast<float>(health) / maxHealth) * 100.0f;
    }

};

#endif
