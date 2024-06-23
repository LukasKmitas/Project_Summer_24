#pragma once
#ifndef LEVELEDITOR_H
#define LEVELEDITOR_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include "Global.h"
#include "SoundManager.h"

enum class TabType
{
    BLOCKS,
    TRAPS,
    ENEMIES,
    ESSENTIALS,
    MISC
};

enum class BlockType
{
    NONE,
    PLAYER,
    HEALTH_PACK,
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
    SQUIG
};

struct Block
{
    BlockType type;
    int health;
    int damage;
    bool traversable;
    sf::RectangleShape shape; 
};

class LevelEditor
{
public:

    LevelEditor();
    ~LevelEditor();

    void update(sf::Time t_deltaTime);
    void render(sf::RenderWindow& m_window);

    void handleMouseClick(sf::Vector2f m_mousePos);
    void handleMouseHover(sf::Vector2f m_mousePos);
    void handleMouseRelease();

    void reset();
    bool isBackButtonPressed() const;

private:

    void loadTextures();
    void setupUI();
    void setupTabs();
    void setupGrid();
    void createTileBlocks();
    void toggleSlider();
    void updateSliderPosition(sf::Time t_deltaTime);
    void updateTabName();
    void updateTileBlocks();
    void selectBlock(sf::Vector2f m_mousePos);
    void placeBlock(sf::Vector2f m_mousePos);

    TabType m_currentTab;

    sf::Font m_font;
    sf::Text m_openSliderText;
    sf::Text m_tabNameText;
    sf::Text m_nextTabText;
    sf::Text m_prevTabText;
    sf::Text m_backButtonText;
    sf::Text m_saveButtonText;

    sf::RectangleShape m_openSliderButton;
    sf::RectangleShape m_sliderPanel;
    sf::RectangleShape m_nextTabButton;
    sf::RectangleShape m_prevTabButton;
    sf::RectangleShape m_backButton;
    sf::RectangleShape m_saveButton;

    float m_sliderPosition;

    bool m_backButtonPressed = false;
    bool m_saveButtonPressed = false;
    bool m_backButtonHovered = false;
    bool m_saveButtonHovered = false;
    bool m_openSliderButtonHovered = false;
    bool m_nextTabButtonHovered = false;
    bool m_prevTabButtonHovered = false;
    bool m_sliderOpen = false;

    static const int GRID_ROWS = 8;
    static const int GRID_COLS = 4;
    sf::RectangleShape m_grid[GRID_ROWS][GRID_COLS];
    std::vector<sf::RectangleShape> m_tileBlocks;

    sf::Texture m_dirtTexture;
    sf::Texture m_graniteTexture;
    sf::Texture m_stoneTexture;
    sf::Texture m_sandTexture;
    sf::Texture m_spikeTexture;
    sf::Texture m_barrelTexture;
    sf::Texture m_slimeTexture;
    sf::Texture m_evilEyeTexture;
    sf::Texture m_squigTexture;
    sf::Texture m_playerTexture;
    sf::Texture m_healthPackTexture;

    std::vector<Block> m_mapBlocks;
    BlockType m_selectedBlockType = BlockType::NONE;
    int m_selectedBlockIndex = -1;

};

#endif